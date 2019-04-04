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
#ifndef MFP_MEMORY_MAPPED_REGISTERS_H
#define MFP_MEMORY_MAPPED_REGISTERS_H

// this file originally provided with MIPSfpga-plus example
// adapted/expanded to suit our needs

// first, define the addresses
// gpio
#define MFP_LEDS_ADDR			0xBF800000
#define MFP_SWITCHES_ADDR		0xBF800004
#define MFP_BUTTONS_ADDR		0xBF800008
// 7seg
#define MFP_7SEG_EN_ADDR		0xBF700000
#define MFP_7SEG_LDIG_ADDR		0xBF700004
#define MFP_7SEG_HDIG_ADDR		0xBF700008
#define MFP_7SEG_DP_ADDR		0xBF70000C
// timers and random LFSR
#define MFP_TIMER0_ADDR			0xBF600000
#define MFP_TIMER1_ADDR			0xBF600004
#define MFP_RAND_ADDR			0xBF600010
// xadc interface
#define MFP_XADC_RESULTA_ADDR	0xBF400000
#define MFP_XADC_RESULTB_ADDR	0xBF400004
// audio, starting at 0xBF300000
#define MFP_AUDIO_SOUNDFX_ADDR		0xBF300000
#define MFP_AUDIO_TONEGEN_ADDR		0xBF300004
#define MFP_AUDIO_STATUS_ADDR		0xBF300008

// sprite palette address
#define MFP_SPRITE_PALETTE_BASE_ADDR 0xBF200000
// gfx sprite control regs
#define MFP_SPRITE_CTRL_BASE_ADDR 	0xBF100000
// assumign there are 4 words for each control set, each with 4 bytes, -> each set has 16 bytes
#define MFP_SPRITE_XPOS_OFFSET		0
#define MFP_SPRITE_XPOS(idx)		((idx * 16) + MFP_SPRITE_CTRL_BASE_ADDR + MFP_SPRITE_XPOS_OFFSET)
#define MFP_SPRITE_YPOS_OFFSET		4
#define MFP_SPRITE_YPOS(idx)		((idx * 16) + MFP_SPRITE_CTRL_BASE_ADDR + MFP_SPRITE_YPOS_OFFSET)
#define MFP_SPRITE_IDX_OFFSET		8
#define MFP_SPRITE_IDX(idx)			((idx * 16) + MFP_SPRITE_CTRL_BASE_ADDR + MFP_SPRITE_IDX_OFFSET)
#define MFP_SPRITE_SCALE_OFFSET		12
#define MFP_SPRITE_SCALE(idx)		((idx * 16) + MFP_SPRITE_CTRL_BASE_ADDR + MFP_SPRITE_SCALE_OFFSET)


//////////////// NOTE ///////////////////
// 128 sprite control register sets
// 32 16x16 sprites in the pallete (accessed via index)


// second, define macros(?) that use the addresses to create pointers
// these can be read/written as tho they were variables, probably?
// certainly they need to be volatile, but don't they also need to be "long long int"?
#define MFP_RW_MANUAL(addr) 	((* (volatile unsigned *) addr ))

#define MFP_LEDS				(* (volatile unsigned *) MFP_LEDS_ADDR ) //read/write
#define MFP_SWITCHES			(* (volatile unsigned *) MFP_SWITCHES_ADDR ) //read
#define MFP_BUTTONS				(* (volatile unsigned *) MFP_BUTTONS_ADDR ) //read

#define MFP_7SEG_EN				(* (volatile unsigned *) MFP_7SEG_EN_ADDR ) //read/write
#define MFP_7SEG_HDIG			(* (volatile unsigned *) MFP_7SEG_HDIG_ADDR ) //read/write
#define MFP_7SEG_LDIG			(* (volatile unsigned *) MFP_7SEG_LDIG_ADDR ) //read/write
#define MFP_7SEG_DP				(* (volatile unsigned *) MFP_7SEG_DP_ADDR ) //read/write

#define MFP_TIMER0				(* (volatile unsigned *) MFP_TIMER0_ADDR ) //read/write
#define MFP_TIMER1				(* (volatile unsigned *) MFP_TIMER1_ADDR ) //read/write
#define MFP_RAND				(* (volatile unsigned *) MFP_RAND_ADDR ) //read/write(seed from timer0, ignores the data actually written)

#define MFP_XADC_RESULTA		(* (volatile unsigned *) MFP_XADC_RESULTA_ADDR ) //read
#define MFP_XADC_RESULTB		(* (volatile unsigned *) MFP_XADC_RESULTB_ADDR ) //read

#define MFP_AUDIO_SOUNDFX		(* (volatile unsigned *) MFP_AUDIO_SOUNDFX_ADDR ) //write
#define MFP_AUDIO_TONEGEN		(* (volatile unsigned *) MFP_AUDIO_TONEGEN_ADDR ) //write
#define MFP_AUDIO_STATUS		(* (volatile unsigned *) MFP_AUDIO_STATUS_ADDR ) //read/write

// button masks
// shift 01234 are "real buttons" on the FPGA
// shift 56 are paddle "shoot buttons"
// greater numbers don't exist for hardware, therefore are impossible to match in hardware, used in SDL only
// (see interface.c read_butts() for what keyboard buttons return what in SDL mode)
#define BUTTON_RIGHT_A			(1<<0)
#define BUTTON_CENTER_A			(1<<1)
#define BUTTON_LEFT_A			(1<<2)
#define BUTTON_DOWN_A			(1<<3)
#define BUTTON_UP_A				(1<<4)
#define BUTTON_SHOOT_A			(1<<6)

#define BUTTON_RIGHT_B			(1<<7)
#define BUTTON_CENTER_B			(1<<8)
#define BUTTON_LEFT_B			(1<<9)
#define BUTTON_DOWN_B			(1<<10)
#define BUTTON_UP_B				(1<<11)
#define BUTTON_SHOOT_B			(1<<5)

// codes to send to the audio module to play/control sound effects
#define AUDIO_PLAYER_EXPLODE	0
#define AUDIO_PLAYER_SHOOT		6
#define AUDIO_ENEMY_EXPLODE		3
#define AUDIO_WAVESTART			(1<<4)
#define AUDIO_GAMEOVER			(1<<5)
#define AUDIO_MUSIC_LOW			14
#define AUDIO_MUSIC_HIGH		19

#define AUDCHAN1				(1<<0)
#define AUDCHAN2				(1<<1)
#define AUDCHAN3				(1<<2)
#define AUDCHAN4				(1<<3)
#define AUD_PAUSEOFFSET			8
#define AUD_RESUMEOFFSET		4
#define AUD_STOPOFFSET			0
#define AUD_CHANOFFSET			28
// music-low on chan1
// music-high on chan2
// enemy explode on chan3
// playershoot/playerexplode on chan4
// wave start/end on chan3+4

// This define is used in boot.S code
// how? where? i can't find it
#define BOARD_16_LEDS_ADDR		MFP_LEDS_ADDR

#endif
