/*
 ************************************************************************
 * "Invaders from Space"                                                *
 * Brian Henson, Jamie Williams, Bradon Kanyid, Grant Vesely            *
 * Portland State University, ECE 540 SoC Design with FPGAs             *
 * 3/22/2019                                                            *
 * We worked really hard on this so please dont steal it and            *
 * pretend like its yours, ok? That would be a dick move.               *
 *                                                                      *
 * This program is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************
 */
#include "mfp_memory_mapped_registers.h"
#include "params.h"

#include "game.h"

#include "wave.h"
#include "util.h"
#include "interface.h"

long current_game_score;
long current_game_lives;
enum bunker_record_enum bunker_record[NUM_BUNKERS][BUNKER_ROWS][BUNKER_COLS];


// PLAY ONE GAME
// return score @ loss
// takes 1-player/2-player as argument



// preserved between waves: lives, score, bunkers
// want to have some delay before each wave and after death before returning to menu

long play_one_game(int numplayers) {
	// load/enable game background (labels)
	// don't need to set the displayed sprite, nor the x-pos... just move them on-screen
	write_sprite_ypos(SPRITE_SCORE_A, SCORE_LABEL_BASE_Y);
	write_sprite_ypos(SPRITE_SCORE_B, SCORE_LABEL_BASE_Y);
	write_sprite_ypos(SPRITE_WAVE_A, WAVE_LABEL_BASE_Y);
	write_sprite_ypos(SPRITE_WAVE_B, WAVE_LABEL_BASE_Y);

	// set initial score
	current_game_score = 0;
	// display initial score
	calc_and_draw_score(current_game_score);
	// set initial lives
	current_game_lives = 2;
	// display initial lives
	write_sprite_ypos(SPRITE_LIFE_A, LIVES_BASE_Y);
	write_sprite_xpos(SPRITE_LIFE_A, LIVES_BASE_X);
	write_sprite_ypos(SPRITE_LIFE_B, LIVES_BASE_Y);
	write_sprite_xpos(SPRITE_LIFE_B, LIVES_BASE_X + LIVES_SPACING);

	// set player ship sprite to center of the screen or something, also alive
	for (int z = 0; z < numplayers; z++) {
		write_sprite_xpos(SPRITE_SHIP+z, SHIP_BASE_X+(z*100));
		write_sprite_ypos(SPRITE_SHIP+z, SHIP_BASE_Y);
		write_sprite_idx(SPRITE_SHIP+z, z ? PALETTE_SHIP_B : PALETTE_SHIP);
	}
	//print("test %i %i %i %i\n", SPRITE_SHIP, read_sprite_xpos(SPRITE_SHIP), read_sprite_ypos(SPRITE_SHIP), read_sprite_idx(SPRITE_SHIP));
	//print("test %i %i %i %i\n", SPRITE_SHIP+1, read_sprite_xpos(SPRITE_SHIP + 1), read_sprite_ypos(SPRITE_SHIP + 1), read_sprite_idx(SPRITE_SHIP + 1));



	// enable wave #
	write_sprite_ypos(SPRITE_WAVE_DIG, WAVE_BASE_Y);

	// init bunker_record to be the shape/arrangement we want
	// init bunker sprites the same way
	for(int j=0; j<NUM_BUNKERS; j++) { for(int k=0; k<BUNKER_ROWS; k++) { for(int l=0; l<BUNKER_COLS; l++) {

		write_sprite_ypos(SPRITE_BUNKER_X(j,k,l), BUNKER_BASE_Y + (k*BUNKER_SPACING_Y));
		// don't write the xpos, it never changes so its done in boot-init

		if(k==0 && l==0) {
			// if k==0 and l==0: lcorner
			bunker_record[j][k][l] = BUNKER_LOUT_A;
			write_sprite_idx(SPRITE_BUNKER_X(j,k,l), PALETTE_BUNKER_LOUT_A);
		} else if(k==0 && l==2) {
			// if k==0 and l==2: rcorner
			bunker_record[j][k][l] = BUNKER_ROUT_A;
			write_sprite_idx(SPRITE_BUNKER_X(j,k,l), PALETTE_BUNKER_ROUT_A);
		#ifdef HOLLOW_BUNKERS
		} else if(k==1 && l==1) {
			// empty slot (might give up on this plan)
			bunker_record[j][k][l] = BUNKER_DEAD;
			write_sprite_ypos(SPRITE_BUNKER_X(j,k,l), DISABLE_SPRITE_Y);
		#endif
		} else {
			// otherwise: solid
			bunker_record[j][k][l] = BUNKER_FULL_A;
			write_sprite_idx(SPRITE_BUNKER_X(j,k,l), PALETTE_BUNKER_FULL_A);
		}
	}}}

	int wavenum = 0;
	int beat_it = 1;
	while(beat_it) {
		wavenum += 1;
		// move all bullets offscreen (they show the right sprite already, dont care about their x)
		for(int i=0; i<MAX_BULLETS; i++) {
			write_sprite_ypos(SPRITE_BULLET_0 + i, DISABLE_SPRITE_Y);
		}
		// display wave #: change it to display the right #
		write_sprite_idx(SPRITE_WAVE_DIG, PALETTE_NUM0 + wavenum);

		// display enemies in their starting positions (set to alive sprite, set correct x and y)
		// 2d iterate
		for(int row=0; row<ENEMY_ROWS; row++) {for(int col=0; col<ENEMY_COLS; col++) {
			write_sprite_xpos(SPRITE_ENEMY_X(row,col), ENEMY_BASE_X + (col*ENEMY_SPACING_X));
			write_sprite_ypos(SPRITE_ENEMY_X(row,col), ENEMY_BASE_Y + (row*ENEMY_SPACING_Y));
			write_sprite_idx( SPRITE_ENEMY_X(row,col), PALETTE_ENEMY_A1 + (row%4));
		}}


		////////////////
		// at this point it looks like a real game, except its frozen
		draw();

#ifndef FAST
		print("play sound: wave-start");
		// TODO sound: play wave-start fanfare
		write_audio_soundfx(AUDIO_WAVESTART);
		// wait until the fanfare is done playing
		write_timer0(0);
		while(read_timer0() < WAVESTART_SOUND_LENGTH) {}
#endif

		// run the wave & capture the return value
		beat_it = run_one_wave(wavenum, numplayers);
		// if beat_it is 1, we beat the wave, so keep going - otherwise quit
		/////////////////
		write_audio_musicstate(AUDIO_STOPMUSIC);

		}
	// if you hit here, game over!

#ifndef FAST
	print("play sound: game-over");
	// TODO sound: play game over sound & freeze?
	write_audio_soundfx(AUDIO_GAMEOVER);
	// wait until sound is done playing
	write_timer0(0);
	while(read_timer0() < GAMEOVER_SOUND_LENGTH) {}
#endif

	// "cleanup" before returning to main menu!
	// hide ship
	for (int z = 0; z < numplayers; z++) {
		write_sprite_ypos(SPRITE_SHIP+z, DISABLE_SPRITE_Y);
	}
	// hide lives
	write_sprite_ypos(SPRITE_LIFE_A, DISABLE_SPRITE_Y);
	write_sprite_ypos(SPRITE_LIFE_B, DISABLE_SPRITE_Y);
	// hide bullets
	for(int i=0; i<MAX_BULLETS; i++) {
		write_sprite_ypos(SPRITE_BULLET_0 + i, DISABLE_SPRITE_Y);
	}
	// hide score
	for (int i = 0; i < NUM_DIGITS; i++) {
		write_sprite_ypos(SPRITE_SCORE_DIG_0 + i, DISABLE_SPRITE_Y);
	}
	// hide wave #
	write_sprite_ypos(SPRITE_WAVE_DIG, DISABLE_SPRITE_Y);

	// hide background(labels)
	write_sprite_ypos(SPRITE_SCORE_A, DISABLE_SPRITE_Y);
	write_sprite_ypos(SPRITE_SCORE_B, DISABLE_SPRITE_Y);
	write_sprite_ypos(SPRITE_WAVE_A,  DISABLE_SPRITE_Y);
	write_sprite_ypos(SPRITE_WAVE_B,  DISABLE_SPRITE_Y);

	// hide enemies... 2D iterate
	for(int j=0; j<ENEMY_ROWS; j++) {
		for(int k=0; k<ENEMY_COLS; k++) {
			write_sprite_ypos(SPRITE_ENEMY_X(j,k), DISABLE_SPRITE_Y);
		}
	}
	// hide bunkers
	for(int j=0; j<NUM_BUNKERS; j++) {
		for(int k=0; k<BUNKER_ROWS; k++) {
			for(int l=0; l<BUNKER_COLS; l++) {
				write_sprite_ypos(SPRITE_BUNKER_X(j,k,l), DISABLE_SPRITE_Y);
			}
		}
	}

	// other stuff?

	// return with the player's score
	return current_game_score;
}
