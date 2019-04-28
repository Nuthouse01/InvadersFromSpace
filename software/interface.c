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
#include "interface.h"
#include "mfp_memory_mapped_registers.h"
#include "load_sprites.h"
#include "params.h"

#ifdef SDL2
#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>

extern SDL_Window* window;
extern SDL_Renderer* renderer;

// these are used to emulate the physical timers with something clever
long virtual_offset_time0 = 0;
long virtual_offset_time1 = 0;

// these emulate LEDs
SDL_Rect led_rects[16];
int led_values[16];

// this is used to emulate the 7seg display
// (note: if we are displaying GRAPHICS correctly we dont need this cuz displayed score always matches 7seg for now)
int sevseg_enable[8];
int sevseg_dec[8];
int sevseg_digits[8];

// this is the sprite graphics RAM aka palette
SDL_Texture* palette[32] = {NULL};

// this is one sprite-control-register-set
struct sprite_ctrl {
	int xpos;
	int ypos;
	int xsize;
	int ysize;
	int enable;
	enum palette_index palette_idx; // indicate which image to display here
};
// this is the whole sprite-graphics module
struct sprite_ctrl GRAPHICS[128];

#else
#endif




//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
// does bootup initialization like filling the sprite RAM
int init() {
#ifdef SDL2
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Invaders from Space software emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0);
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
		return 2;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
		return 3;
	}

	#define PATH_PREFIX		"../media_resources"
	// specify the names of the input files (order corresponds to palette index!)
	char* filenames[32];
	// digits
	filenames[0] =  PATH_PREFIX"/final_Num_0.png";
	filenames[1] =  PATH_PREFIX"/final_Num_1.png";
	filenames[2] =  PATH_PREFIX"/final_Num_2.png";
	filenames[3] =  PATH_PREFIX"/final_Num_3.png";
	filenames[4] =  PATH_PREFIX"/final_Num_4.png";
	filenames[5] =  PATH_PREFIX"/final_Num_5.png";
	filenames[6] =  PATH_PREFIX"/final_Num_6.png";
	filenames[7] =  PATH_PREFIX"/final_Num_7.png";
	filenames[8] =  PATH_PREFIX"/final_Num_8.png";
	filenames[9] =  PATH_PREFIX"/final_Num_9.png";
	// explode, bullet, & ship
	filenames[10] = PATH_PREFIX"/final_Explosion_1.png";
	filenames[11] = PATH_PREFIX"/final_Explosion_2.png";
	filenames[12] = PATH_PREFIX"/final_Bullet_player.png";
	//filenames[13] = PATH_PREFIX"/offset-core-cannon.png";
	filenames[13] = PATH_PREFIX"/final_Player.png";
	// enemies
	//filenames[14] = PATH_PREFIX"/large-thicc-space-invader0.png";
	filenames[14] = PATH_PREFIX"/final_AlienA_frame1.png";
	filenames[15] = PATH_PREFIX"/final_AlienB_frame1.png";
	//filenames[16] = PATH_PREFIX"/large-thicc-space-invader1.png";
	filenames[16] = PATH_PREFIX"/final_AlienA_frame2.png";
	filenames[17] = PATH_PREFIX"/final_AlienB_frame2.png";
	filenames[18] = PATH_PREFIX"/final_Alien_Special.png";
	// bunker blocks
	filenames[19] = PATH_PREFIX"/final_Bunker_square_1.png";
	filenames[20] = PATH_PREFIX"/final_Bunker_square_2.png";
	filenames[21] = PATH_PREFIX"/final_Bunker_square_3.png";
	filenames[22] = PATH_PREFIX"/final_Bunker_left_1.png";
	filenames[23] = PATH_PREFIX"/final_Bunker_left_2.png";
	filenames[24] = PATH_PREFIX"/final_Bunker_right_1.png";
	filenames[25] = PATH_PREFIX"/final_Bunker_right_2.png";
	// labels
	filenames[26] = PATH_PREFIX"/final_Label_Score1.png";
	filenames[27] = PATH_PREFIX"/final_Label_Score2.png";
	filenames[28] = PATH_PREFIX"/final_Label_Wave1.png";
	filenames[29] = PATH_PREFIX"/final_Label_Wave2.png";
	// unused
	filenames[30] = PATH_PREFIX"/final_Player_2.png";
	filenames[31] = PATH_PREFIX"/placeholder.png";


	// read the files in and copy them into the palette
	for (unsigned long i = 0; i < 32; i++) {
		SDL_Surface* surface = IMG_Load(filenames[i]);
		if (surface == NULL) {
			fprintf(stderr, "IMG_Load error: %s\n", SDL_GetError());
			return 4;
		}
		palette[i] = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
	}

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	// build the LED display things
	for (int i = 0; i < 16; i++) {
		led_rects[i].x = 22 * i + 2;
		led_rects[i].y = h - 20 - 2;
		led_rects[i].w = 20;
		led_rects[i].h = 20;
		led_values[i] = i / 8; // 0 for bottom 8 and 1 for the top
	}

	// init the GRAPHICS array with zeros
	for(int i=0; i<128; i++) {
		struct sprite_ctrl p = {0};
		GRAPHICS[i] = p;
	}

	// NOTE: when writing ypos = DISABLE_SPRITE_Y, it sets enable to 0... writing anything else sets it to 1

	// cheat a little! set the xsize and ysize of each entry, even tho other stuff isn't yet set
	// the sprite it displays may change(animate) but its size will stay constant
	// BULLET
	for(int i=0; i < MAX_BULLETS; i++) {
		(GRAPHICS[SPRITE_BULLET_0 + i]).xsize = BULLET_SIZE_X;
		(GRAPHICS[SPRITE_BULLET_0 + i]).ysize = BULLET_SIZE_Y;
	}
	// ENEMY
	for(int i=0; i<(ENEMY_ROWS*ENEMY_COLS); i++) {
		(GRAPHICS[SPRITE_ENEMY_0 + i]).xsize = ENEMY_SIZE_X;
		(GRAPHICS[SPRITE_ENEMY_0 + i]).ysize = ENEMY_SIZE_Y;
	}
	// SHIP
	// LIVES
	(GRAPHICS[SPRITE_SHIP]).xsize = SHIP_SIZE_X;
	(GRAPHICS[SPRITE_SHIP_TWO]).xsize = SHIP_SIZE_X;
	(GRAPHICS[SPRITE_LIFE_A]).xsize = SHIP_SIZE_X;
	(GRAPHICS[SPRITE_LIFE_B]).xsize = SHIP_SIZE_X;
	(GRAPHICS[SPRITE_SHIP]).ysize = SHIP_SIZE_Y;
	(GRAPHICS[SPRITE_SHIP_TWO]).ysize = SHIP_SIZE_Y;
	(GRAPHICS[SPRITE_LIFE_A]).ysize = SHIP_SIZE_Y;
	(GRAPHICS[SPRITE_LIFE_B]).ysize = SHIP_SIZE_Y;
	// BUNKER
	for(int i=0; i<(NUM_BUNKERS*BUNKER_ROWS*BUNKER_COLS); i++) {
		(GRAPHICS[SPRITE_BUNKER_0 + i]).xsize = BUNKER_SIZE_X;
		(GRAPHICS[SPRITE_BUNKER_0 + i]).ysize = BUNKER_SIZE_Y;
	}
	// DIGIT
	for(int i=0; i<10; i++) {
		(GRAPHICS[SPRITE_SCORE_DIG_0 + i]).xsize = DIGIT_SIZE_X;
		(GRAPHICS[SPRITE_SCORE_DIG_0 + i]).ysize = DIGIT_SIZE_Y;
	}
	(GRAPHICS[SPRITE_WAVE_DIG]).xsize = DIGIT_SIZE_X;
	(GRAPHICS[SPRITE_WAVE_DIG]).ysize = DIGIT_SIZE_Y;
	// LABEL
	(GRAPHICS[SPRITE_SCORE_A]).xsize = LABEL_SIZE_X;
	(GRAPHICS[SPRITE_SCORE_A]).ysize = LABEL_SIZE_Y;
	(GRAPHICS[SPRITE_SCORE_B]).xsize = LABEL_SIZE_X;
	(GRAPHICS[SPRITE_SCORE_B]).ysize = LABEL_SIZE_Y;
	(GRAPHICS[SPRITE_WAVE_A]).xsize = LABEL_SIZE_X;
	(GRAPHICS[SPRITE_WAVE_A]).ysize = LABEL_SIZE_Y;
	(GRAPHICS[SPRITE_WAVE_B]).xsize = LABEL_SIZE_X;
	(GRAPHICS[SPRITE_WAVE_B]).ysize = LABEL_SIZE_Y;


#else
	// load sprite palette into the palette RAM
	fill_and_load_sprite_palette();

#endif

	return 0;
}


// only used for SDL2 stuff, MIPS never quits it just runs infinite loop style
void quit() {
#ifdef SDL2
	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
	}
	if (window != NULL) {
		SDL_DestroyWindow(window);
	}
	for (unsigned long i = 0; i < sizeof(palette) / sizeof(SDL_Surface*); i++) {
		if (palette[i]) {
			SDL_DestroyTexture(palette[i]);
		}
	}

	SDL_Quit();
#endif

	return;
}


// only used for SDL2 stuff, MIPS never renders, the hardware does it continually
// this specifically draws what we expect to see on the main menu
void render_main() {
#ifdef SDL2
	SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255);
	SDL_RenderClear(renderer);

	// draw the LEDs
	for (int i = 0; i < 16; i++) {
		if (led_values[i]) {
			SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
		} else {
			SDL_SetRenderDrawColor(renderer, 25, 0, 0, 255);
		}
		SDL_RenderFillRect(renderer, &led_rects[i]);
	}
	SDL_RenderPresent(renderer);

#endif
	return;
}

// this draws in the main wave-loop
void draw() {
#ifdef SDL2
	SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
	SDL_RenderClear(renderer);

	// draw the LEDs
	for (int i = 0; i < 16; i++) {
		if (led_values[i]) {
			SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
		} else {
			SDL_SetRenderDrawColor(renderer, 25, 0, 0, 255);
		}
		SDL_RenderFillRect(renderer, &led_rects[i]);
	}

	// for each entry in the GRAPHICS array, if its enabled, draw a box and put the appropriate texture on it
	for(int i=0; i < 128; i++) {
		struct sprite_ctrl p = GRAPHICS[i];
		if (p.enable) {
			SDL_Rect rect;
			rect.x = p.xpos;
			rect.y = p.ypos;
			rect.w = p.xsize;
			rect.h = p.ysize;
			SDL_RenderCopy(renderer, palette[p.palette_idx], NULL, &rect);
		}
	}
	SDL_RenderPresent(renderer);
#endif
	return;
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
// actual GPIO stuff
// seed & read the random number generator
void seed(int seed) {
#ifdef SDL2
	srand(seed);
#else
	// seed the LFSR if it hasn't been seeded yet, human delay before press/release serves as seed
	// seed it with the time on timer0 (done in hardware)
	MFP_RAND = seed;
#endif
	return;
}

int get_random() {
#ifdef SDL2
	return (int)rand();
#else
	return MFP_RAND;
#endif
}

long read_switches() {
#ifdef SDL2
	return SDL_MULTIPLAYER;
#else
	return MFP_SWITCHES;
#endif
}



// read the current button values
// SDL version maps keyboard buttons to the MIPS button-byte positions
long read_butts() {
	long butts = 0;
#ifdef SDL2
	SDL_Event event;
	while (SDL_PollEvent(&event)) {}
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	// SDL quit via escape
	if (state[SDL_SCANCODE_ESCAPE]) {
		quit();
		exit(0);
	}
	if (state[SDL_SCANCODE_LEFT]) {
		butts |= BUTTON_LEFT_A;
	}
	if (state[SDL_SCANCODE_RIGHT]) {
		butts |= BUTTON_RIGHT_A;
	}
	if (state[SDL_SCANCODE_SPACE]) {
		butts |= BUTTON_SHOOT_A;
	}
	if (state[SDL_SCANCODE_UP]) {
		butts |= BUTTON_UP_A;
	}
	if (state[SDL_SCANCODE_DOWN]) {
		butts |= BUTTON_DOWN_A;
	}

	if (state[SDL_SCANCODE_A]) {
		butts |= BUTTON_LEFT_B;
	}
	if (state[SDL_SCANCODE_D]) {
		butts |= BUTTON_RIGHT_B;
	}
	if (state[SDL_SCANCODE_W]) {
		butts |= BUTTON_SHOOT_B;
	}
	if (state[SDL_SCANCODE_Q]) {
		butts |= BUTTON_UP_B;
	}
	if (state[SDL_SCANCODE_S]) {
		butts |= BUTTON_DOWN_B;
	}
#else
	butts = MFP_BUTTONS; // this is actually really clever - expands into a read. maybe.
	#ifndef USE_PADDLE
	// if not using the paddle, then zero out these button signals
	butts &= ~BUTTON_SHOOT_A;
	butts &= ~BUTTON_SHOOT_B;
	#endif
#endif
	return butts;
}


// read the currently-displayed leds
int read_leds() {
	int retme = 0;
#ifdef SDL2
	for (int i = 0; i < 16; i++) {
		retme |= (led_values[i] << i);
	}
#else
	retme = MFP_LEDS;
#endif
	return retme;
}


// write to the leds
void write_leds(int writeme) {
#ifdef SDL2
	for (int i = 0; i < 16; i++) {
		led_values[i] = (writeme >> i) & 1;
	}
#else
	MFP_LEDS = writeme;
#endif
}



// 7seg interface
void write_7seg_enable(int en) {
#ifdef SDL2
	for (int i = 0; i < 8; i++) {
		sevseg_enable[i] = (en >> i) & 1;
	}
#else
	MFP_7SEG_EN = en;
#endif
}

void write_7seg_dec(int dec) {
#ifdef SDL2
	for (int i = 0; i < 8; i++) {
		sevseg_dec[i] = (dec >> i) & 1;
	}
#else
	MFP_7SEG_DP = dec;
#endif
}

void write_7seg_high(long high) {
#ifdef SDL2
	for (int i = 0; i < 4; i++) {
		sevseg_digits[i+4] = (high >> (i*8)) & 0x001F;
	}
#else
	MFP_7SEG_HDIG = high;
#endif
}

void write_7seg_low(long low) {
#ifdef SDL2
	for (int i = 0; i < 4; i++) {
		sevseg_digits[i] = (low >> (i*8)) & 0x001F;
	}
#else
	MFP_7SEG_LDIG = low;
#endif
}


int read_xadc_a() {
#ifdef SDL2
	// TODO: xadc for SDL? not actually used in SDL path
	return 42;
#else
	return MFP_XADC_RESULTA;
#endif
}

int read_xadc_b() {
#ifdef SDL2
	// TODO: xadc for SDL? not actually used in SDL path
	return 42;
#else
	return MFP_XADC_RESULTB;
#endif
}


void write_audio_soundfx(int channels, int input) {
#ifdef SDL2
	printf("write_audio_soundfx: c=%i, i=%li\n", channels, input);
#else
	long temp = channels << AUD_CHANOFFSET;
	temp |= input;
	MFP_AUDIO_SOUNDFX = temp;
#endif
}
// NOTE: unused & unimplemented
void write_audio_tonegen(int channels, long input) {
#ifdef SDL2
	printf("write_audio_tonegen: c=%i, i=%li\n", channels, input);
#else
	long temp = channels << AUD_CHANOFFSET;
	temp |= input;
	MFP_AUDIO_TONEGEN = temp;
#endif
}
void write_audio_status(int pause, int resume, int stop) {
#ifdef SDL2
	printf("write_audio_status: p=%i,r=%i,s=%i\n", pause,resume,stop);
	// TODO
#else
	long temp = pause << AUD_PAUSEOFFSET;
	temp |= resume << AUD_RESUMEOFFSET;
	temp |= stop << AUD_STOPOFFSET;
	MFP_AUDIO_STATUS = temp;
#endif
}
// NOTE: unused
long read_audio_status() {
#ifdef SDL2
	printf("read_audio_status\n");
	// TODO
#else
	return MFP_AUDIO_STATUS;
#endif
}



//////////////////////////////////////////////////
// read/write timer0/timer1
void write_timer0(long new_now) {
#ifdef SDL2
	//last_virtual_time0 = new_now;
	virtual_offset_time0 += read_timer0() - new_now;
#else
	MFP_TIMER0 = new_now;
#endif
	return;
}

long read_timer0() {
#ifdef SDL2
	long now = SDL_GetTicks();
	return (now - virtual_offset_time0);
#else
	return MFP_TIMER0;
#endif
}

void write_timer1(long new_now) {
#ifdef SDL2
	//last_virtual_time1 = new_now;
	virtual_offset_time1 += read_timer1() - new_now;
#else
	MFP_TIMER1 = new_now;
#endif
	return;
}

long read_timer1() {
#ifdef SDL2
	long now = SDL_GetTicks();
	return (now - virtual_offset_time1);
#else
	return MFP_TIMER1;
#endif
}



//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
// these functions are placeholders for interacting with the AHB sprite graphics module
// assuming 256 sets of sprite control registers
// still not quite sure how the sprite RAM gets filled/addressed but we'll get to it
// assuming they are in the order ADDR/X/Y/scale
void write_sprite_xpos(int spriteidx, int xpos) {
#ifdef SDL2
	(GRAPHICS[spriteidx]).xpos = xpos;
#else
	// make the address that I'm trying to write into
	// sprite slot "spriteidx" gets turned into an actual address
	// assumign there are 4 words for each control set, each with 4 bytes, -> each set has 16 bytes
	long dest = MFP_SPRITE_XPOS(spriteidx);
	MFP_RW_MANUAL(dest) = xpos;
#endif
}
int read_sprite_xpos(int spriteidx) {
#ifdef SDL2
	return (GRAPHICS[spriteidx]).xpos;
#else
	// sprite slot "spriteidx" gets turned into an actual address
	long dest = MFP_SPRITE_XPOS(spriteidx);
	return MFP_RW_MANUAL(dest);
#endif
}



void write_sprite_ypos(int spriteidx, int ypos) {
#ifdef SDL2
	(GRAPHICS[spriteidx]).ypos = ypos;
	if(ypos == DISABLE_SPRITE_Y) {
		(GRAPHICS[spriteidx]).enable = 0;
	} else {
		(GRAPHICS[spriteidx]).enable = 1;
	}
#else
	// sprite slot "spriteidx" gets turned into an actual address
	long dest = MFP_SPRITE_YPOS(spriteidx);
	MFP_RW_MANUAL(dest) = ypos;
#endif
}
int read_sprite_ypos(int spriteidx) {
#ifdef SDL2
	return (GRAPHICS[spriteidx]).ypos;
#else
	// sprite slot "spriteidx" gets turned into an actual address
	long dest = MFP_SPRITE_YPOS(spriteidx);
	return MFP_RW_MANUAL(dest);
#endif
}



void write_sprite_idx(int spriteidx, enum palette_index idx) {
#ifdef SDL2
	(GRAPHICS[spriteidx]).palette_idx = idx;
#else
	// sprite slot "spriteidx" gets turned into an actual address
	long dest = MFP_SPRITE_IDX(spriteidx);
	MFP_RW_MANUAL(dest) = idx;
#endif
}
enum palette_index read_sprite_idx(int spriteidx) {
#ifdef SDL2
	return (GRAPHICS[spriteidx]).palette_idx;
#else
	// sprite slot "spriteidx" gets turned into an actual address
	long dest = MFP_SPRITE_IDX(spriteidx);
	return MFP_RW_MANUAL(dest);
#endif
}

/*
void write_sprite_scale(int spriteidx, int scale) {
#ifdef SDL2
	// ??????
#else
	// sprite slot "spriteidx" gets turned into an actual address
	long dest = MFP_SPRITE_SCALE(spriteidx);
	MFP_RW_MANUAL(dest) = scale;
#endif
}
int read_sprite_scale(int spriteidx) {
	int retme = 0;
#ifdef SDL2
	// ??????
#else
	// sprite slot "spriteidx" gets turned into an actual address
	long dest = MFP_SPRITE_SCALE(spriteidx);
	retme = MFP_RW_MANUAL(dest);
#endif
	return retme;
}
*/

int print(const char *format, ...) {
#ifdef SDL2
	va_list arg;
	int done;
	va_start (arg, format);
	done = vfprintf(stdout, format, arg);
	va_end (arg);
	return done;
#else
	return 0;
#endif
}
