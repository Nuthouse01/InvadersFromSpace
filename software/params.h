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
#ifndef SPACEINVADER_PARAMS
#define SPACEINVADER_PARAMS

// THIS FILE holds a variety of #defines that are specific to our game and need used in multiple places
// some of these we may want to be flexible and re-defineable at runtime, but this is a necessary stopgap measure

/////////////////////////////////////////////////////////////
// misc constants!?

#ifdef SDL2
#define TIMERCNT_4_SEC			4000
#define TIMERCNT_1_SEC			1000
#define TIMERCNT_1_HZ			1000
#define TIMERCNT_6_HZ			166
#define TIMERCNT_12_HZ			83
#define TIMERCNT_60_HZ          17
#define TIMERCNT_200_HZ         5
#else
// added div by 2 cuz its 25Mhz clock instead of 50Mhz
#define TIMERCNT_4_SEC			33333333	// when timer reaches this value, 4 seconds have passed
#define TIMERCNT_1_SEC			 8333333
#define TIMERCNT_1_HZ			 8333333
#define TIMERCNT_6_HZ			 1388888
#define TIMERCNT_12_HZ			  694444
#define TIMERCNT_60_HZ			  138888
#define TIMERCNT_200_HZ			   41666
#endif


/////////////////////////////////////////////////////////////
// timing-related game constants

//NOTE: should these be const or defines? does it matter?
// 83333 -> 0.005s = 200Hz
// 277777 ->0.0166s = 60Hz (note: if we go to 60Hz we may as well make it screen-synchrous!!)
#define GAMESTEP			TIMERCNT_200_HZ
#define GAMETIME_MAX		6000
#define GAMETIME_WINDOW		1000
// NOTE: maximum allowable [duration] = [GAMETIME_MAX] - [GAMETIME_WINDOW] - 1
#define TARGET_OFF			0xFFFF

// how long to pause to wait for these sounds to play
#define WAVESTART_SOUND_LENGTH		TIMERCNT_1_SEC
#define GAMEOVER_SOUND_LENGTH		TIMERCNT_1_SEC

// NOTE: soundfx lengths! (not needed for code)
// player explode = 0.6s
// player shoot = 0.2s
// enemy explode = 0.3s

// how many ticks between doing each of these things
#define DELAY_MOVEPLAYER				10
#define DELAY_MOVEENEMY_BASE			100
#define DELAY_MOVEENEMY_MIN				15
#define DELAY_MOVEBULLET				5
#define DELAY_PLAYER_SHOOT_COOLDOWN		80		// 0.4s


// how long to linger on each of the enemy explosion frames
#define ENEMY_EXPLODE_SPEED				40		// 0.2s
// how long to linger on each of the player explosion frames, also how long to stay dead (gone) before respawn
#define PLAYER_EXPLODE_SPEED			100		// 0.5s


// enemies will wiggle (mirror their sprites) every Xth time they are moved (NOT IN GAMETICKS)
#define ENEMY_WIGGLE_FREQ	2

// random shooting parameters, in gameticks
#define ESHOOT_FLOOR_BASE	80			// 0.4s
#define ESHOOT_CEIL_BASE	1000		// 5.0s
#define ESHOOT_MAX_ATTEMPTS	50
#define ESHOOT_CEILDROP_PER_WAVE 140	// 0.7s



// 1=twoplayer, 0=oneplayer
#define SDL_MULTIPLAYER		1
// on=paddle, off=buttons
#define USE_PADDLE
// on=blankpad, off=zeropad
#define SCORE_LEFT_BLANKPAD
// on=hollow bunker, off=filled bunker
//#define HOLLOW_BUNKERS
// on=skip delays, off=play as intended
//#define FAST

#define SCORE_PER_ENEMY			100
#define	SCORE_PER_BUNKER		25
#define SCORE_PER_SPECIAL		800

/////////////////////////////////////////////////////////////
// sprite sizes

#define WINDOW_SIZE_X		640
#define WINDOW_SIZE_Y		480

// sizes of sprites (size they will apprear to be after scaling, minimum bounding box!
// NOTE: these sizes are the size on screen, AFTER scaling!!
// commented value is base size
// INCORRECT: because hardware scaling isn't supported, these instead are just the bounding box
// for each type of sprite, for the purposes of collision
#define BULLET_SIZE_X		3	//3
#define BULLET_SIZE_Y		12	//12

#define ENEMY_SIZE_X		16	//16
#define ENEMY_SIZE_Y		8	//8

#define ENEMY_SPEC_SIZE_X	16	//16
#define ENEMY_SPEC_SIZE_Y	15	//15

#define SHIP_SIZE_X			16	//16
#define SHIP_SIZE_Y			15	//15

#define BUNKER_SIZE_X		16
#define BUNKER_SIZE_Y		16

#define DIGIT_SIZE_X		16	//16
#define DIGIT_SIZE_Y		16	//16

#define LABEL_SIZE_X		16	//16
#define LABEL_SIZE_Y		14	//14


/////////////////////////////////////////////////////////////
// define "playing field" area
// at what y do bullets dissipate(top/bottom)
// at what x is the player movement bounded

// move a sprite offscreen to disable it
#define DISABLE_SPRITE_Y	800


// distance in from either side of the screen the player is bounded
#define PLAYER_MARGIN 		20
#define PLAYER_MIN_X		PLAYER_MARGIN
#define PLAYER_MAX_X		(WINDOW_SIZE_X - PLAYER_MARGIN - SHIP_SIZE_X)
// initial position of the ship
// the y-position is very important cuz that never changes, ship only moves along x axis
#define SHIP_BASE_Y		440
#define SHIP_BASE_X		(WINDOW_SIZE_X / 2)
// player moves this many pixels per movement operation when using buttons
#define PLAYER_BUTTON_MOVESPEED	15





// max # of bullets that can be on screen at a time (number of sprite slots allocated for bullets)
#define MAX_BULLETS			10
// bullets die when crossing these thresholds
#define MIN_FIELD_Y 		50
#define MAX_FIELD_Y 		(WINDOW_SIZE_Y - BULLET_SIZE_Y)
// how many pixels do bullets move each time they are moved?
#define BULLET_MOVESPEED	11



// enemy formation parameters
#define ENEMY_ROWS			4
#define ENEMY_COLS			10
// dist from top-left corner to top-left corner of aliens
#define ENEMY_SPACING_X		40
#define ENEMY_SPACING_Y		40
// xy of alien 00
#define ENEMY_BASE_X		50
#define ENEMY_BASE_Y		50
// how many pixels do the enemies move left/right/down?
#define ENEMY_MOVESPEED_X	10
#define ENEMY_MOVESPEED_Y	10
// how far left/right do the enemies move?
#define ENEMY_MARGIN		20
#define ENEMY_MIN_X			ENEMY_MARGIN
#define ENEMY_MAX_X			(WINDOW_SIZE_X - ENEMY_MARGIN - ENEMY_SIZE_X)
#define ENEMY_MOVE_DOWN_LIMIT	ENEMY_SPACING_Y



// how many and how spaced out the bunkers are
#define NUM_BUNKERS				4
#define BUNKER_ROWS				2
#define BUNKER_COLS				3
// the dist from top-left corner to top-left corner (must be greater than BUNKER_COLS*BUNKER_SIZE_X)
#define BUNKER_TO_BUNKER		(WINDOW_SIZE_X / NUM_BUNKERS)
// the top-left corner of the leftmost bunker
#define BUNKER_BASE_X			((BUNKER_TO_BUNKER - BUNKER_COLS * BUNKER_SIZE_X) / 2)
#define BUNKER_BASE_Y			375
#define BUNKER_SPACING_X 		BUNKER_SIZE_X
#define BUNKER_SPACING_Y		BUNKER_SIZE_Y
// NOTE: bunker outer corners, break in 2 hits (=4)
// bunker square, break in 3 hits (=3)
// =7 bunker tiles
// 4 bunkers, each 2x3



// position of the label for score
#define SCORE_LABEL_BASE_X	20
#define SCORE_LABEL_BASE_Y	20
#define SCORE_LABEL_SPACING LABEL_SIZE_X

// position of the 8th (MSB) number
#define SCORE_BASE_X		60
#define SCORE_BASE_Y		20
// dist from top-left to top-left of each number
#define SCORE_SPACING		18
#define NUM_DIGITS			8

// position of the label for wave#
#define WAVE_LABEL_BASE_X	300
#define WAVE_LABEL_BASE_Y	20
#define WAVE_LABEL_SPACING LABEL_SIZE_X

// position of the wave#
#define WAVE_BASE_X			350
#define WAVE_BASE_Y			20

// extra lives
#define LIVES_BASE_X		450
#define LIVES_BASE_Y		20
// dist from top-left to top-left of each icon
#define LIVES_SPACING		50



////////////////////////
// XADC conversion
//#define XADC_MAX_READING		2867
#define XADC_MAX_READING		3141	// right
//#define XADC_MIN_READING		1228
#define XADC_MIN_READING		1774	// left

#define XADC_MAGIC_NUMBER		(int)(((XADC_MAX_READING - XADC_MIN_READING) * 100) / WINDOW_SIZE_X)
/*
the following was originally calculated with min-reading=1228 and max-reading=2867
it was later parameterized so the values could be re-measured and changed without redoing the math

goal: xadc space -> 0-640
if xadc_min_raw=1228 and xadc_max_raw=2867, then obvious first step is to shift down by 1228 so both ranges start at 0:
xadc_min=0, xadc_max=1459, easy
not doing any fancy "perfect math", just assuming the voltage/xadc is linear with the potentiometer angle (its not but its close)
how to scale 0-1459 -> 0-640?
1459(xadc_max) / 640(window_max) = 2.28, AKA:
1459(maxin) / 2.28 = 640(maxout) and
0(minin) / 2.28 = 0(minout), therefore
(rawinput - xadc_min_raw) / 2.28 = (desired output)
so dividing by 2.28 is the proper scale
PROBLEM: cannot use floating point numbers...
SOLUTION: multiply top & bottom by 100
((rawinput - xadc_min_raw) * 100) / (2.28*100) = (desired output)
((rawinput - xadc_min_raw) * 100) / ((xadc_max/window_max)*100) = (desired output)
((rawinput - xadc_min_raw) * 100) / (((xadc_max_raw-xadc_min_raw)/window_max)*100) = (desired output)
however, not even the preprocessor can handle intermediate floating-point numbers, so we rearrange the denominator:
((rawinput - xadc_min_raw) * 100) / (((xadc_max_raw-xadc_min_raw)*100)/window_max) = (desired output)
becomes
((rawinput - xadc_min_raw) * 100) / MAGIC_NUMBER = (desired output)
where MAGIC_NUMBER= <<< (((xadc_max_raw-xadc_min_raw)*100)/window_max) >>>
*/




/////////////////////////////////////////////////////////////
// interacting with the AHB sprite module

// define the sprite palette indices for each sprite
enum palette_index { \
	PALETTE_NUM0=0, PALETTE_NUM1, PALETTE_NUM2, PALETTE_NUM3, PALETTE_NUM4, \
	PALETTE_NUM5, PALETTE_NUM6, PALETTE_NUM7, PALETTE_NUM8, PALETTE_NUM9, \
	PALETTE_EXPLODE_A=10, PALETTE_EXPLODE_B, PALETTE_BULLET, PALETTE_SHIP, \
	PALETTE_ENEMY_A1=14, PALETTE_ENEMY_B1, PALETTE_ENEMY_A2, PALETTE_ENEMY_B2, PALETTE_ENEMY_SPECIAL, \
	PALETTE_BUNKER_FULL_A=19, PALETTE_BUNKER_FULL_B, PALETTE_BUNKER_FULL_C, \
	PALETTE_BUNKER_LOUT_A, PALETTE_BUNKER_LOUT_B, PALETTE_BUNKER_ROUT_A, PALETTE_BUNKER_ROUT_B, \
	PALETTE_LABEL_SCORE_A=26, PALETTE_LABEL_SCORE_B, PALETTE_LABEL_WAVE_A, PALETTE_LABEL_WAVE_B, \
  PALETTE_SHIP_B=30};
// 0-9 are digits
// 10-13 are explosion, bullet, & ship
// 14-18 are enemies (including special enemy)
// 19-25 are bunker blocks
// 26-29 are labels that say "score" and "wave"
// 30 empty
// 31 empty

// define the sprite control reg slot for each sprite, 0-127
// 10? bullets
// 1 player + 2 life icons
// 8? score digits
// 1? wave digit
// 8x4=32? enemies, up to 55
// 4x4x3=48? bunker icons
#define SPRITE_BULLET_0			0
// 0-9 for bullets
#define SPRITE_LIFE_A			11
#define SPRITE_LIFE_B			12
#define SPRITE_WAVE_DIG			13
#define SPRITE_SCORE_DIG_0		14 //[14:21](8) for score
#define SPRITE_BUNKER_0			22 //[22:45](4x2x3= 24) for bunkers, maybe more
#define SPRITE_BUNKER_X(j,k,l)	((j * BUNKER_COLS*BUNKER_ROWS) + (k * BUNKER_COLS) + l + SPRITE_BUNKER_0)
// gap [46:59]
#define SPRITE_ENEMY_0			60 //[60:114](55) for enemies (5x11) maybe less
#define SPRITE_ENEMY_X(j,k)		((j * ENEMY_COLS) + k + SPRITE_ENEMY_0)

// gap [115:121]
// NOTE: SPRITE_SHIP_TWO must be in position SPRITE_SHIP+1
#define SPRITE_SHIP				122
#define SPRITE_SHIP_TWO			123

#define SPRITE_SCORE_A			124 // the labels that say "score" and "wave"
#define SPRITE_SCORE_B			125
#define SPRITE_WAVE_A			126
#define SPRITE_WAVE_B			127

#endif
