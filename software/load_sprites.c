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
#include "load_sprites.h"
#include "mfp_memory_mapped_registers.h"
#include "params.h"

// this file is for writing the sprite data from prog mem into the sprite mem
// this feature may need to be removed if the program gets too big (unlikely)
// advantage: no need to resynthesize each time we change some of the art!

void load_sprite(enum palette_index sprite_idx, char sprite[16][16]) {
	// sprite_idx is int 0-30 or so, represents which "sprite slot" to fill

	// sprite is an int[16][16] array that will be written into that area
	// assuming top-left(0,0) of sprite is displayed as top-left
	// assuming low bytes in RAM =right= 0,0, visually flipped right-to-left

	// each pixel in 16x16 img has 4 bits
	// 8 pixels per word
	// 32 words for 1 sprite, 32x4=128 bytes per sprite
	long dest = MFP_SPRITE_PALETTE_BASE_ADDR + (sprite_idx * 128);

	for(int row=0; row<16; row++) {
		long word1 = 0;
		for(int col=7; col>=0; col--) {
			// mask to be sure, and shift into position
			long t = (sprite[row][col] & 0xF) << ((7-col)*4);
			// accumulate it into "word"
			word1 |= t;
		}
		int word2 = 0;
		for(int col=7; col>=0; col--) {
			// mask to be sure, and shift into position
			long t = (sprite[row][col+8] & 0xF) << ((7-col)*4);
			// accumulate it into "word"
			word2 |= t;
		}
		// write it
		MFP_RW_MANUAL(dest) = word1;
		// point to the next word
		dest += 4;
		// write it
		MFP_RW_MANUAL(dest) = word2;
		// point to the next word
		dest += 4;
	}


}




// fills & loads all sprites into the sprite palette
void fill_and_load_sprite_palette() {

	// TODO: 16x16 SPRITES GO HERE

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	// DIGITS (10)

	char spritearr_dig0[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_NUM0, spritearr_dig0);


	char spritearr_dig1[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_NUM1, spritearr_dig1);


	char spritearr_dig2[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_NUM2, spritearr_dig2);


	char spritearr_dig3[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_NUM3, spritearr_dig3);


	char spritearr_dig4[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 3, 3, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_NUM4, spritearr_dig4);


	char spritearr_dig5[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_NUM5, spritearr_dig5);



	char spritearr_dig6[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0	},
			{	0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_NUM6, spritearr_dig6);


	char spritearr_dig7[16][16] = {

    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0 },
    { 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0 },
    { 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	load_sprite(PALETTE_NUM7, spritearr_dig7);


	char spritearr_dig8[16][16] = {

    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0 },
    { 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0 },
    { 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 3, 3, 3, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};
	load_sprite(PALETTE_NUM8, spritearr_dig8);


	char spritearr_dig9[16][16] = {

    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0 },
    { 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 3, 3, 3, 3, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};
	load_sprite(PALETTE_NUM9, spritearr_dig9);

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	// EXPLOSION, BULLET, SHIP (4)

	char spritearr_explode_a[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 2, 0, 0	},
			{	0, 1, 2, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1, 0, 0	},
			{	0, 0, 0, 2, 0, 2, 6, 6, 1, 1, 2, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 2, 6, 1, 1, 6, 6, 6, 2, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 2, 6, 6, 1, 1, 6, 6, 2, 0, 0, 2, 0	},
			{	0, 0, 0, 2, 0, 6, 6, 1, 6, 1, 6, 2, 0, 2, 0, 0	},
			{	0, 0, 0, 0, 2, 2, 1, 6, 6, 6, 2, 0, 1, 0, 0, 0	},
			{	0, 1, 0, 2, 0, 2, 2, 0, 2, 2, 0, 2, 0, 0, 0, 0	},
			{	0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0	},
			{	0, 0, 0, 2, 0, 2, 0, 0, 2, 1, 2, 0, 2, 0, 1, 0	},
			{	0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_EXPLODE_A, spritearr_explode_a);


	char spritearr_explode_b[16][16] = {
			{	0, 1, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 2, 0, 1, 0	},
			{	0, 0, 2, 0, 0, 2, 1, 0, 0, 2, 0, 1, 0, 0, 0, 0	},
			{	1, 0, 2, 1, 0, 0, 0, 2, 0, 1, 2, 0, 2, 6, 2, 0	},
			{	2, 2, 0, 2, 2, 0, 1, 0, 2, 0, 2, 6, 6, 0, 0, 2	},
			{	0, 2, 6, 6, 6, 2, 0, 2, 6, 2, 6, 2, 0, 1, 2, 0	},
			{	0, 0, 2, 0, 6, 6, 2, 6, 1, 2, 6, 0, 2, 2, 0, 0	},
			{	0, 2, 0, 2, 2, 6, 6, 1, 6, 1, 6, 2, 6, 2, 6, 2	},
			{	2, 1, 0, 0, 1, 6, 1, 1, 1, 6, 1, 6, 6, 0, 0, 2	},
			{	0, 0, 0, 0, 2, 6, 1, 1, 6, 6, 2, 0, 0, 2, 0, 0	},
			{	0, 0, 2, 2, 6, 6, 2, 6, 6, 0, 6, 2, 0, 0, 0, 6	},
			{	1, 2, 0, 6, 6, 2, 0, 2, 6, 6, 2, 0, 0, 0, 2, 0	},
			{	0, 0, 2, 1, 2, 0, 6, 0, 2, 6, 2, 2, 0, 1, 0, 0	},
			{	0, 6, 0, 0, 0, 2, 6, 2, 0, 2, 6, 1, 2, 0, 0, 0	},
			{	2, 0, 2, 1, 0, 2, 0, 2, 0, 1, 0, 6, 0, 2, 0, 1	},
			{	0, 2, 0, 0, 1, 0, 6, 0, 0, 2, 0, 2, 0, 2, 0, 0	},
			{	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0	}
	};
	load_sprite(PALETTE_EXPLODE_B, spritearr_explode_b);


	char spritearr_bullet[16][16] = {
			{	0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	3, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_BULLET, spritearr_bullet);


	char spritearr_ship[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 8, 9, 9, 8, 0, 0, 0, 0, 0, 0	},
			{	3, 0, 0, 0, 0, 0, 9, 3, 3, 9, 0, 0, 0, 0, 0, 3	},
			{	3, 9, 0, 0, 0, 8, 9, 3, 3, 9, 8, 0, 0, 0, 9, 3	},
			{	3, 9, 0, 0, 9, 3, 3, 1, 1, 3, 3, 9, 0, 0, 9, 3	},
			{	1, 3, 9, 8, 3, 3, 1, 1, 1, 1, 3, 3, 8, 9, 3, 1	},
			{	3, 9, 9, 3, 3, 8, 8, 8, 8, 8, 8, 3, 3, 9, 9, 3	},
			{	9, 8, 1, 3, 8, 9, 9, 9, 9, 9, 9, 8, 3, 1, 8, 9	},
			{	8, 9, 1, 3, 9, 3, 9, 9, 9, 9, 3, 9, 3, 1, 9, 8	},
			{	0, 9, 9, 9, 8, 1, 9, 9, 9, 9, 1, 8, 9, 9, 9, 0	},
			{	0, 8, 8, 9, 9, 8, 8, 9, 9, 8, 8, 9, 9, 8, 8, 0	},
			{	0, 0, 8, 8, 8, 9, 8, 9, 9, 8, 9, 8, 8, 8, 0, 0	},
			{	0, 0, 0, 0, 8, 8, 8, 9, 8, 9, 8, 8, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 8, 9, 8, 9, 8, 8, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 8, 9, 9, 8, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_SHIP, spritearr_ship);




	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	// ENEMIES (5)

	char spritearr_enemy_a1[16][16] = {
			{	0, 0, 0, 4, 0, 5, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0	},
			{	0, 0, 4, 0, 0, 0, 5, 0, 0, 5, 0, 0, 0, 4, 0, 0	},
			{	0, 4, 0, 5, 4, 4, 0, 4, 4, 0, 4, 4, 5, 0, 4, 0	},
			{	4, 0, 5, 0, 4, 5, 0, 5, 5, 0, 5, 4, 0, 5, 0, 4	},
			{	4, 0, 5, 0, 4, 5, 5, 5, 5, 5, 5, 4, 0, 5, 0, 4	},
			{	0, 4, 0, 5, 4, 4, 4, 0, 0, 4, 4, 4, 5, 0, 4, 0	},
			{	0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0	},
			{	0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_ENEMY_A1, spritearr_enemy_a1);


	char spritearr_enemy_a2[16][16] = {
			{	4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4	},
			{	0, 4, 0, 0, 0, 5, 4, 0, 0, 4, 5, 0, 0, 0, 4, 0	},
			{	5, 0, 4, 0, 4, 4, 5, 4, 4, 5, 4, 4, 0, 4, 0, 5	},
			{	0, 5, 0, 4, 4, 5, 1, 5, 5, 1, 5, 4, 4, 0, 5, 0	},
			{	0, 5, 0, 4, 4, 5, 5, 5, 5, 5, 5, 4, 4, 0, 5, 0	},
			{	5, 0, 4, 0, 4, 4, 4, 0, 0, 4, 4, 4, 0, 4, 0, 5	},
			{	0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0	},
			{	4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_ENEMY_A2, spritearr_enemy_a2);


	char spritearr_enemy_b1[16][16] = {
			{	0, 4, 4, 5, 5, 5, 5, 0, 0, 5, 5, 5, 5, 4, 4, 0	},
			{	4, 0, 0, 5, 4, 4, 5, 5, 5, 5, 4, 4, 5, 0, 0, 4	},
			{	0, 0, 4, 5, 5, 0, 4, 5, 5, 4, 0, 5, 5, 4, 0, 0	},
			{	0, 4, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 4, 0	},
			{	4, 0, 0, 0, 0, 4, 4, 0, 0, 4, 4, 0, 0, 0, 0, 4	},
			{	0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_ENEMY_B1, spritearr_enemy_b1);


	char spritearr_enemy_b2[16][16] = {
			{	4, 0, 4, 5, 5, 5, 5, 0, 0, 5, 5, 5, 5, 4, 0, 4	},
			{	0, 4, 0, 5, 4, 4, 5, 5, 5, 5, 4, 4, 5, 0, 4, 0	},
			{	4, 0, 4, 5, 5, 1, 4, 5, 5, 4, 1, 5, 5, 4, 0, 4	},
			{	0, 4, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 4, 0	},
			{	0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 4, 4, 0, 0, 4, 4, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_ENEMY_B2, spritearr_enemy_b2);


	char spritearr_enemy_special[16][16] = {
			{	0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 1, 7, 1, 7, 1, 7, 1, 7, 0, 0, 0, 0	},
			{	0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0	},
			{	0, 0, 4, 4, 5, 4, 5, 4, 5, 4, 5, 4, 4, 4, 0, 0	},
			{	0, 4, 4, 5, 4, 5, 4, 5, 5, 5, 4, 5, 5, 4, 4, 0	},
			{	5, 4, 5, 4, 5, 5, 4, 4, 4, 4, 5, 4, 4, 5, 4, 5	},
			{	4, 7, 7, 7, 7, 4, 4, 7, 7, 4, 4, 7, 7, 7, 7, 4	},
			{	1, 7, 1, 7, 1, 4, 7, 1, 1, 7, 4, 1, 7, 1, 7, 1	},
			{	4, 7, 7, 7, 7, 4, 4, 7, 7, 4, 4, 7, 7, 7, 7, 4	},
			{	5, 4, 5, 4, 4, 5, 4, 4, 4, 4, 5, 4, 4, 5, 4, 5	},
			{	0, 4, 4, 5, 5, 4, 5, 5, 4, 5, 4, 5, 5, 4, 4, 0	},
			{	0, 0, 4, 4, 4, 4, 5, 4, 5, 4, 4, 4, 4, 4, 0, 0	},
			{	0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0	},
			{	0, 0, 0, 0, 7, 1, 7, 1, 7, 1, 7, 1, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_ENEMY_SPECIAL, spritearr_enemy_special);




	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	// BUNKERS (7)

	char spritearr_BUNKER_FULL_A[16][16] = {
			{	9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8	},
			{	8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9	},
			{	9, 8, 1, 1, 8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 9, 8	},
			{	8, 9, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 8, 9	},
			{	9, 8, 8, 3, 3, 8, 8, 8, 8, 8, 8, 3, 3, 8, 9, 8	},
			{	8, 9, 8, 3, 8, 8, 1, 8, 8, 1, 8, 8, 3, 8, 8, 9	},
			{	9, 8, 8, 3, 8, 1, 8, 8, 8, 8, 1, 8, 3, 8, 9, 8	},
			{	8, 9, 8, 3, 8, 8, 8, 9, 9, 8, 8, 8, 3, 8, 8, 9	},
			{	9, 8, 8, 3, 8, 8, 8, 9, 9, 8, 8, 8, 3, 8, 9, 8	},
			{	8, 9, 8, 3, 8, 1, 8, 8, 8, 8, 1, 8, 3, 8, 8, 9	},
			{	9, 8, 8, 3, 8, 8, 1, 8, 8, 1, 8, 8, 3, 8, 9, 8	},
			{	8, 9, 8, 3, 3, 8, 8, 8, 8, 8, 8, 3, 3, 8, 8, 9	},
			{	9, 8, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 9, 8	},
			{	8, 9, 1, 1, 8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 8, 9	},
			{	9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8	},
			{	8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9	}
	};
	load_sprite(PALETTE_BUNKER_FULL_A, spritearr_BUNKER_FULL_A);


	char spritearr_BUNKER_FULL_B[16][16] = {
			{	9, 8, 9, 8, 9, 0, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8	},
			{	8, 9, 8, 9, 0, 0, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9	},
			{	9, 8, 8, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 8	},
			{	8, 9, 8, 8, 0, 9, 3, 3, 3, 3, 3, 9, 8, 8, 8, 0	},
			{	9, 8, 8, 3, 9, 0, 8, 8, 8, 8, 8, 0, 9, 0, 0, 8	},
			{	8, 9, 8, 3, 8, 0, 1, 8, 8, 1, 0, 8, 0, 8, 8, 0	},
			{	9, 8, 8, 3, 8, 1, 8, 8, 8, 8, 1, 8, 3, 8, 0, 8	},
			{	8, 9, 8, 3, 8, 8, 8, 9, 9, 8, 8, 8, 3, 8, 8, 9	},
			{	9, 8, 8, 3, 8, 8, 8, 9, 9, 8, 8, 8, 3, 8, 9, 8	},
			{	8, 9, 0, 9, 8, 1, 8, 8, 8, 8, 1, 8, 3, 8, 8, 9	},
			{	9, 0, 8, 0, 8, 8, 1, 8, 8, 1, 8, 8, 3, 8, 9, 8	},
			{	0, 9, 8, 9, 0, 8, 8, 8, 8, 0, 8, 3, 3, 8, 8, 9	},
			{	9, 8, 8, 8, 3, 3, 3, 9, 0, 3, 3, 3, 8, 8, 9, 8	},
			{	8, 9, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 8, 9	},
			{	9, 8, 9, 8, 9, 8, 9, 8, 0, 8, 0, 8, 9, 8, 9, 8	},
			{	8, 9, 8, 9, 8, 9, 8, 9, 8, 0, 8, 9, 8, 9, 8, 9	}
	};
	load_sprite(PALETTE_BUNKER_FULL_B, spritearr_BUNKER_FULL_B);


	char spritearr_BUNKER_FULL_C[16][16] = {
			{	9, 8, 9, 8, 9, 0, 0, 0, 9, 8, 9, 8, 9, 8, 9, 8	},
			{	8, 9, 8, 9, 0, 0, 0, 0, 0, 9, 8, 9, 8, 9, 8, 0	},
			{	9, 8, 8, 0, 0, 8, 0, 0, 9, 0, 8, 8, 8, 8, 0, 0	},
			{	0, 9, 8, 8, 0, 9, 3, 0, 0, 9, 3, 9, 0, 0, 0, 0	},
			{	9, 0, 8, 3, 9, 0, 8, 8, 0, 8, 8, 0, 0, 0, 0, 8	},
			{	8, 9, 0, 3, 8, 0, 1, 8, 8, 1, 8, 8, 0, 8, 8, 0	},
			{	9, 8, 8, 3, 8, 9, 8, 8, 8, 8, 9, 0, 0, 8, 0, 0	},
			{	0, 9, 8, 3, 8, 8, 8, 9, 9, 8, 0, 0, 0, 8, 8, 0	},
			{	0, 0, 0, 9, 8, 8, 8, 9, 9, 8, 8, 8, 9, 8, 9, 8	},
			{	0, 0, 0, 0, 0, 9, 8, 8, 8, 8, 1, 8, 3, 8, 8, 9	},
			{	0, 0, 8, 0, 0, 8, 1, 8, 8, 9, 8, 8, 3, 0, 9, 0	},
			{	0, 9, 8, 9, 0, 0, 8, 8, 8, 0, 8, 3, 0, 8, 0, 9	},
			{	9, 8, 8, 8, 9, 9, 3, 9, 0, 9, 9, 3, 8, 8, 9, 8	},
			{	8, 9, 8, 0, 8, 8, 8, 0, 0, 0, 0, 8, 8, 0, 8, 9	},
			{	9, 8, 0, 8, 9, 8, 9, 8, 0, 0, 0, 0, 0, 8, 9, 8	},
			{	8, 9, 8, 9, 8, 9, 8, 9, 8, 0, 0, 0, 8, 9, 8, 9	}
	};
	load_sprite(PALETTE_BUNKER_FULL_C, spritearr_BUNKER_FULL_C);



	char spritearr_BUNKER_LOUT_A[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 8, 8, 8	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 8, 8, 8, 9, 8	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 9, 8, 8, 8, 8, 9, 8, 9	},
			{	0, 0, 0, 0, 0, 0, 0, 9, 8, 8, 9, 8, 9, 8, 8, 8	},
			{	0, 0, 0, 0, 0, 0, 9, 8, 8, 9, 8, 9, 8, 9, 8, 8	},
			{	0, 0, 0, 0, 0, 9, 8, 8, 9, 8, 8, 8, 8, 8, 8, 8	},
			{	0, 0, 0, 0, 9, 8, 8, 9, 8, 8, 8, 8, 8, 8, 1, 8	},
			{	0, 0, 0, 9, 8, 8, 9, 8, 9, 8, 8, 1, 8, 9, 8, 8	},
			{	0, 0, 0, 9, 8, 9, 8, 8, 8, 8, 8, 8, 9, 3, 9, 8	},
			{	0, 0, 9, 8, 8, 8, 9, 8, 8, 8, 8, 8, 3, 1, 3, 8	},
			{	0, 0, 9, 8, 8, 9, 8, 8, 1, 8, 8, 8, 3, 1, 3, 8	},
			{	0, 9, 8, 8, 9, 8, 9, 8, 8, 9, 3, 3, 9, 1, 3, 8	},
			{	0, 9, 8, 9, 8, 8, 8, 8, 9, 3, 1, 1, 1, 1, 3, 8	},
			{	9, 8, 8, 8, 9, 8, 8, 1, 8, 9, 3, 3, 3, 3, 9, 8	},
			{	9, 8, 8, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8	}
	};
	load_sprite(PALETTE_BUNKER_LOUT_A, spritearr_BUNKER_LOUT_A);


	char spritearr_BUNKER_LOUT_B[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 8, 8, 8	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 9, 8	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 9, 8, 0, 8, 0, 0, 8, 9	},
			{	0, 0, 0, 0, 0, 0, 0, 9, 8, 8, 0, 0, 9, 8, 0, 8	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 8, 0, 0, 9, 8, 8	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 0, 8, 8, 8	},
			{	0, 0, 0, 0, 0, 0, 8, 9, 0, 0, 8, 0, 8, 8, 1, 8	},
			{	0, 0, 0, 9, 8, 0, 0, 8, 9, 8, 8, 1, 8, 9, 8, 8	},
			{	0, 0, 0, 9, 8, 9, 0, 0, 8, 8, 8, 8, 9, 3, 9, 8	},
			{	0, 0, 0, 0, 8, 8, 9, 8, 0, 8, 8, 8, 3, 8, 3, 8	},
			{	0, 0, 0, 0, 0, 0, 8, 8, 1, 8, 8, 8, 3, 8, 3, 8	},
			{	0, 0, 0, 0, 0, 0, 9, 8, 8, 9, 3, 3, 9, 8, 3, 8	},
			{	0, 9, 8, 0, 8, 0, 8, 8, 9, 3, 8, 8, 8, 8, 3, 8	},
			{	9, 8, 8, 0, 9, 8, 0, 1, 8, 9, 3, 3, 3, 3, 9, 8	},
			{	9, 8, 8, 9, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8	}
	};
	load_sprite(PALETTE_BUNKER_LOUT_B, spritearr_BUNKER_LOUT_B);


	char spritearr_BUNKER_ROUT_A[16][16] = {
			{	9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 8, 8, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 9, 8, 8, 8, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	9, 8, 9, 8, 8, 8, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 8, 8, 9, 8, 9, 8, 8, 9, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 8, 9, 8, 9, 8, 9, 8, 8, 9, 0, 0, 0, 0, 0, 0	},
			{	8, 8, 8, 8, 8, 8, 8, 9, 8, 8, 9, 0, 0, 0, 0, 0	},
			{	8, 1, 8, 8, 8, 8, 8, 8, 9, 8, 8, 9, 0, 0, 0, 0	},
			{	8, 8, 9, 8, 1, 8, 8, 9, 8, 9, 8, 8, 9, 0, 0, 0	},
			{	8, 9, 3, 9, 8, 8, 8, 8, 8, 8, 9, 8, 9, 0, 0, 0	},
			{	8, 3, 1, 3, 8, 8, 8, 8, 8, 9, 8, 8, 8, 9, 0, 0	},
			{	8, 3, 1, 3, 8, 8, 8, 1, 8, 8, 9, 8, 8, 9, 0, 0	},
			{	8, 3, 1, 9, 3, 3, 9, 8, 8, 9, 8, 9, 8, 8, 9, 0	},
			{	8, 3, 1, 1, 1, 1, 3, 9, 8, 8, 8, 8, 9, 8, 9, 0	},
			{	8, 9, 3, 3, 3, 3, 9, 8, 1, 8, 8, 9, 8, 8, 8, 9	},
			{	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 8, 8, 9	}
	};
	load_sprite(PALETTE_BUNKER_ROUT_A, spritearr_BUNKER_ROUT_A);


	char spritearr_BUNKER_ROUT_B[16][16] = {
			{	9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 8, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 9, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	9, 8, 0, 0, 8, 0, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 0, 8, 9, 0, 0, 8, 8, 9, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 8, 9, 0, 0, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 8, 8, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	8, 1, 8, 8, 0, 8, 0, 0, 9, 8, 0, 0, 0, 0, 0, 0	},
			{	8, 8, 9, 8, 1, 8, 8, 9, 8, 0, 0, 8, 9, 0, 0, 0	},
			{	8, 9, 3, 9, 8, 8, 8, 8, 0, 0, 9, 8, 9, 0, 0, 0	},
			{	8, 3, 8, 3, 8, 8, 8, 0, 8, 9, 8, 8, 0, 0, 0, 0	},
			{	8, 3, 8, 3, 8, 8, 8, 1, 8, 8, 0, 0, 0, 0, 0, 0	},
			{	8, 3, 8, 9, 3, 3, 9, 8, 8, 9, 0, 0, 0, 0, 0, 0	},
			{	8, 3, 8, 8, 8, 8, 3, 9, 8, 8, 0, 8, 0, 8, 9, 0	},
			{	8, 9, 3, 3, 3, 3, 9, 8, 1, 0, 8, 9, 0, 8, 8, 9	},
			{	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 9, 8, 8, 9	}
	};
	load_sprite(PALETTE_BUNKER_ROUT_B, spritearr_BUNKER_ROUT_B);




	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	// LABELS (4)

	char spritearr_LABEL_SCORE_A[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	3, 3, 0, 0, 0, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3	},
			{	3, 3, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 3, 3	},
			{	3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 3, 0, 3, 3	},
			{	3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3	},
			{	0, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3	},
			{	0, 0, 3, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3	},
			{	0, 0, 0, 0, 3, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3	},
			{	0, 0, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3	},
			{	0, 0, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3	},
			{	3, 0, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 3, 0, 3, 3	},
			{	0, 3, 0, 0, 3, 0, 0, 0, 3, 0, 0, 3, 0, 0, 3, 3	},
			{	0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_LABEL_SCORE_A, spritearr_LABEL_SCORE_A);


	char spritearr_LABEL_SCORE_B[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	3, 3, 0, 0, 3, 3, 3, 0, 0, 0, 3, 3, 3, 3, 3, 0	},
			{	0, 0, 3, 0, 3, 3, 0, 3, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 3, 0, 3, 3, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 3, 0, 3, 3, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 3, 0, 3, 3, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 3, 0, 3, 3, 0, 3, 0, 0, 3, 3, 3, 3, 3, 0	},
			{	0, 0, 3, 0, 3, 3, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 3, 0, 3, 0, 0, 3, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 3, 0, 3, 0, 0, 3, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 0, 3, 0, 3, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	3, 3, 0, 0, 3, 0, 0, 0, 3, 0, 3, 3, 3, 3, 3, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_LABEL_SCORE_B, spritearr_LABEL_SCORE_B);


	char spritearr_LABEL_WAVE_A[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0	},
			{	0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0	},
			{	0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3	},
			{	0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 3	},
			{	0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0	},
			{	0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0	},
			{	0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 3, 0, 0	},
			{	0, 0, 3, 3, 0, 0, 3, 0, 0, 0, 3, 0, 0, 3, 0, 0	},
			{	0, 0, 3, 3, 0, 3, 0, 3, 0, 0, 3, 0, 0, 3, 3, 3	},
			{	0, 0, 3, 3, 0, 3, 0, 3, 0, 3, 3, 0, 0, 3, 3, 3	},
			{	0, 0, 3, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 3, 0, 0	},
			{	0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 3, 0, 0	},
			{	0, 0, 0, 3, 3, 3, 0, 3, 3, 3, 0, 0, 0, 3, 0, 0	},
			{	0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_LABEL_WAVE_A, spritearr_LABEL_WAVE_A);


	char spritearr_LABEL_WAVE_B[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 3, 3, 3, 3, 3, 0	},
			{	3, 0, 0, 3, 3, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	3, 0, 0, 3, 3, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 3, 0, 3, 3, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 3, 0, 3, 3, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 3, 0, 3, 3, 0, 0, 0, 3, 0, 3, 3, 3, 3, 3, 0	},
			{	3, 3, 0, 3, 3, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	3, 3, 0, 3, 3, 0, 0, 0, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 3, 3, 0, 3, 3, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 3, 0, 3, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 0, 0	},
			{	0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 3, 3, 3, 3, 3, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_LABEL_WAVE_B, spritearr_LABEL_WAVE_B);


	char spritearr_ship_b[16][16] = {
			{	0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 8, 9, 9, 8, 0, 0, 0, 0, 0, 0	},
			{	2, 0, 0, 0, 0, 0, 9, 2, 2, 9, 0, 0, 0, 0, 0, 2	},
			{	2, 9, 0, 0, 0, 8, 9, 2, 2, 9, 8, 0, 0, 0, 9, 2	},
			{	2, 9, 0, 0, 9, 2, 2, 1, 1, 2, 2, 9, 0, 0, 9, 2	},
			{	1, 2, 9, 8, 2, 2, 1, 1, 1, 1, 2, 2, 8, 9, 2, 1	},
			{	2, 9, 9, 2, 2, 8, 8, 8, 8, 8, 8, 2, 2, 9, 9, 2	},
			{	9, 8, 1, 2, 8, 9, 9, 9, 9, 9, 9, 8, 2, 1, 8, 9	},
			{	8, 9, 1, 2, 9, 2, 9, 9, 9, 9, 2, 9, 2, 1, 9, 8	},
			{	0, 9, 9, 9, 8, 1, 9, 9, 9, 9, 1, 8, 9, 9, 9, 0	},
			{	0, 8, 8, 9, 9, 8, 8, 9, 9, 8, 8, 9, 9, 8, 8, 0	},
			{	0, 0, 8, 8, 8, 9, 8, 9, 9, 8, 9, 8, 8, 8, 0, 0	},
			{	0, 0, 0, 0, 8, 8, 8, 9, 8, 9, 8, 8, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 8, 9, 8, 9, 8, 8, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 8, 9, 9, 8, 0, 0, 0, 0, 0, 0	},
			{	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	}
	};
	load_sprite(PALETTE_SHIP_B, spritearr_ship_b);

}
