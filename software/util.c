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
#include "util.h"

// bunch of utility functions that don't cleanly fit anywhere els

// num-to-base-ten conversion
// same logic as the base_ten.S file
// returns pointer to an array that has both high=1 and low=0 results
// if blankpad=0, it left-pads with zeros; if blankpad=1, it left-pads with 31s (blanks)
#define BLANKVAL 		31
void to_base_ten(long in, int blankpad, long result[2]) {
	long quotient = 10000000;
	while(quotient > 0) {
		long v = in / quotient;	// digit value
		long r = in % quotient;	// remainder
		in = r; // save the remainder for next pass

		//printf("v=%li, r=%li, blankpad=%i\n", v, r, blankpad);

		if(blankpad) {
			if(v == 0) {
				v = BLANKVAL; // becomes blank when displayed
			} else {
				blankpad = 0; // disable blankpadding once a non-zero digit is found
			}
		}

		result[0] = result[0] << 8; // make room for new digit
		result[0] |= v; // OR in the new digit

		// if we just calculated the 5th digit, then copy result[0] to result[1]
		if(quotient == 10000) {
			result[1] = result[0];
		}

		quotient = quotient / 10; // move down to the next digit
	}
	// now its done! whole number is filled!
	// however, if converting 0 to base-ten and blank-pad is on, the whole thing is blanks... not ok
	// if at the end of the loop, blank-pad is still on, this has happened
	if(blankpad) {
		result[0] &= 0xFFFFFF00;
	}
	return;
}


// check to see if any two arbirtrary regions are overlapping!
// x/y are top-left corner of the areas in question
// NOTE: b must have smaller width/height than a for complete enclosure collision to work
// ... a=player/alien/bunker, b=bullet
// return 1 if collision, return 0 if not
int collide(int aleft, int atop, int aw, int ah, int bleft, int btop, int bw, int bh) {
	int aright = aleft + aw;
	int abot = atop + ah;
	int bright = bleft + bw;
	int bbot = btop + bh;
	// remember that aright > aleft
	// remember that atop < abot
	// if aleft is in [b], OR aright is in [b], OR bleft is in [a]
	if (((aleft <= bright) && (aleft >= bleft)) ||
		((aright <= bright) && (aright >= bleft)) ||
		((bleft <= aright) && (bleft >= aleft))) {
		// AND if atop is in [b], OR abot is in [b], OR btop is in [a]
		if (((atop <= bbot) && (atop >= btop)) ||
			((abot <= bbot) && (abot >= btop)) ||
			((btop <= abot) && (btop >= atop))) {
			// if both are true, then there is a collision
			return 1;
		}
	}
	//if (((aleft <= bright) && (bright <= aright)) || ((aleft <= bleft) && (bleft <= aright))) {
	//	if (((atop <= bbot) && (bbot <= abot)) || ((atop <= btop) && (btop <= abot))) {
	//		// if both are true, then there is a collision
	//		return 1;
	//	}
	//}
	return 0;
	// if we want to handle the case where b completely encloses a, OR the above with this:
	// (ax <= bx) && (ax + aw >= bx + bw)
}


// when a bit transitions from high to low (compared to the last invocation of this function)
// then this function returns an int with that bit high
int wait_for_button_release(int val) {
	static int lastval = 0;
	int lastval_copy = lastval;
	lastval = val; // save the current state to compare with the next read
	return (lastval_copy & ~val);
}



int my_clip(int in, int floor, int ceil) {
	if (in < floor) {
		in = floor;
	} else if (in > ceil) {
		in = ceil;
	}
	return in;
}
