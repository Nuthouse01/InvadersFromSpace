# Make_sound_mem.py

# Python file to make the .coe files (the block memory contents) for all
#       of the sound generator memories
# See the comments in .coe files for more details on formatting
# Jamie Williams - 2019

import numpy as np


# Makes the tone table block memory
# This takes "music notes" as address, and produces the period values for that tone
# See the comments in .coe files for more details
def make_tone_table(filename):
    sound_fx1 = [64, 60, 56, 52, 48, 44, 40, 36, 32, 28] #player explode
    sound_fx2 = [120, 112, 104, 96, 88, 80, 72, 64, 56, 48] #enemy explode
    sound_fx3 = [3000, 2850, 2700, 2550, 2400, 2250, 2100, 1950, 1800, 1650] # player shoot
    # In order:     [A, A#, B, C, C#, D, D#, E, F, F#, G, G#]
    notes_oct0 = [27.5, 29.14, 30.87, 16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96]
    notes_oct1 = [55, 58.27, 61.74, 32.7, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49, 51.91]
    notes_oct2 = [110, 116.54, 123.47, 65.41, 69.3, 73.42, 77.78, 82.41, 87.31, 92.50, 98, 103.83]
    notes_oct3 = [220, 233.08, 246.94, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185, 196, 207.65]
    notes_oct4 = [440, 446.16, 493.88, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392, 415.3]
    notes_oct5 = [880, 932.33, 987.77, 523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61]
    notes_oct6 = [1760, 1864.66, 1975.53, 1046.5, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22]
    notes_oct7 = [3520, 3729.31, 3951.07, 2093, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44]
    notes_oct8 = [7040, 7458.62, 7902.13, 4186.01, 4434.92, 4698.63, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88]

    file = open(filename, 'w')
    
    for y in range(0, 16):
        for x in range(0, 16):
            target_val = 0
            if y < 12:
                if x < 9:
                    #Legit note
                    if x == 0:
                        target_val = notes_oct0[y]
                    if x == 1:
                        target_val = notes_oct1[y]
                    if x == 2:
                        target_val = notes_oct2[y]
                    if x == 3:
                        target_val = notes_oct3[y]
                    if x == 4:
                        target_val = notes_oct4[y]
                    if x == 5:
                        target_val = notes_oct5[y]
                    if x == 6:
                        target_val = notes_oct6[y]
                    if x == 7:
                        target_val = notes_oct7[y]
                    if x == 8:
                        target_val = notes_oct8[y]

                else:
                    # Not a note but unused
                    target_val = 0
            else:
                #Not a note, use as sfx tone
                if y == 12: # fist digit of address is C
                    #load sfx1
                    if x < len(sound_fx1):
                        target_val = sound_fx1[x]
                if y == 13: # first digit of address is D
                    #load sfx2
                    if x < len(sound_fx2):
                        target_val = sound_fx2[x]
                if y == 14: # first digit of address is E
                    #load sfx3
                    if x < len(sound_fx3):
                        target_val = sound_fx3[x]
                # Tones at addresses with the first digit F are currently unused
            # Done loading target_val
            if target_val == 0:
                # PRINT TO FILE 00000000
                file.write("00")
            else:
                limit = hex(round((100*10**6)/target_val)).lstrip("0x").rstrip("L")
                # PRINT TO FILE, LIMIT
                file.write(limit)
            # PRINT COMMA AND SPACE
            file.write(", ")
        #End of y loop
        #PRINT ENTER
        file.write('\n')
    # End of x loop
    file.close()

# Low notes of background music
def make_song_low(filename):
    file = open(filename, 'w')
    # Notes in order: rest, D4, A3, A#3, A3, (rest?), A3, A#3, D4, rest, D4, A3, A#3, A3, (rest), A3, A#3, D4, rest
    # Note converstion: Rest-FF, D4-5_4, A3-0_3, A#3-1_3
    notes = ["ff", "54", "03", "13", "03", "ff", "03", "13", "54", "ff", "54", "03", "13", "03", "ff", "03", "13", "54", "ff", "54", "03", "13", "03", "ff", "03", "13", "54", "ff", "54", "03", "13", "03", "ff", "03", "13", "54", "ff"]
    duration = [2, 4,4,4,3,1,4,4,4, 4, 4,4,4,3,1,4,4,4, 4, 4,4,4,3,1,4,4,4, 4, 4,4,4,3,1,4,4,4, 2] #These numbers were used when 1/4th of second
    #print(sum(duration)/2)
    for x in range(0, len(duration)):
        for y in range(0, (duration[x]*4)):
            value = notes[x]
            #PRINT TO FILE VALUE AND COMMA AND SPACE
            file.write(value)
            file.write(", ")
        # End for y
        # PRINT TO FILE ENTER
        file.write('\n')
    # End for x
    file.close()

# High notes of background music
def make_song_high(filename):
    file = open(filename, 'w')
    # Notes in order: rest, a4, d5, a4, d5, c5, a4, c5, rest
    #                       a4, d5, a4, d5, c5, a4, d5, rest
    #                       a4, d5, a4, d5, c5, a4, c5, rest
    #                       a4, d5, a4, d5, a4, c5, d5
    # Note converstion: Rest-FF, A4-04, D5-5_5, C5-3_5
    notes_pt1 = ["ff", "04", "55", "04", "55", "35", "04", "35", "ff", "04", "55", "04", "55", "35", "04", "55", "ff"  ]
    notes_pt2 = ["04", "55", "04", "55", "35", "04", "35", "ff", "04", "55", "04", "55", "04", "35", "55"]
    notes = notes_pt1 + notes_pt2
    duration = [66, 2,2,2,2,2,2,2, 2, 2,2,2,2,2,2,2, 2, 2,2,2,2,2,2,2, 2, 2,2,2,2,2,2,2 ] #These numbers were used when 1/4th of second
    print(sum(duration)*4)

    for x in range(0, len(duration)):
        large_count = 0
        for y in range(0, (duration[x]*4)):
            value = notes[x]
            #PRINT TO FILE VALUE AND COMMA AND SPACE
            file.write(value)
            file.write(", ")
            if large_count == 15:
                file.write('\n')
                large_count = 0
            else:
                large_count = large_count +1
        # End for y
        # PRINT TO FILE ENTER
        file.write('\n')
    # End for x
    file.close()

# Sound effects for the player
def make_sfx_player(filename):
    file = open(filename, 'w')
    notes_1 = ["c0","c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf"]
    notes_3 = ["e0","e1", "e2", "e3", "e4", "e5", "e6", "ce", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef"]
    notes_buff = ["ff"]
    notes = notes_1 +  notes_3 +notes_buff# Pad middle rest to get good duration
    duration = [6,6,6,6, 6,6,6,6, 6,6,6,6, 6,6,6,6,  2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,2, 128] #in number of 100ths of a second
    print(sum(duration)) #128 long!! (nice!)
    for x in range(0, len(duration)):
        large_count = 0
        for y in range(0, duration[x]):
            value = notes[x]
            file.write(value)
            file.write(", ")
            if large_count == 15:
                file.write('\n')
                large_count = 0
            else:
                large_count = large_count +1
            #PRINT TO FILE VALUE AND COMMA AND SPACE
        # End for y
        # PRINT TO FILE ENTER
        file.write('\n')
    # End for x
    file.close()

# Sound effects for the enemy
def make_sfx_enemy(filename):
    file = open(filename, 'w')
    notes_d = ["d0","d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df"]
    notes_f = ["f0","f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"] # Not currently used, but doing anyway
    notes_buff = ["ff"]
    notes = notes_d + notes_buff + notes_f + notes_buff
    duration = [3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3, 16, 4,4,4,4, 4,4,4,4, 4,4,4,4, 4,4,4,4, 128] #in number of 100ths of a second
    print(sum(duration))
    for x in range(0, len(duration)):
        large_count = 0
        for y in range(0, duration[x]):
            value = notes[x]
            file.write(value)
            file.write(", ")
            if large_count == 15:
                file.write('\n')
                large_count = 0
            else:
                large_count = large_count +1
            #PRINT TO FILE VALUE AND COMMA AND SPACE
        # End for y
        # PRINT TO FILE ENTER
        file.write('\n')
    # End for x
    file.close()



#Start of main code

filename1 = "test_file.txt"

# Change this number to create the different .coe files
run_type = 0

if run_type == 1:
    make_tone_table(filename1)
if run_type == 2:
    make_song_low(filename1)
if run_type == 3:
    make_song_high(filename1)
if run_type == 4:
    make_sfx_player(filename1)
if run_type == 5:
    make_sfx_enemy(filename1)





