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
#include "game.h"
#include "interface.h"
#include "params.h"
#include "util.h"
#include "mfp_memory_mapped_registers.h"


#ifdef SDL2
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
SDL_Window *window;
SDL_Renderer *renderer;
#else
#endif

// this file has main(), runs all the init stuff and runs the main menu (if we have one)
// calls game() with number of players

int main () {
#ifdef SDL2
	// calculate & print sound frequency targets
	// its just easier to have a computer do it than do it by hand
	long MYCLOCKFORSOUND = 100000000; // 100_000_000 = 100Mhz

	print("SOUND MATH: player explode\n");
	int freq = 64;
	print("64hz dropping by 4hz every 0.06s, sound periods=\n");
	for (int i = 0; i < 10; i++) {
		print("%li\n", MYCLOCKFORSOUND / freq);
		freq -= 4;
	}
	print("duration for 0.06s=\n");
	print("%li\n", (MYCLOCKFORSOUND * 6) / 100);

	print("SOUND MATH: enemy explode\n");
	freq = 120;
	print("120hz dropping by 8hz every 0.03s, sound periods=\n");
	for (int i = 0; i < 10; i++) {
		print("%li\n", MYCLOCKFORSOUND / freq);
		freq -= 8;
	}
	print("duration for 0.03s=\n");
	print("%li\n", (MYCLOCKFORSOUND * 3) / 100);

	print("SOUND MATH: player shoot\n");
	freq = 3000;
	print("3000hz dropping by 150hz every 0.02s, sound periods=\n");
	for (int i = 0; i < 10; i++) {
		print("%li\n", MYCLOCKFORSOUND / freq);
		freq -= 150;
	}
	print("duration for 0.02s=\n");
	print("%li\n", (MYCLOCKFORSOUND * 2) / 100);

#endif

	// load the sprite tables and whatever SDL needs to set up
	init();

	// load the slots that always disp the same thing (just on or off) with the correct sprite index
	// anything whose sprite changes/animates is loaded later
	// bullets
	for(int i=0; i<MAX_BULLETS; i++) {
		write_sprite_idx(SPRITE_BULLET_0 + i, PALETTE_BULLET);
	}
	// lives (not ship)
	write_sprite_idx(SPRITE_LIFE_A, PALETTE_SHIP);
	write_sprite_idx(SPRITE_LIFE_B, PALETTE_SHIP);
	// labels
	write_sprite_idx(SPRITE_SCORE_A, PALETTE_LABEL_SCORE_A);
	write_sprite_idx(SPRITE_SCORE_B, PALETTE_LABEL_SCORE_B);
	write_sprite_idx(SPRITE_WAVE_A, PALETTE_LABEL_WAVE_A);
	write_sprite_idx(SPRITE_WAVE_B, PALETTE_LABEL_WAVE_B);

	// load the x-position of the icons that dont move (labels, numbers, bunkers, lives)
	// anything whose x-pos changes is loaded later
	// digits(score+wave)
	for(int i=0; i<NUM_DIGITS; i++) {
		write_sprite_xpos(SPRITE_SCORE_DIG_0 + NUM_DIGITS - 1 - i, SCORE_BASE_X + (i * SCORE_SPACING));
	}
	write_sprite_xpos(SPRITE_WAVE_DIG, WAVE_BASE_X);
	// bunkers
	for(int j=0; j<NUM_BUNKERS; j++) { for(int k=0; k<BUNKER_ROWS; k++) { for(int l=0; l<BUNKER_COLS; l++) {
		write_sprite_xpos(SPRITE_BUNKER_X(j,k,l), BUNKER_BASE_X + (j*BUNKER_TO_BUNKER) + (l * BUNKER_SPACING_X));
	}}}
	// lives (not ship)
	write_sprite_xpos(SPRITE_LIFE_A, LIVES_BASE_X);
	write_sprite_xpos(SPRITE_LIFE_B, LIVES_BASE_X + LIVES_SPACING);
	// labels
	write_sprite_xpos(SPRITE_SCORE_A, SCORE_LABEL_BASE_X);
	write_sprite_xpos(SPRITE_SCORE_B, SCORE_LABEL_BASE_X + SCORE_LABEL_SPACING);
	write_sprite_xpos(SPRITE_WAVE_A, WAVE_LABEL_BASE_X);
	write_sprite_xpos(SPRITE_WAVE_B, WAVE_LABEL_BASE_X + WAVE_LABEL_SPACING);

	// set sprites to be off/disabled by moving off screen in Y axis
	// disable literally EVERYTHING, selectively re-enable later
	for(int i=0; i<128; i++) {
		write_sprite_ypos(i, DISABLE_SPRITE_Y);
	}

	// set up 7seg i guess
	write_7seg_enable(0xFF);
	write_7seg_dec(0x00);
	write_7seg_high(0x01020304);
	write_7seg_low( 0x05060708);
	// no music on main menu
	write_audio_musicstate(AUDIO_STOPMUSIC);

	print("done with bootup init\n");


	int seeded = 0;
	// this infinite loop ensures the program never actually ends; you always return to the main menu and pick again
	while (1) {
		print("waiting for mode selection");

		// TODO stretch: load the menu options (one ship and two ships)
		// TODO stretch: load the cursor (explosion)

		long butts = 0;
#ifdef FAST
		int ready = 2; // DEBUG: skipping directly to running state
#else
		int ready = 0;
#endif
		// load the LEDs with the pattern that's supposed to flash
		write_leds(0xFF00);
		// wait until any button is released:
		while (ready==0) {
			// read buttons
			butts = read_butts();

			// once any button transitions from high to low, then this returns a 1 on the corresponding bit
			ready = wait_for_button_release(butts);

			// in the meantime, be animating the LEDs to indicate we are waiting
			// is it time to blink the LEDs?
			long now = read_timer1();
			if (now > TIMERCNT_6_HZ) {
				// wrap the timer
				write_timer1(now - TIMERCNT_6_HZ);
				// toggle the LEDs
				int t = read_leds();
				write_leds(~t);
			}

			// also, read the XADC and display its RAW value on 7seg
			// mostly for debug/calibration but its also neat to look at
			// read xadc A
			int raw_xadc_a = read_xadc_a();
			// convert to base-ten
			long result_a[2] = { 0,0 };
			to_base_ten(raw_xadc_a, 0, result_a);
			// write into bottom 4 7seg digits
			write_7seg_low(result_a[0]);
			// read xadc B
			int raw_xadc_b = read_xadc_b();
			// convert to base-ten
			long result_b[2] = { 0,0 };
			to_base_ten(raw_xadc_b, 0, result_b);
			// write into top 4 7seg digits
			write_7seg_high(result_b[0]);

			// TODO stretch: also, be updating the cursor position based on the analog inputs or whatever

			render_main();

		} // "waiting for button press-release" loop

		if (!seeded) {
			seed(1234);
			// set the flag so I don't seed it again; only seed exactly once
			seeded = 1;
		}
		print("done waiting for start\n");

		// a button has been pressed and released
		// TODO stretch: which menu option is selected?? determine and execute accordingly!!

		// for now, only one option is supported, so it can run unconditionally

		// begin game, wait for it to return with score
		long newscore = 0;
		if (read_switches()) {
			newscore = play_one_game(2);
		} else {
			newscore = play_one_game(1);
		}
		print("game is done\n");
		// TODO stretch: handle returned score, maybe?

	}

	return 0;
}
