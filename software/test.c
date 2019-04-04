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

int main () {
    MFP_7SEG_EN	= 0xFF;
    MFP_7SEG_LDIG = 0x00000000;

    unsigned volatile int *sprite_ram = MFP_SPRITE_GRAPHICS;

    for (int j = 0; j < 1024; j++) sprite_ram[j] = 0;

    for (int j = 0; j < 32; j++) {
        //int pattern1 = 0x04000000;//0x01234567; // & ~(0xF <<j); 
        //int pattern1 = 0x01234567  & ~(0xF <<j); 
        //int pattern2 = 0x00000030;//0x89ABCDEF; // & ~(0xF <<j); 
        //int pattern2 = 0x89ABCDEF & ~(0xF <<j); 
        sprite_ram[(j*32)+0 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) & 0xFF000000;
        sprite_ram[(j*32)+2 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) & 0x0FF00000;
        sprite_ram[(j*32)+4 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) & 0x00FF0000;
        sprite_ram[(j*32)+6 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) & 0x000FF000;
        sprite_ram[(j*32)+8 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) & 0x0000FF00;
        sprite_ram[(j*32)+10] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) & 0x00000FF0;
        sprite_ram[(j*32)+12] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x000000FF;
        sprite_ram[(j*32)+14] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x000000FF;
        sprite_ram[(j*32)+16] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x000000FF;
        sprite_ram[(j*32)+18] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x000000FF;
        sprite_ram[(j*32)+20] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x00000FF0;
        sprite_ram[(j*32)+22] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x0000FF00;
        sprite_ram[(j*32)+24] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x000FF000;
        sprite_ram[(j*32)+26] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x00FF0000;
        sprite_ram[(j*32)+28] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x0FF00000;
        sprite_ram[(j*32)+30] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0xFF000000;

        sprite_ram[(j*32)+1 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x000000FF;
        sprite_ram[(j*32)+3 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x00000FF0;
        sprite_ram[(j*32)+5 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x0000FF00;
        sprite_ram[(j*32)+7 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x000FF000;
        sprite_ram[(j*32)+9 ] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x00FF0000;
        sprite_ram[(j*32)+11] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x0FF00000;
        sprite_ram[(j*32)+13] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0xFF000000;
        sprite_ram[(j*32)+15] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0xFF000000;
        sprite_ram[(j*32)+17] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0xFF000000;
        sprite_ram[(j*32)+19] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0xFF000000;
        sprite_ram[(j*32)+21] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x0FF00000;
        sprite_ram[(j*32)+23] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x00FF0000;
        sprite_ram[(j*32)+25] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x000FF000;
        sprite_ram[(j*32)+27] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x0000FF00;
        sprite_ram[(j*32)+29] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x00000FF0;
        sprite_ram[(j*32)+31] = ((j<<28) | (j<<24) | (j<<20) | (j<<16) | (j<<12) | (j<<8) | (j<<4) | j ) &0x000000FF;
     }

    unsigned volatile int *sprite_table = MFP_SPRITE_TABLE;

    for (int i = 0; i < (128*4); i++) {
      sprite_table[i] = 0;
    }

    for (int j = 0; j < 16; j++) {
      for (int i = 0; i < 8; i++) {
         sprite_table[(j<<5)+(i<<2)+0] = 600 - (i * 32);
         sprite_table[(j<<5)+(i<<2)+1] = 300 - (j * 16);
         sprite_table[(j<<5)+(i<<2)+2] = i; 
      }
    }


    while (1) {
      int x = MFP_XADC_RESULTA;
      int y = MFP_XADC_RESULTB;
      //MFP_7SEG_LDIG = (((x & 0xF00) >> 8) << 16) | (((x & 0xF0) >> 4) << 8) | (x & 0xF);
      //MFP_7SEG_HDIG = (((x & 0xF00) >> 8) << 16) | (((x & 0xF0) >> 4) << 8) | (x & 0xF);

      int buttons = MFP_BUTTONS;			

      for (int j = 0; j < 16; j++) {
        for (int i = 0; i < 8; i++) {
          sprite_table[(j<<5)+(i<<2)+0] = (((x - 1600) / 3) & 0xFFF0) + (i * 29);
          sprite_table[(j<<5)+(i<<2)+1] = (((y - 1650) / 4) & 0xFFF0) + (j * 23);
          if (buttons & 0x40) {
            sprite_table[(j<<5)+(i<<2)+2]++;
          }
          if (buttons & 0x20) {
            sprite_table[(j<<5)+(i<<2)+2]--;
          }
        }
      }
    }
}
