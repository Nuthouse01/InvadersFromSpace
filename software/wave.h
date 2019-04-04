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
#ifndef SPACEINVADER_WAVE
#define SPACEINVADER_WAVE

#include "params.h"

enum bullet_record_enum {BULLET_OFF=0, BULLET_ENEMY=1, BULLET_PLAYER=2};
enum player_status_enum {PLAYER_ALIVE=0, PLAYER_EXPLODING_A=1, PLAYER_EXPLODING_B=2, PLAYER_DEAD=3};
enum enemy_record_enum  {ENEMY_ALIVE=0, ENEMY_EXPLODING_A=1, ENEMY_EXPLODING_B=2, ENEMY_DEAD=3};
enum bunker_record_enum {BUNKER_DEAD=0,
						BUNKER_FULL_A=1, BUNKER_FULL_B=2, BUNKER_FULL_C=3,
						BUNKER_LOUT_A=4, BUNKER_LOUT_B=5, BUNKER_ROUT_A=6, BUNKER_ROUT_B=7};
enum enemy_movement_enum {MOVING_DOWN, MOVING_LEFT, MOVING_RIGHT};

int run_one_wave(int, int);

int new_target(int);
int time_to_run_target(int);
int update_gametime();

void calc_and_draw_score(long);
int  check_pausequit();
int  create_bullet(int, int, int);
int  collision_ebullet(int);
int  collision_pbullet();
int  collision_bunker();
void hit_players(int);
void hit_enemy(int, int);
void hit_bunker(int, int, int, enum bullet_record_enum whobrokeit);
void shoot_enemy();
void shoot_players(int);
int  move_bullets();
void move_players(int);
int  move_enemies(int);
int  animate_explosion_enemy(int, int, int);
int  animate_explosion_player(int);


#endif
