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
#ifndef INTERFACE_H
#define INTERFACE_H

#include "params.h"

int init();
void quit();
void render_main();

void write_timer0(long new_now);
long read_timer0();
void write_timer1(long new_now);
long read_timer1();

void seed(int seed);
int  get_random();
long read_butts();
long read_switches();
int  read_leds();
void write_leds(int);
void write_7seg_enable(int);
void write_7seg_dec(int);
void write_7seg_high(long);
void write_7seg_low(long);
int  read_xadc_a();
int  read_xadc_b();
void write_audio_soundfx(long);
void write_audio_musicstate(long);
void write_audio_currwave(long);

void write_sprite_xpos(int spriteidx, int xpos);
void write_sprite_ypos(int spriteidx, int ypos);
void write_sprite_idx(int spriteidx, enum palette_index idx);
//void write_sprite_scale(int spriteidx, int scale);
int read_sprite_xpos(int spriteidx);
int read_sprite_ypos(int spriteidx);
enum palette_index read_sprite_idx(int spriteidx);
//int read_sprite_scale(int spriteidx);

void draw();

int print(const char *format, ...);

#endif
