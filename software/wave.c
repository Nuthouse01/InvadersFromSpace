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
#include "wave.h"
#include "util.h"
#include "interface.h"

extern long current_game_score;
extern long current_game_lives;
extern enum bunker_record_enum bunker_record[NUM_BUNKERS][BUNKER_ROWS][BUNKER_COLS];

// RUN ONE WAVE
// return 0 for game over, return 1 for victory
// take some sort of wave # or other "difficulty parameters"

// NOTE: long=4bytes=32bit, int=2bytes=16bit, char=1byte=8bit


// NOTE: because the lookup time would be the same (2cycle per value) we may
// as well be just reading and holding data in the AHB sprite module rather
// than keeping a copy in our memory


/*
 * TODO: (core goal list) (%%%=done)
 * %%%fill out the sprite-control-reg accessing functions for SDL
 * %%%slot those functions into the places they'll be used
 * %%%score calculation & display
 * %%%interface.c functions for 7seg
 * %%%"load_sprites"
 * %%%start-of-game init
 * %%%end-of-game cleanup
 * %%%figure out how the bunkers look
 * %%%figure out how the bunkers are implemented
 * %%%translate XADC to x-position
 * %%%MIPS quite and SDL pause
 * %%%bootup init
 * %%%SDL generic display-everything call
 * %%%enemy movement pattern
 * %%%game loss from enemy movement
 * %%%bunker destroy from enemy movement
 *
 * %%%actual sprite artwork
 * %%%reconfigure object positions for 640x480 display
 */



/*
 * TODO: (stretch goal list)
 * %%%enemies move faster/shoot more as they die
 * SOUND
 * special enemy
 * "piercing shot" powerup
 * main menu graphics
 * %%%2-player
 * high score tracker (not saved thru powerdown tho)
 */


/////////////////////////////////////////////////
// global vars (things I can/will use from within functions)

long gametime;		// gameticks
int player_shoot_cooldown[2]; // player can or cannot shoot
enum bullet_record_enum bullet_record[MAX_BULLETS]; // lists status/ownership of each bullet slot
enum player_status_enum player_status[2];	// player alive/dead
enum enemy_record_enum enemy_record[ENEMY_ROWS][ENEMY_COLS]; // lists status of each enemy (NOTE: could be replaced by reading & interpreting currently displayed sprite for each enemy, but that's confusing so nah)
int enemy00_x;
int enemy00_y;


/////////////////////////////////////////////////
// targets
// IDEA: consider making these some sort of struct-thing, not much benefit but they'd be grouped into
// adjacent memory addresses so thats kinda neat i guess?

//... asynchronous ones do need to be global, tho, so they can be set from any func
int player_shoot_cooldown_target[2];
int player_animate_target[2];
int enemy_animate_target[ENEMY_ROWS][ENEMY_COLS];

//////////////////////////////////////////////////
// main function


// returns 0 if you died and returns 1 if you beat the wave
int run_one_wave(int wavenum, int numplayers) {

	///////////////////////////////////////////////////
	// wave init
	print("wave start\n");

	enemy00_x = ENEMY_BASE_X;
	enemy00_y = ENEMY_BASE_Y;

	int eshoot_floor = ESHOOT_FLOOR_BASE;						// 0.4s
	int eshoot_ceil = ESHOOT_CEIL_BASE - ESHOOT_FLOOR_BASE;		// 5.0s
	// ceiling drops by 0.7s per wave past the first
	eshoot_ceil -= ((wavenum - 1) * ESHOOT_CEILDROP_PER_WAVE);
	// ceiling
	if (eshoot_ceil <= eshoot_floor) {
		eshoot_ceil = eshoot_floor + 1;
	}
	print("wave%i, eshoot range=%i-%i\n", wavenum, eshoot_floor, eshoot_ceil);

	int delay_moveenemy = DELAY_MOVEENEMY_BASE;

	gametime = 0; //global
	int num_enemies_remaining = ENEMY_COLS*ENEMY_ROWS; // wave-win condition

	int max_gameticks_jumped = 0;

	// "bullet record"
	// keeps track of which bullets are disabled=0, enemy=1, player=2
	// this is critical, cannot be replaced with reading the sprites or anything
	for (int j=0; j<MAX_BULLETS; j++) {
		bullet_record[j] = BULLET_OFF;
	}
	// "enemy record"
	// keeps track of which enemies are alive/dead/exploding without having to think about sprite addresses
	// this could potentially be eliminated and instead read the currently displayed sprite for each enemy to know its state
	// but its simpler to think about if its internally held like this
	for(int j=0; j<ENEMY_ROWS; j++) { for(int k=0; k<ENEMY_COLS; k++) {
			enemy_record[j][k] = ENEMY_ALIVE;
		}}

	// "player status"
	// keeps track of player's state: alive, explodeA, explodeB, gone
	// this could potentially be eliminated and instead read the currently displayed sprite for the ship to know its state
	// but its simpler to think about if its internally held like this
	player_status[0] = PLAYER_ALIVE; //global
	// 0=player can shoot, 1=player cannot shoot (cooldown or dead)
	player_shoot_cooldown[0] = 0; //global

	// give this an argument of 1 to reset the static variables
	move_enemies(1);


	// DO NOT initialize bunker_record here, that is extern and carried over between waves! dont even draw it

	// initialize targets to get the ball rolling
	int r = get_random();
	int eshoot_target =     new_target((r % eshoot_ceil) + eshoot_floor);
	int moveplayer_target = new_target(DELAY_MOVEPLAYER);
	int movebullet_target = new_target(DELAY_MOVEBULLET);
	int moveenemy_target =  new_target(delay_moveenemy);

	// initially disable asynch targets
	player_shoot_cooldown_target[0] = TARGET_OFF;
	player_animate_target[0] = TARGET_OFF;
	for (int j = 0; j < ENEMY_ROWS; j++) {
		for (int k = 0; k < ENEMY_COLS; k++) {
			enemy_animate_target[j][k] = TARGET_OFF;
		}
	}


	if (numplayers == 2) {
		player_shoot_cooldown_target[1] = TARGET_OFF;
		player_animate_target[1] = TARGET_OFF;
		player_status[1] = PLAYER_ALIVE; //global
		player_shoot_cooldown[1] = 0; //global
	}

	///////////////////////////////////////////////////
	// wave loop

	write_timer0(0); // reset timer, should happen immediately before loop begins

	// infinite loop:
	// when last enemy explosion has dissipated, return 1 (win)
	// when player explosion has dissipated, return 0 (lose)
	// when aliens reach the ground, return 0 (lose)
	while (1) {
		// if(update_gametime() != 0) {continue;} // if gametime didn't change, no reason to test against it again
		// advance the gametime, how many ticks were jumped?
		//update_gametime();
		int gameticks_jumped = update_gametime();
		if (gameticks_jumped > max_gameticks_jumped) {
			// if a new max jump has happened, save it & write it to 7seg DECIMAL POINTS
			max_gameticks_jumped = gameticks_jumped;
			write_7seg_dec(gameticks_jumped);
		}

		draw();

		// TARGET: move player
		// (very fast but doesn't need to be each main-loop)
		if (time_to_run_target(moveplayer_target)) {
			// first, move the target for next pass
			moveplayer_target = new_target(DELAY_MOVEPLAYER);
			// do the thing

			// handle pause, handles everything to do with pausing the system
			// button press -> button release -> busywait -> button press -> button release -> move on
			// if the user quits, it returns 1 and I return 0
			int retval = check_pausequit();
			if (retval != 0) {
				hit_players(retval - 1);
				//return 0;
			}

			// note: if you are dead, you cannot move, cannot be hit, and cannot shoot! (handled inside each function)

			// check whether they are able to shoot and are trying to shoot
			shoot_players(numplayers);

			// read the input and change the player's displayed position
			move_players(numplayers);

			// *run ebeam collision (ebeams vs player), return 1 if player died
			collision_ebullet(numplayers);

		}

		// TARGET: move bullets
		// move bullets (player bullets up, enemy bullets down)
		if(time_to_run_target(movebullet_target)) {
			// first, move the target for next pass
			movebullet_target = new_target(DELAY_MOVEBULLET);

			int c = move_bullets();

			// if no bullets were moved, then no bullets are on-screen, and there's no reason to run collision
			if (c != 0) {
				// run all 3 flavors of collision-checking:
				// *run ebeam collision (ebeams vs player), return 1 if player died
				collision_ebullet(numplayers);
				// *run pbeam collision (pbeams vs enemies), return # of enemies that died
				c = collision_pbullet();
				if(c) {
					num_enemies_remaining -= c;
					// increase enemy move speed as enemies die
					delay_moveenemy = ((num_enemies_remaining * DELAY_MOVEENEMY_BASE) / (ENEMY_ROWS*ENEMY_COLS)) + DELAY_MOVEENEMY_MIN;
				}
				// *run bunker collision (pbeams vs bunker, ebeams vs bunker)
				// returns # of things hit, but we have no use for it
				collision_bunker();
			}
		} // end of MOVE BULLETS


		// TARGET: move enemies
		if (time_to_run_target(moveenemy_target)) {
			// first, move the target for next pass
			// (note: frequency will change as more enemies die???)
			// (for now it will just be constant)
			moveenemy_target = new_target(delay_moveenemy);
			// note: if no enemies are alive, don't move!
			// don't want the dead enemies to keep invisibly moving while the final explosion is dissipating and reach
			// the bottom and cause a game-over, that wouldn't make any sense

			if (num_enemies_remaining != 0) {
				// returns 1 if the enemies reached the ground, so we lost
				if (move_enemies(0)) { return 0; }

				// *run pbeam collision (pbeams vs enemies), return # of enemies that died
				int c = collision_pbullet();
				if (c) {
					num_enemies_remaining -= c;
					delay_moveenemy = ((num_enemies_remaining * DELAY_MOVEENEMY_BASE) / (ENEMY_ROWS*ENEMY_COLS)) + DELAY_MOVEENEMY_MIN;
				}
			}

		} // end of MOVE ENEMIES



		// TARGET: ENEMY SHOOT (sorta periodic? sorta nonperiodic?)
		// 1 or 2 enemies fire, random time between firings (with floor & ceil)
		if (time_to_run_target(eshoot_target)) {
			// before each shot, generate a random number w/ some ceil and floor... that is delay until this triggers next
			// ceil gets lower as wave progresses(maybe?), therefore fires more frequently
			int r = get_random();
			eshoot_target = new_target((r % eshoot_ceil) + eshoot_floor);

			// do the actual shooting
			shoot_enemy();

		} // end of ENEMY SHOTS


		// other targets & events?
		// TODO stretch: spawn special enemy?




		// animations!!! oneshot "asynchronous/nonperiodic" events
		// TARGET: animate enemy explosions
		for (int j = 0; j < ENEMY_ROWS; j++) {
			for (int k = 0; k < ENEMY_COLS; k++) {
				if ((enemy_record[j][k] != ENEMY_ALIVE) && time_to_run_target(enemy_animate_target[j][k])) {
					// if this returns 1, then we won! so we should return 1
					// otherwise, the game continues
					int c = animate_explosion_enemy(j, k, num_enemies_remaining);
					if (c == 1) { return 1; }
				}
			}
		}

		// for each player,
		for (int z = 0; z < numplayers; z++) {
			// TARGET: player shot cooldown wears off
			if ((player_shoot_cooldown[z]) && time_to_run_target(player_shoot_cooldown_target[z])) {
				// when the cooldown expires, then you can disengage the lockout
				player_shoot_cooldown[z] = 0;
				// disable the target
				player_shoot_cooldown_target[z] = TARGET_OFF;
			}

			// TARGET: animate player explosion (and respawn)
			if ((player_status[z] != PLAYER_ALIVE) && time_to_run_target(player_animate_target[z])) {
				// if this returns 1, then we lost! so we should return 0
				// otherwise, the game continues
				int c = animate_explosion_player(z);
				if (c == 1) { return 0; }
			}
		}
	} // end of WAVE LOOP

	// doesn't actually hit here, "return"s out of the loop instead of breaking

	// return 0 to stop playing (game over)
	// return 1 to play another wave (beat the wave)
	return 0;
}








//////////////////////////////////////////////////
// other "event" functions


// return 1 = game over, return 0 = nothing exciting happened
int animate_explosion_player(int z) {
	// PLAYER_ALIVE -> PLAYER_EXPLODING_A
	// (done by player_hit())
	switch (player_status[z]) {
	case PLAYER_EXPLODING_A:
		// PLAYER_EXPLODING_A -> PLAYER_EXPLODING_B
		// advance the state
		player_status[z] = PLAYER_EXPLODING_B;
		// change the target
		player_animate_target[z] = new_target(PLAYER_EXPLODE_SPEED);
		// change the displayed sprite
		write_sprite_idx(SPRITE_SHIP + z, PALETTE_EXPLODE_B);
		break;
	case PLAYER_EXPLODING_B:
		// PLAYER_EXPLODING_B -> PLAYER_DEAD
		// advance the state
		player_status[z] = PLAYER_DEAD;
		// change the target
		player_animate_target[z] = new_target(PLAYER_EXPLODE_SPEED);
		// disable the sprite
		write_sprite_ypos(SPRITE_SHIP + z, DISABLE_SPRITE_Y);
		break;
	case PLAYER_DEAD:
		// PLAYER_DEAD -> PLAYER_ALIVE (respawn)
		// does the player have lives remaining to use to respawn?
		if (current_game_lives > 0) {
			print("respawning player%i\n", z);
			// lose a life
			current_game_lives -= 1;

			// remove one life icon from screen
			switch (current_game_lives) {
			case 1: // if one life is left, disable life A
				write_sprite_ypos(SPRITE_LIFE_A, DISABLE_SPRITE_Y);
				break;
			case 0: // if no lives are left, disable life B (life A should already have been disabled)
				//write_sprite_ypos(SPRITE_LIFE_A, DISABLE_SPRITE_Y);
				write_sprite_ypos(SPRITE_LIFE_B, DISABLE_SPRITE_Y);
				break;
			default:
				break;
			}

			// advance the state
			player_status[z] = PLAYER_ALIVE;
			// disable the target
			player_animate_target[z] = TARGET_OFF;
			// re-enable the ship
			write_sprite_ypos(SPRITE_SHIP + z, SHIP_BASE_Y);
			// change the displayed sprite
			write_sprite_idx(SPRITE_SHIP + z, (z ? PALETTE_SHIP_B : PALETTE_SHIP));
		} else {
			// if there are no lives remaining, then its GAME OVER!
			return 1;
		}
		break;
	default:
		break;
	}
	return 0;
}



// return 1 = wave complete, return 0 = nothing exciting happened
int animate_explosion_enemy(int j, int k, int num_enemies_remaining) {
	// ENEMY_ALIVE -> ENEMY_EXPLODING_A
	// (done by enemy_hit())
	int bay = SPRITE_ENEMY_X(j, k);
	switch (enemy_record[j][k]) {
	case ENEMY_EXPLODING_A:
		// ENEMY_EXPLODING_A -> ENEMY_EXPLODING_B
		// advance the state
		enemy_record[j][k] = ENEMY_EXPLODING_B;
		// set the new target
		enemy_animate_target[j][k] = new_target(ENEMY_EXPLODE_SPEED);
		// change the sprite
		write_sprite_idx(bay, PALETTE_EXPLODE_B);
		break;
	case ENEMY_EXPLODING_B:
		// ENEMY_EXPLODING_B -> ENEMY_DEAD
		// advance the state
		enemy_record[j][k] = ENEMY_DEAD;
		// disable the target
		enemy_animate_target[j][k] = TARGET_OFF;
		// disable the sprite
		write_sprite_ypos(bay, DISABLE_SPRITE_Y);

		// check to see if you might've won
		if (num_enemies_remaining == 0) {
			// double-check that all explosions have dissipated
			int foobar = 0;
			for (int j = 0; j < ENEMY_ROWS; j++) {
				for (int k = 0; k < ENEMY_COLS; k++) {
					if (enemy_record[j][k] != ENEMY_DEAD) { foobar += 1; }
				}
			}
			// if all enemies are indeed dead and their explosions are done, THEN we win
			if (foobar == 0) { return 1; }
		}
		break;
	default:
		break;
	}
	// if we don't win, the game continues
	return 0;
}




// write the score to the screen AND the 7segs!
void calc_and_draw_score(long newscore) {
	print("new score: %li\n", newscore);

	// convert the score into the proper format
	long result[2] = {0};
#ifdef SCORE_LEFT_BLANKPAD
	to_base_ten(newscore, 1, result);
#else
	to_base_ten(newscore, 0, result);
#endif
	// write to 7seg
	// (SDL is handled inside these)
	write_7seg_low(result[0]);
	write_7seg_high(result[1]);

	// write to digit sprites
	// (SDL is handled inside these too)
	// low sprites
	for(int i=0; i<4; i++) {
		int val = result[0] >> (i*8); // extract the value for this digit
		val &= 0x1f; // mask
		// if val == 31, digit is disabled! move it off screen
		// else, digit is normally enabled, move it back on screen & change displayed sprite
		write_sprite_ypos(SPRITE_SCORE_DIG_0 + i, (val==31) ? DISABLE_SPRITE_Y : SCORE_BASE_Y);
		if (val < 10) {
			// if the digit is not blank, tell it what digit to display
			write_sprite_idx(SPRITE_SCORE_DIG_0 + i, PALETTE_NUM0 + val);
		}
	}
	// high sprites
	for(int i=4; i<NUM_DIGITS; i++) {
		int val = result[1] >> ((i-4)*8); // extract the value for this digit
		val &= 0x1f; // mask
		// if val == 31, digit is disabled! move it off screen
		// else, digit is normally enabled, move it back on screen & change displayed sprite
		write_sprite_ypos(SPRITE_SCORE_DIG_0 + i, (val==31) ? DISABLE_SPRITE_Y : SCORE_BASE_Y);
		if (val < 32) {
			// if the digit is not blank, tell it what digit to display
			write_sprite_idx(SPRITE_SCORE_DIG_0 + i, PALETTE_NUM0 + val);
		}
	}
}


// SDL: pause=UP, quit=DOWN
// MIPS: pause=UP, quit=DOWN
// return 1 if we quit, return 0 if not
int check_pausequit() {
	int t = (int)read_butts(); // read the buttons & cast
	int releases = wait_for_button_release(t); // what buttons were released since i read last?

	// if the down button is released...
	if (releases & BUTTON_DOWN_A) {
		// MIPS quit via return/return
		// return 1, signaling we should kill player 1
		return 1;
	} else if (releases & BUTTON_DOWN_B) {
		// return 1, signaling we should kill player 2
		return 2;
	}

	// if the center button is released...
	if (releases & (BUTTON_UP_A | BUTTON_UP_B)) {
		// pause all music/sfx
		write_audio_status(AUDCHAN1|AUDCHAN2|AUDCHAN3|AUDCHAN4,0,0);
		print("begin_pause at time=%li\n", gametime);
		// init the "waiting for input" animation
		write_leds(0xFF00);
		// reset timer1 so the blinking works properly
		write_timer1(0);
		// then pause the game by entering this busy-wait thing!
		int done = 0;
		while (done == 0) {
			// animate the LEDS with the same method as the main menu
			long now = read_timer1();
			if (now > TIMERCNT_6_HZ) {
				// wrap the timer
				print("blink");
				write_timer1(now - TIMERCNT_6_HZ);
				// toggle the LEDs
				int temp = read_leds();
				write_leds(~temp);
				draw();
			}

			t = (int)read_butts(); // read the buttons & cast
			// only the center button can unpause the game
			releases = wait_for_button_release(t);
			done = releases & (BUTTON_UP_A | BUTTON_UP_B);
		}
		// game has been unpaused!
		// resume all music/sfx
		write_audio_status(0,AUDCHAN1|AUDCHAN2|AUDCHAN3|AUDCHAN4,0);
		// to prevent the game from suddenly skipping forward over tons of gameticks, set MFP_TIMER0 to 0
		// so its like we never even paused!
		write_timer0(0);
		print("\nend_pause at time=%li\n",gametime);

	}
	return 0;
}




// done for 2player, MIPS only
void move_players(int numplayers) {
#if !defined(SDL2) && defined(USE_PADDLE)
	for(int z=0; z<numplayers; z++) {
		// for each player,
		if (player_status[z] == PLAYER_ALIVE) {
			int new_pos = 0;
			// analog input(s)!
			// read analog input, arrives as 12-bit?
			int raw = (z ? read_xadc_b() : read_xadc_a());
			// scale/translate it to be within the range of valid positions using "magic math"
			// "magic number" explained in (params.h) where it is #defined
			raw = (raw - XADC_MIN_READING) * 100;
			new_pos = raw / XADC_MAGIC_NUMBER;

			// ensure new_pos is clipped to within the bounds/margin
			new_pos = my_clip(new_pos, PLAYER_MIN_X, PLAYER_MAX_X);

			// write new ship position=new_pos as determined by the above blocks
			write_sprite_xpos(SPRITE_SHIP+z, new_pos);
		}
	}// end of for-each-player loop
#else
	// optional/fallback: button control!
	// NOTE: MIPS buttons don't support two-player(yet), SDL buttons do (via WASD)

	// first, read the buttons only once, use for both passes
	int butts = (int)read_butts();
	for(int z=0; z<numplayers; z++) {
		// for each player,
		if (player_status[z] == PLAYER_ALIVE) {
			int posmod = 0;
			// player1, player2 have different left/right buttons to mask&test
			int leftcode = (z ? BUTTON_LEFT_B : BUTTON_LEFT_A);
			int rightcode = (z ? BUTTON_RIGHT_B : BUTTON_RIGHT_A);
			if ((butts & leftcode) && !(butts & rightcode)) {
				// if indicating left, movespeed is negative
				posmod -= PLAYER_BUTTON_MOVESPEED;
			} else if (!(butts & leftcode) && (butts & rightcode)) {
				// if indicating right, movespeed is positive
				posmod += PLAYER_BUTTON_MOVESPEED;
			}

			// read current position and add the delta
			int new_pos = read_sprite_xpos(SPRITE_SHIP+z) + posmod;

			// ensure new_pos is clipped to within the bounds/margin
			new_pos = my_clip(new_pos, PLAYER_MIN_X, PLAYER_MAX_X);

			// write new ship position=new_pos
			write_sprite_xpos(SPRITE_SHIP+z, new_pos);
		}
	}// end of for-each-player loop
#endif
}


// return 1 if the enemies reached the ground and we lost
// enemies move down 1 row size each time
// if arg=0, run normal; if arg=1, reset static members
int move_enemies(int reset) {
	static enum enemy_movement_enum movement_state = MOVING_RIGHT;
	static int times_moved = 0;
	static int dist_moved = 0;
	if (reset != 0) {
		movement_state = MOVING_RIGHT;
		times_moved = 0;
		dist_moved = 0;
		return 0;
	}
	// read location of enemy00
	// (have it)
	int deltaX=0, deltaY=0;
	// determine where I am in the movement pattern and what deltaX/deltaY I should move by

	// first check if its time to change movement direction: either change or hold
	// use newly determined direction to decide deltaX, deltaY
	// TODO minor: "overflow" dist moved left/right/down into the other direction when changing directions
	int left_enemy_index = ENEMY_COLS;
	int right_enemy_index = 0;
	int bottom_enemy_index = 0;
	for (int y = 0; y < ENEMY_ROWS; y++) {
		for (int x = 0; x < ENEMY_COLS; x++) {
			if (enemy_record[y][x] == ENEMY_ALIVE) {
				bottom_enemy_index = y;
				if (x < left_enemy_index) {
					left_enemy_index = x;
				}
				if (x > right_enemy_index) {
					right_enemy_index = x;
				}
			}
		}
	}
	int left_enemy_x = enemy00_x + (left_enemy_index * ENEMY_SPACING_X);
	int right_enemy_x = enemy00_x + (right_enemy_index * ENEMY_SPACING_X) + ENEMY_SIZE_X;
	switch(movement_state) {
	case MOVING_RIGHT:
		//if rightmost enemy pos is too far to the right, change to down
		// rightmost LIVING enemy pos
		if(right_enemy_x >= ENEMY_MAX_X) {
			// begin moving down now
			print("enemy begin moving down from right\n");
			movement_state = MOVING_DOWN;
			dist_moved = ENEMY_MOVESPEED_Y;
			deltaY = ENEMY_MOVESPEED_Y;
		} else {
			// otherwise, still moving right
			deltaX = ENEMY_MOVESPEED_X;
		}
		break;
	case MOVING_LEFT:
		// if leftmost enemy pos is too far to the left, change to down
		// leftmost LIVING enemy pos
		if(left_enemy_x <= ENEMY_MIN_X) {
			// begin moving down now
			print("enemy begin moving down from left\n");
			movement_state = MOVING_DOWN;
			dist_moved = ENEMY_MOVESPEED_Y;
			deltaY = ENEMY_MOVESPEED_Y;
		} else {
			// otherwise, still moving left
			deltaX = -(ENEMY_MOVESPEED_X);
		}
		break;
	case MOVING_DOWN:
		// if dist_moved > threshold,
		if(dist_moved >= ENEMY_MOVE_DOWN_LIMIT) {
			if(left_enemy_x <= ENEMY_MIN_X) {
				// if on the left, change to right
				print("enemy begin moving right from down\n");
				movement_state = MOVING_RIGHT;
				deltaX = ENEMY_MOVESPEED_X;
			} else {
				// otherwise, we must be on the right, so change to left
				print("enemy begin moving left from down\n");
				movement_state = MOVING_LEFT;
				deltaX = -(ENEMY_MOVESPEED_X);
			}
		} else {
			// otherwise, still moving down
			dist_moved += ENEMY_MOVESPEED_Y;
			deltaY = ENEMY_MOVESPEED_Y;
		}
		break;
	}


	// delete rows of bunkers once they begin to overlap with the enemies
	// if touching where a 3rd row of bunker tiles would be, lose the game immediately
	if(movement_state == MOVING_DOWN) {
		// if moving down, speculatively add deltaY, see if it clips the bunkers
		// if so, delete some of the bunkers
		for(int row=0; row<BUNKER_ROWS+1; row++) {
			// the y-value of this row of the bunker
			int bunkerpos = BUNKER_BASE_Y + (row * BUNKER_SPACING_Y);
			// if (the bottom of where the lowest enemy will move to) is below the top of the bunker row...
			// bottommost LIVING enemy pos
			if(enemy00_y + deltaY + (bottom_enemy_index * ENEMY_SPACING_Y) + ENEMY_SIZE_Y >= bunkerpos) {
				if(row < BUNKER_ROWS) {
					// ... if its a real row...
					// ... then delete the row of bunker tiles
					for(int n=0; n<NUM_BUNKERS; n++) {
						for(int c=0; c<BUNKER_COLS; c++) {
							bunker_record[n][row][c] = BUNKER_DEAD;
							write_sprite_ypos(SPRITE_BUNKER_X(n,row,c), DISABLE_SPRITE_Y);
						}
					}
				} else {
					// ... if its the virtual row...
					// ... lose the game immediately
					return 1;
				}
			}
		}
	}

	// add the delta to the x and y point for the top-left corner
	enemy00_x += deltaX;
	enemy00_y += deltaY;

	// inc with wrap, wiggle every Xth movement operation
	times_moved += 1;
	if (times_moved >= ENEMY_WIGGLE_FREQ) {
		times_moved = 0;
	}

	// for each enemy... 2D iterate
	for(int j=0; j<ENEMY_ROWS; j++) {
		for(int k=0; k<ENEMY_COLS; k++) {
			// skip all enemies that are dead/exploding
			if(enemy_record[j][k] != ENEMY_ALIVE) { continue;}

			// read location of enemy j,k from AHB
			int bay = SPRITE_ENEMY_X(j,k);
			int ex = read_sprite_xpos(bay);
			int ey = read_sprite_ypos(bay);
			// add deltaX,deltaY to its x and y
			ex += deltaX; ey += deltaY;
			// write back new position
			write_sprite_xpos(bay, ex);
			write_sprite_ypos(bay, ey);

			// is it time to "wiggle" the enemies?
			if(times_moved == 0) {
				// read the currently-displayed sprite for enemy j,k from AHB
				enum palette_index paletteidx = read_sprite_idx(bay);
				// "wiggle" the sprite by toggling from one variant to the other
				// (probably highly dependant on how we store sprites & what sprites are available)
				switch(paletteidx) {
				case PALETTE_ENEMY_A1: paletteidx = PALETTE_ENEMY_A2; break;
				case PALETTE_ENEMY_A2: paletteidx = PALETTE_ENEMY_A1; break;
				case PALETTE_ENEMY_B1: paletteidx = PALETTE_ENEMY_B2; break;
				case PALETTE_ENEMY_B2: paletteidx = PALETTE_ENEMY_B1; break;
				default: break;
				}
				// write the newly-chosen enemy sprite to enemy j,k
				write_sprite_idx(bay, paletteidx);
			}
		}
	}


	return 0;
}


// return the # of bullets that were moved
int move_bullets() {
	int c = 0;
	for(int i=0; i<MAX_BULLETS; i++) {
		// skip any disabled bullets
		if(bullet_record[i] == BULLET_OFF) {continue;}

		c += 1;
		// get the bullet position from AHB (dont need x, just y)
		int bay = SPRITE_BULLET_0 + i;
		int by = read_sprite_ypos(bay);

		switch(bullet_record[i]) {
		case BULLET_PLAYER:
			// its a player bullet, move it UP
			// (up means decrement)
			by -= BULLET_MOVESPEED;
			// if its position is below some threshold (off the top of the screen) then it dies
			if(by <= MIN_FIELD_Y) {
				// offscreen, dies
				// move the sprite off screen
				write_sprite_ypos(bay, DISABLE_SPRITE_Y);
				// indicate that it is off
				bullet_record[i] = BULLET_OFF;
			} else {
				// everything is normal
				// write to AHB
				write_sprite_ypos(bay, by);
			}
			break;
		case BULLET_ENEMY:
			// its an enemy bullet, move it DOWN
			// (down means increment)
			by += BULLET_MOVESPEED;
			// if its position is above some threshold (below the bottom of the screen) then it dies
			if(by > MAX_FIELD_Y) {
				// offscreen, dies
				// move the sprite off screen
				write_sprite_ypos(bay, DISABLE_SPRITE_Y);
				// indicate that it is off
				bullet_record[i] = BULLET_OFF;
			} else {
				// everything is normal
				// write to AHB
				write_sprite_ypos(bay, by);
			}
			break;
		case BULLET_OFF:
			// not possible
			break;
		}
	}
	return c;
}


void shoot_enemy() {
	print("ENEMIES SHOOTING\n");

	int a = 0; // DEBUG: holds the LED bytes

	int col_prev = ENEMY_COLS+1;
	// how many enemies will fire?
	int r = get_random();
	int numfire = r & 0x0001; // =0/1
	numfire += 1; // numfire =1/2 = how many enemies will fire

	for(int z=0; z<numfire; z++) {
		int col;
		// note: made this a for loop so if something wonky happens it won't get stuck in infinite while-loop, not actually using "i"
		for(int i=0; i<ESHOOT_MAX_ATTEMPTS; i++) {
			// pick a random column
			r = get_random();
			col = r % ENEMY_COLS; // r%x
			// not allowed to fire on the same column twice
			if(col==col_prev) { continue; }
			// check that the chosen column is valid by checking rows front to back (0=top=back, probably)
			int row;
			for(row=ENEMY_ROWS-1; row>=0; row--) {
				// is enemy (col, row) alive? if so, break and keep value of row
				if(enemy_record[row][col] == ENEMY_ALIVE) { break; }
			}
			// if all enemies in the col are dead, row will equal -1 when the loop ends
			if(row!=-1) {
				// if able to find the front, fire and exit the for-loop
				// read AHB, get the x and y position of the enemy at formation position (row, col)
				int bay = SPRITE_ENEMY_X(row,col);
				int ex = read_sprite_xpos(bay);
				int ey = read_sprite_ypos(bay);
				// that coord is the top-left of the enemy; translate to the bottom-center of the enemy
				ex += (ENEMY_SIZE_X / 2) - (BULLET_SIZE_X / 2);
				ey += ENEMY_SIZE_Y;

				// generate the bullet
				create_bullet(BULLET_ENEMY, ex, ey);

				a |= 1<<col; // DEBUG
				// break
				break;
			}
		}
		// save the column that just fired so it can ensure it doesnt fire twice on the same col
		col_prev = col;
	}

	// TODO sound: enemy shoot sound effect?

	write_leds(a); // DEBUG: "firing" for test purposes
}


// done for 2player
void shoot_players(int numplayers) {
	// check if the player is trying to shoot!
	int butts = (int)read_butts(); // read butts & cast
	for (int z = 0; z < numplayers; z++) {
		if (butts & (z?BUTTON_SHOOT_B:BUTTON_SHOOT_A)) {
			// check if the player is able to shoot
			// this handles death-lockout, no need to verify that they are alive
			if (player_shoot_cooldown[z] == 0) {
				// button is pressed! fire a shot!
				print("player1 shoot\n");
				// read AHB, get the x and y position of the player
				int px = read_sprite_xpos(SPRITE_SHIP+z);
				int py = read_sprite_ypos(SPRITE_SHIP+z);
				// that coord is the top-left of the player; translate to the top-center of the player
				px += (SHIP_SIZE_X / 2) - (BULLET_SIZE_X / 2);
				py += (BULLET_SIZE_Y);
				// generate the bullet
				create_bullet(BULLET_PLAYER, px, py);

				// sound: player fire
				write_audio_soundfx(AUDCHAN4, AUDIO_PLAYER_SHOOT);

				// engage the shooting cooldown
				player_shoot_cooldown[z] = 1;
				player_shoot_cooldown_target[z] = new_target(DELAY_PLAYER_SHOOT_COOLDOWN);
			}
		}
	}
}



// create a bullet, type=1=enemy, type=2=player
// return 0 if it failed to create a bullet, return 1 if normal (not used tho)
int create_bullet(int type, int locx, int locy) {
	// search thru the record for a blank space
	for (int i=0; i<MAX_BULLETS; i++) {
		if (bullet_record[i] == BULLET_OFF) {
			// when a space is found, write it into bullet record
			bullet_record[i] = type;
			// create it by writing to ABH in bullet-slot "i" with locx,locy
			write_sprite_xpos(SPRITE_BULLET_0 + i, locx);
			write_sprite_ypos(SPRITE_BULLET_0 + i, locy);
			return 1;
		}
	}

	// if no blank spaces are found, just don't create one, should be an obvious error
	write_leds(0xFFFF);
	print("ERROR: ALL BULLET SLOTS ARE FULL WHEN TRYING TO CREATE BULLET\n");
	return 0;
}

// done for 2player
void hit_players(int whichplayer) {
	print("player%i hit\n", whichplayer+1);
	// THE PLAYER HAS BEEN HIT! DO STUFF!
	// set "dead" flag: disable collision with player ship until we respawn (cant die while im dead)
	player_status[whichplayer] = PLAYER_EXPLODING_A;
	// replace ship with explodeA sprite
	write_sprite_idx(SPRITE_SHIP + whichplayer, PALETTE_EXPLODE_A);
	// set player_animate_target
	player_animate_target[whichplayer] = new_target(PLAYER_EXPLODE_SPEED);
	// disable shooting (cant shoot while im dead)(need to go thru 3 frames of animation before you can shoot again)
	player_shoot_cooldown[whichplayer] = 1;
	player_shoot_cooldown_target[whichplayer] = new_target(PLAYER_EXPLODE_SPEED * 3);
	// sound: player explode
	write_audio_soundfx(AUDCHAN4, AUDIO_PLAYER_EXPLODE);
}


void hit_enemy(int xidx, int yidx) {
	print("enemy hit\n");
	// change the record to indicate enemy is exploding
	enemy_record[yidx][xidx] = ENEMY_EXPLODING_A;
	// set explosion animation target
	enemy_animate_target[yidx][xidx] = new_target(ENEMY_EXPLODE_SPEED);
	// enemy becomes explosion, change its sprite addr on AHB
	int bay = SPRITE_ENEMY_X(yidx,xidx);
	write_sprite_idx(bay, PALETTE_EXPLODE_A);
	// add to score
	current_game_score += SCORE_PER_ENEMY;
	// redisplay score
	calc_and_draw_score(current_game_score);

	// sound: enemy explode
	write_audio_soundfx(AUDCHAN3, AUDIO_ENEMY_EXPLODE);
}



void hit_bunker(int bidx, int xidx, int yidx, enum bullet_record_enum whobrokeit) {
	// bunker advances to next state
	print("bunker hit\n");
	int bay = 0;
	switch(bunker_record[bidx][yidx][xidx]) {
	case BUNKER_FULL_A: // advance
	case BUNKER_FULL_B: // advance
	case BUNKER_LOUT_A: // advance
	case BUNKER_ROUT_A: // advance
		// move the record to the next stage (B)
		bunker_record[bidx][yidx][xidx] += 1;
		// read the sprite index for [bidx][yidx][xidx] from AHB
		bay = SPRITE_BUNKER_X(bidx,yidx,xidx);
		int spriteidx = read_sprite_idx(bay);
		// add 1 to the index to be pointing at the next sprite
		spriteidx += 1;
		// write the sprite address to AHB
		write_sprite_idx(bay, spriteidx);
		break;
	case BUNKER_FULL_C:
	case BUNKER_LOUT_B:
	case BUNKER_ROUT_B:
		if(whobrokeit == BULLET_PLAYER) {
			// some small score? but only if the player broke it!
			current_game_score += SCORE_PER_BUNKER;
			calc_and_draw_score(current_game_score);
		}
		// move the record to the next stage (dead)
		bunker_record[bidx][yidx][xidx] = BUNKER_DEAD;
		// disable the sprite
		bay = SPRITE_BUNKER_X(bidx,yidx,xidx);
		write_sprite_ypos(bay, DISABLE_SPRITE_Y);
		break;
	default:
		break;
	}
	// no animation targets to set
}



// *run ebeam collision (ebeams vs player)
// return 1 if hit, return 0 if not
// for better encapsulation, "player-death events" are in their own function?
// done for 2player
int collision_ebullet(int numplayers) {
	for(int z=0; z<numplayers; z++) {
		if (player_status[z] != PLAYER_ALIVE) { continue; }
		// get player ship position from AHB
		int px = read_sprite_xpos(SPRITE_SHIP + z);
		int py = read_sprite_ypos(SPRITE_SHIP + z);

		// run through all bullets, find enemy bullets,
		for(int i=0; i<MAX_BULLETS; i++) {
			// skip anything that isn't an enemy bullet
			if(bullet_record[i] != BULLET_ENEMY) {continue;}

			int bay = SPRITE_BULLET_0 + i;
			// read the position of bullet "i" from AHB
			int bx = read_sprite_xpos(bay);
			int by = read_sprite_ypos(bay);
			// already know its size of course

			// use "collide()" to compare its position with the player ship
			if(collide(px, py, SHIP_SIZE_X, SHIP_SIZE_Y, bx, by, BULLET_SIZE_X, BULLET_SIZE_Y)) {
				// THE PLAYER HAS BEEN HIT! DO STUFF!
				hit_players(z);
				// disable/kill bullet "i"
				bullet_record[i] = BULLET_OFF;
				// move bullet "i" off screen
				// create it by writing to ABH in bullet-slot "i" with locx,locy
				write_sprite_ypos(bay, DISABLE_SPRITE_Y);
				return 1;
			}
		}
	}
	return 0;
}

// *run pbeam collision (pbeams vs enemies)
// return # of enemies that died when this ran
int collision_pbullet() {
	int retme = 0;
	// read location of enemy00 from AHB
	// (have it)

	// run through all bullets & find player bullets,
	for(int i=0; i<MAX_BULLETS; i++) {
		// skip anything that isn't a player bullet
		if(bullet_record[i] != BULLET_PLAYER) {continue;}
		// read the position of bullet "i" from AHB
		int bx = read_sprite_xpos(SPRITE_BULLET_0 + i);
		int by = read_sprite_ypos(SPRITE_BULLET_0 + i);
		// already know its size of course

		// CURRENT PLAN: use knowledge of enemy size/spacing to determine which enemy we might hit; see if we acutally are
		// ALTERNATE PLAN: check collision of bullet against each living enemy

		// Q1: is the bullet ANYWHERE within the enemy array at all? (including slightly to the left and above)
		if(collide(enemy00_x-BULLET_SIZE_X, enemy00_y-BULLET_SIZE_Y, (ENEMY_COLS*ENEMY_SPACING_X)+BULLET_SIZE_X, (ENEMY_ROWS*ENEMY_SPACING_Y)+BULLET_SIZE_Y,
				   bx, by, BULLET_SIZE_X, BULLET_SIZE_Y)){
			// Q2: which enemy array sub-square is it within?
			int xidx = (bx - enemy00_x) / ENEMY_SPACING_X;
			// NO MOD HERE: NOW THIS CAN RETURN -1
			int yidx = (by - enemy00_y) / ENEMY_SPACING_Y;
			// NO MOD HERE: NOW THIS CAN RETURN -1
			if (yidx >= ENEMY_ROWS) { continue; }
			//print("xy: %i %i\n", xidx, yidx);

			for (int g = 1; g != -1; g--) {
				// idea: check only that subsquare and the one BELOW
				int yidx2 = my_clip((yidx + g), 0, ENEMY_ROWS-1); // constrain to fall within valid range
				for (int h = 1; h != -1; h--) {
					// idea: check only that subsquare and the one to the RIGHT
					int xidx2 = my_clip((xidx + h), 0, ENEMY_COLS-1); // constrain to fall within valid range
					// Q3: is the enemy in that sub-square alive?
					if (enemy_record[yidx2][xidx2] == ENEMY_ALIVE) {
						// Q4: is it actually touching the enemy in the subsquare its in
						int bay = SPRITE_ENEMY_X(yidx2, xidx2);
						int ex = read_sprite_xpos(bay);
						int ey = read_sprite_ypos(bay);

						// compare bullet position against enemy position
						//print("%i %i %i %i %i %i %i %i\n", ex, ey, ENEMY_SIZE_X, ENEMY_SIZE_Y, bx, by, BULLET_SIZE_X, BULLET_SIZE_Y);
						if (collide(ex, ey, ENEMY_SIZE_X, ENEMY_SIZE_Y, bx, by, BULLET_SIZE_X, BULLET_SIZE_Y)) {
							// AN ENEMY HAS BEEN HIT! DO STUFF!
							hit_enemy(xidx2, yidx2);
							// disable/kill bullet "i"
							bullet_record[i] = BULLET_OFF;
							// move bullet "i" off screen
							write_sprite_ypos(SPRITE_BULLET_0 + i, DISABLE_SPRITE_Y);
							// increment value to be returned
							retme += 1;
							break;
						}
					}
				}
			}
		}
		// TODO stretch: is the pbullet colliding with the "special enemy"?
		// is the special alive?
		// check collision

	}// end of for-loop
	return retme;
}

// *run bunker collision (pbeams vs bunker, ebeams vs bunker)
// what does it return? the # of bunkers hit this pass, for no particular reason
int collision_bunker() {
	int retme = 0;
	// for each bullet, check if it hit a bunker block
	for(int i=0; i<MAX_BULLETS; i++) {
		// skip any dead bullets
		if(bullet_record[i] == BULLET_OFF) {continue;}
		// read the position of bullet "i" from AHB
		int bx = read_sprite_xpos(SPRITE_BULLET_0 + i);
		int by = read_sprite_ypos(SPRITE_BULLET_0 + i);
		// already know its size of course

		// CURRENT PLAN: use knowledge of bunker size/spacing to determine which bunker we might hit; see if we acutally are
		// ALTERNATE PLAN: check collision of bullet against each living enemy

		// TODO MINOR BUG: bullets that horizontally overlap the left edge of a bunker tile won't hit that tile

		// Q1: is the bullet ANYWHERE within the bunker array at all?
		if(collide(BUNKER_BASE_X, BUNKER_BASE_Y-BULLET_SIZE_Y,NUM_BUNKERS*BUNKER_TO_BUNKER, (BUNKER_ROWS*BUNKER_SPACING_Y)+BULLET_SIZE_Y,
				   bx, by, BULLET_SIZE_X, BULLET_SIZE_Y)){
			// Q2: which bunker array sub-square is it within?
			int bidx = (bx - BUNKER_BASE_X) / BUNKER_TO_BUNKER;
			bidx = bidx % NUM_BUNKERS; // this shoudl never happen but its to prevent fencepost errors
			int xidx = (bx - BUNKER_BASE_X - (bidx*BUNKER_TO_BUNKER)) / BUNKER_SPACING_X;
			xidx = xidx % BUNKER_COLS; // this shoudl never happen but its to prevent fencepost errors
			int yidx = (by - BUNKER_BASE_Y) / ENEMY_SPACING_Y;
			// NO MOD HERE: NOW THIS CAN RETURN -1

			//if a player bullet, check bottom(1) then top(0) (decrement)
			//if an enemy bullet, check top(0) then bottom(1) (increment)
			for (int z = (bullet_record[i]==BULLET_PLAYER ? 1 : 0); z<2 && z>-1; z += (bullet_record[i] == BULLET_PLAYER ? -1 : 1)) {
				int yidx2 = my_clip((yidx + z), 0, BUNKER_ROWS-1); // constrain to fall within valid range
				// Q3: is the bunker in that sub-square alive?
				if (bunker_record[bidx][yidx2][xidx] != BUNKER_DEAD) {
					// Q4: is it actually touching the bunker in the subsquare its in
					// (probably, but just be sure)
					int ex = BUNKER_BASE_X + (BUNKER_TO_BUNKER * bidx) + (BUNKER_SPACING_X * xidx);
					int ey = BUNKER_BASE_Y + (BUNKER_SPACING_Y * yidx2);

					// compare bullet position against bunker position
					if (collide(ex, ey, BUNKER_SIZE_X, BUNKER_SIZE_Y, bx, by, BULLET_SIZE_X, BULLET_SIZE_Y)) {
						// A BUNKER HAS BEEN HIT! DO STUFF!
						hit_bunker(bidx, xidx, yidx2, bullet_record[i]);
						// disable/kill bullet "i"
						bullet_record[i] = BULLET_OFF;
						// move bullet "i" off screen
						write_sprite_ypos(SPRITE_BULLET_0 + i, DISABLE_SPRITE_Y);
						// increment value to be returned
						retme += 1;
						continue;
					}
				}
			}
		}
	}// end for-each-bullet loop
	return retme;
}

//////////////////////////////////////////////////
// scheduler functions

// this will be used LOTS so it should be a function
// add duration with wrap, "gametime" is global so it doesn't need to be passed as an arg
int new_target(int duration) {
	int target = gametime + duration;
	if(target >= GAMETIME_MAX) {
		// equivalent to unconditional target % GAMETIME_MAX
		return (target - GAMETIME_MAX); // wrap
	} else {
		return target;
	}
}

// this will be used LOTS so it should be a function
// returns 0 or 1... apparently "bool" isnt a supported type, gotta use int
// NOTE: now gametime is global, not needed as a param!
int time_to_run_target(int target) {
	// idea: the difference between gametime and target must be between 0 and 1000 to be counted as a match!
	int diff = gametime - target;
	if(diff<0) { diff += GAMETIME_MAX; } // wrap the difference if it is negative
	if((diff >= 0) && (diff < GAMETIME_WINDOW)) {
		return 1;
	}
	return 0;
}

// read the clock & increment gametime accordingly
// return the # of ticks advanced this call
// NOTE: now gametime is global, not needed as a param!
int update_gametime() {
	// we're using a larger tick increment, "gametime", to time most events
	long now = read_timer0();
	if (now >= GAMESTEP) {

		// removed the "jump multiple gameticks at once" plan because it can cause rare issues
		//int inc = 1;
		//int new_now = now - GAMESTEP;
		// idea: allow ability to jump multiple gameticks at once
		// reinstated the idea because a new way of comparing targets eliminates the issue
		int inc = now / GAMESTEP;
		int new_now = now % GAMESTEP;

		write_timer0(new_now); // write
		gametime += inc;
		if (gametime >= GAMETIME_MAX) {
			gametime -= GAMETIME_MAX; // wrap
		}
		return inc;
	}
	return 0;
}
