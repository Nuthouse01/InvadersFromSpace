import math
# brian's script to make the COE files for brian's sound modules
#
#
# period and duration are stored in the same COE file, stapled together
# duration is bottom (LSB)
# size of each part is flexible
# duration scale is flexible
# number of FX is flexible (ID)
# size of FX is flexible (depth)
# 
# jump code: duration=0, period=ID of sfx to play next
# stop code: duration=0, period > any possible sfx ID


# human hearing is 20Hz - 20KHz, ==> 10Hz - 40KHz

################################################################################################################################
# user-changeable parameters

OUT_FILENAME = "_testfile4.coe"

USE_HEX = 1		# bin=0, hex=1
VERBOSE = 1		# print extra info to terminal or not
DUMP_RESULT = 0	# print a copy of the linelist (with space separation)

DURCLK_FREQ = 100		#100Hz
DUR_BITWIDTH = 8
# therefore greatest length of time on a single note is ~2.5s
AUDCLK_FREQ =	5000000	#5mhz
#AUDCLK_FREQ =	1000000	#1mhz
#AUDCLK_FREQ =	 500000	#500khz
#AUDCLK_FREQ =	 100000	#100khz
PER_BITWIDTH = 20
# at 1MHz, 2^20 ==> 2Hz minimum
WIDTH_IN_BIN = DUR_BITWIDTH + PER_BITWIDTH
WIDTH_IN_HEX = math.ceil((DUR_BITWIDTH + PER_BITWIDTH)/4)

SFXID_BITWIDTH = 5			# number of bits for sound effect IDs
SFXDEPTH_BITWIDTH = 2		# number of bits for tones in a sound effect


################################################################################################################################
# sound FX definition section


sound_fx1 = [64, 60, 56, 52, 48, 44, 40, 36, 32, 28] #player explode
fx1_dur = 0.06
sound_fx2 = [120, 112, 104, 96, 88, 80, 72, 64, 56, 48] #enemy explode
fx2_dur = 0.03
sound_fx3 = [3000, 2850, 2700, 2550, 2400, 2250, 2100, 1950, 1800, 1650] #player shoot
fx3_dur = 0.02

# define music notes via frequency here
# octaves wrap A-Gs
#Order:[A,		A#,		B,		C,		C#,		D,		D#,		E,		F,		F#,		G,		G#]
n =[[	27.50,	29.14,	30.87,	32.70,	34.65,	36.71,	38.89,	41.20,	43.65,	46.25,	49.00,	51.91],	#oct0
	[	55.00,	58.27,	61.74,	65.41,	69.30,	73.42,	77.78,	82.41,	87.31,	92.50,	98.00,	103.83], #oct1
	[	110.00,	116.54,	123.47,	130.81,	138.59,	146.83,	155.56,	164.81,	174.61,	185.00,	196.00,	207.65], #oct2
	[	220.00,	233.08,	246.94,	261.63,	277.18,	293.66,	311.13,	329.63,	349.23,	369.99,	392.00,	415.30], #oct3
	[	440.00,	446.16,	493.88,	523.25,	554.37,	587.33,	622.25,	659.25,	698.46,	739.99,	783.99,	830.61], #oct4
	[	880.00,	932.33,	987.77,	1046.50,1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22], #oct5
	[	1760.00,1864.66,1975.53,2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44], #oct6
	[	3520.00,3729.31,3951.07,4186.01,4434.92,4698.63,4978.03,5274.04,5587.65,5919.91,6271.93,6644.88], #oct7
	[	7040.00,7458.62,7902.13]] #oct8
A,As,Bb,B,C,Cs,Db,D,Ds,Eb,E,F,Fs,Gb,G,Gs,Ab = (0,1,1,2,3,3,4,5,6,6,7,8,8,9,10,10,11)
REST = AUDCLK_FREQ	# to make a rest, play tone above human hearing, period of 1
# usage: n[octave0-8][noteletter]
# Cs = C# = C-sharp
# Db = D-flat
# note that octave2 is probably the lowest you should use, extremely hard to hear sounds below that when our sound generator is so shitty



################################################################################################################################
# dont change this
WIDTH_IN_BIN = DUR_BITWIDTH + PER_BITWIDTH
WIDTH_IN_HEX = math.ceil((DUR_BITWIDTH + PER_BITWIDTH)/4)
SFX_COUNTER_HUMAN = 0
WIDEST_PERIOD = 0

################################################################################################################################
# functions

def numtoline(num):
	global WIDEST_PERIOD
	if(num.bit_length() > WIDEST_PERIOD):
		WIDEST_PERIOD = num.bit_length()
	if(USE_HEX):
		line = hex(num).lstrip("0x").zfill(WIDTH_IN_HEX)
		return line
	else:
		line = bin(num).lstrip("0b").zfill(WIDTH_IN_BIN)
		return line

# function: take freq, duration(seconds) & return the line in hex (as a string)
def coeline(freq, dur):
	period = int(AUDCLK_FREQ / freq)
	duration = int(DURCLK_FREQ * dur)
	if (period < 32):
		print(f"WARNING: freq={freq} causes per={period}, might sound bad")
	if (period >= 2**PER_BITWIDTH):
		print(f"ERROR: freq={freq} at clk={AUDCLK_FREQ} causes per={period}, too great for given period bitwidth!!")
	if (duration >= 2**DUR_BITWIDTH):
		print(f"ERROR: dur={dur} causes duration={duration}, too great for given duration bitwidth!!")
	combine = (period * (2**DUR_BITWIDTH)) + duration
	result = numtoline(combine)
	return result
	
# make a coe line that will jump to a target sound effect
def jumpcode(target):
	if (target >= 2**SFXID_BITWIDTH):
		print(f"ERROR: target={target} is greater than the number of possible SFX")
	# duration=0, period=target
	combine = target * (2**DUR_BITWIDTH)
	result = numtoline(combine)
	return result
	
# make a coe line that will stop the sound effect channel
def stopcode():
	# duration=0, period=maximum
	combine = ((2**PER_BITWIDTH)-1) * (2**DUR_BITWIDTH)
	if(USE_HEX):
		line = hex(combine).lstrip("0x").zfill(WIDTH_IN_HEX)
		return line
	else:
		line = bin(combine).lstrip("0b").zfill(WIDTH_IN_BIN)
		return line

# fills the rest of the SFX with zeros until taking up full SFX slot(s)
# takes a list of the COE lines so far (should have already added jump/stop code)
# returns it with zeroes added to the end, and # of SFX ID slots it fills
def endpadsfx(sofar):
	global SFX_COUNTER_HUMAN
	SFX_COUNTER_HUMAN += 1
	begin = len(sofar)
	# until its length is a whole# multiple of 2^depth,
	while((len(sofar) % (2**SFXDEPTH_BITWIDTH))!=0):
		sofar.append(numtoline(0))
		pass
	if(VERBOSE):
		added = len(sofar) - begin
		print(f"        added {added} lines to sfx #{SFX_COUNTER_HUMAN} to finish it")
	next = int(len(sofar) / (2**SFXDEPTH_BITWIDTH))
	if(SFX_COUNTER_HUMAN == 1):
		print(f"    sfx #1 begins at 0/{(2**SFXID_BITWIDTH)-1}")
	
	if(next == (2**SFXID_BITWIDTH)):
		print(f"    sfx memory perfectly filled with {SFX_COUNTER_HUMAN} sound effects!!")
	else:
		print(f"    sfx #{SFX_COUNTER_HUMAN+1} begins at {next}/{(2**SFXID_BITWIDTH)-1}")	
	return sofar

# given a list of COE lines, write them to a properly formatted COE file
def writetofile(linelist):
	# first pad the linelist to be the full length of the memory block
	begin = len(linelist)
	if(len(linelist) > (2**SFXDEPTH_BITWIDTH)*(2**SFXID_BITWIDTH)):
		print(f"ERROR: length={len(linelist)} is too great for ids=2^{SFXID_BITWIDTH} and depth=2^{SFXDEPTH_BITWIDTH}")
	while(len(linelist) < (2**SFXDEPTH_BITWIDTH)*(2**SFXID_BITWIDTH)):
		linelist.append(numtoline(0))
		pass
		
	if(VERBOSE):
		added = len(linelist) - begin
		print(f"        added {added} lines to file to finish it")
		print(f"        resulting size = {len(linelist)} = {(2**SFXDEPTH_BITWIDTH)*(2**SFXID_BITWIDTH)}")
	
	# now acutally open & start writing to the file
	name = OUT_FILENAME
	try:
		file = open(name, 'w')
	except IOError: 
		print(f"ERROR: could not open file='{name}'")
		return
	file.write("; generated sound memory for brian's sound module\n")
	file.write(";\n")
	file.write(";\n") # TODO: more header comments?
	file.write(";\n")
	file.write('\n')
	if(USE_HEX):
		file.write("memory_initialization_radix=16;\n")
	else:
		file.write("memory_initialization_radix=2;\n")
	file.write("memory_initialization_vector=\n")
	file.write('\n')
	

	if(DUMP_RESULT):
		# now print the actual linelist
		print("printing the COE file guts now, with space separation")
		if(USE_HEX and ((DUR_BITWIDTH % 4) != 0)):
			print(f"WARNING: with hex and durwidth={DUR_BITWIDTH} not multiple of 4, cannot perfectly separate duraton from period")
			print( "         note that this only effects how it is printed in terminal, it is written to file without issue")
		
	i = 0 # which line i'm on
	j = 0 # track where i am within a SFX
	while(i < len(linelist)):
		if(j == (2**SFXDEPTH_BITWIDTH)):
			file.write("\n")
			if(DUMP_RESULT):
				print("")
			j = 0
		file.write(linelist[i])
		if(DUMP_RESULT):
			# try to apply fancy space-separation
			if(USE_HEX):
				line = linelist[i][0:WIDTH_IN_HEX - math.ceil(DUR_BITWIDTH/4)] + ' ' + linelist[i][WIDTH_IN_HEX - math.ceil(DUR_BITWIDTH/4):WIDTH_IN_HEX]
			else:
				line = linelist[i][0:PER_BITWIDTH] + ' ' + linelist[i][PER_BITWIDTH:WIDTH_IN_BIN]
			print(line)
		
		if(i != len(linelist)-1):
			file.write(",\n")
		else:
			file.write(";\n")
		
		j += 1
		i += 1
		pass
	
	file.close()
	print(f"    '{name}' successfully written")
	if((WIDEST_PERIOD-DUR_BITWIDTH) < PER_BITWIDTH):
		print(f"    PER_BITWIDTH={PER_BITWIDTH} but widest period={WIDEST_PERIOD-DUR_BITWIDTH}, suggest reducing period width to match")
	return

################################################################################################################################
# end of function definitions
# beginning of using them to make the COE file


linelist = []


print("sfx1 begin: player explode")
for f in sound_fx1:
	linelist.append(coeline(f, fx1_dur))
	pass
linelist.append(stopcode())
linelist = endpadsfx(linelist)


print("sfx2 begin: enemy explode")
for f in sound_fx2:
	linelist.append(coeline(f, fx2_dur))
	pass
linelist.append(stopcode())
linelist = endpadsfx(linelist)


print("sfx3 begin: player shoot")
for f in sound_fx3:
	linelist.append(coeline(f, fx3_dur))
	pass
linelist.append(stopcode())
linelist = endpadsfx(linelist)


print("sfx4 begin: looptest1")
linelist.append(coeline(n[4][A],1))
linelist.append(jumpcode(9))
linelist = endpadsfx(linelist)

print("sfx5 begin: looptest2")
linelist.append(coeline(n[4][D],1))
linelist.append(jumpcode(10))
linelist = endpadsfx(linelist)

print("sfx6 begin: looping scale")
linelist.append(coeline(n[5][C],0.5))
linelist.append(coeline(n[5][D],0.5))
linelist.append(coeline(n[5][E],0.5))
linelist.append(coeline(n[5][F],0.5))
linelist.append(coeline(n[5][G],0.5))
linelist.append(coeline(n[6][A],0.5))
linelist.append(coeline(n[6][B],0.5))
linelist.append(coeline(n[6][C],0.5))
linelist.append(jumpcode(11))
linelist = endpadsfx(linelist)


# song_low
# Notes in order: rest, D4, A3, A#3, A3, (rest), A3, A#3, D4, rest, D4, A3, A#3, A3, (rest), A3, A#3, D4, rest
notes1 = [	n[3][D], n[3][A], n[3][As], n[3][A], REST, 
			n[3][A], n[3][As], n[3][D], REST, 
			n[3][D], n[3][A], n[3][As], n[3][A], REST, 
			n[3][A], n[3][As], n[3][D], REST ]
duration1= [4,4,4,3, 1,
			4,4,4, 4, 
			4,4,4,3, 1,
			4,4,4, 4 ] #These numbers were used when 1/4th of second

# song_high
# Notes in order: rest, a4, d5, a4, d5, c5, a4, c5, rest
#                       a4, d5, a4, d5, c5, a4, d5, rest
#                       a4, d5, a4, d5, c5, a4, c5, rest
#                       a4, d5, a4, d5, a4, c5, d5
notes2= [	n[5][A], n[5][D], n[5][A], n[5][D], n[5][C], n[5][A], n[5][C], REST, 
			n[5][A], n[5][D], n[5][A], n[5][D], n[5][C], n[5][A], n[5][D], REST,
			n[5][A], n[5][D], n[5][A], n[5][D], n[5][C], n[5][A], n[5][C], REST, 
			n[5][A], n[5][D], n[5][A], n[5][D], n[5][A], n[5][C], n[5][D], REST ]
duration2= [2,2,2,2,2,2,2, 2, 
			2,2,2,2,2,2,2, 2, 
			2,2,2,2,2,2,2, 2, 
			2,2,2,2,2,2,2, 2 ] #These numbers were used when 1/4th of second
song_high = zip(notes2, duration2)
# print(sum(duration2)/4)
# print(len(duration2))
# print(len(notes2))

print("sfx7 begin: song_low")
for note,dur in zip(notes1, duration1):
	linelist.append(coeline(note,dur/4))
linelist.append(jumpcode(14))
linelist = endpadsfx(linelist)

print("sfx8 begin: song_high")
for note,dur in zip(notes2, duration2):
	linelist.append(coeline(note,dur/4))
linelist.append(jumpcode(19))
linelist = endpadsfx(linelist)









# finish it off & save the result
writetofile(linelist)



####################################
# scratch area

# print("220hz")
# print(coeline(220,1))
# print("440hz")
# print(coeline(440,1))
# print("880hz")
# print(coeline(880,1))


print("...press ENTER to exit...")
input()


#old
# #Order:[	A,		A#,		B,		C,		C#,		D,		D#,		E,		F,		F#,		G,		G#]
# oct0=[	27.5, 	29.14, 	30.87, 	16.35, 	17.32, 	18.35, 	19.45, 	20.60, 	21.83, 	23.12, 	24.50, 25.96]
# oct1=[	55.00,	58.27,	61.74,	32.70,	34.65,	36.71,	38.89,	41.20,	43.65,	46.25,	49.00,	51.91]
# oct2=[	110.00,	116.54,	123.47,	65.41,	69.30,73.42,77.78,82.41,87.31,92.50,98,103.83]
# oct3=[	220.00,	233.08,	246.94,	130.81,	138.59,146.83,155.56,164.81,174.61,185,196,207.65]
# oct4=[	440.00,	446.16,	493.88,	261.63,	277.18,293.66,311.13,329.63,349.23,369.99,392,415.3]
# oct5=[	880.00,	932.33,	987.77,	523.25,	554.37,587.33,622.25,659.25,698.46,739.99,783.99,830.61]
# oct6=[	1760.00,1864.66,1975.53,1046.50,1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22]
# oct7=[	3520.00,3729.31,3951.07,2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44]
# oct8=[	7040.00,7458.62,7902.13,4186.01,4434.92,4698.63,4978.03,5274.04,5587.65,5919.91,6271.93,6644.88]

#new
# each octave is C-B
# this is how the tone-generator site arranged the notes
#Order:[C,		C#,		D,		D#,		E,		F,		F#,		G,		G#,		A,		A#,		B]
# n =[[	16.35,	17.32,	18.35,	19.45,	20.60,	21.83,	23.12,	24.50,	25.96,	27.50,	29.14,	30.87], #oct0
	# [	32.70,	34.65,	36.71,	38.89,	41.20,	43.65,	46.25,	49.00,	51.91,	55.00,	58.27,	61.74], #oct1
	# [	65.41,	69.30,	73.42,	77.78,	82.41,	87.31,	92.50,	98.00,	103.83,	110.00,	116.54,	123.47], #oct2
	# [	130.81,	138.59,	146.83,	155.56,	164.81,	174.61,	185.00,	196.00,	207.65,	220.00,	233.08,	246.94], #oct3
	# [	261.63,	277.18,	293.66,	311.13,	329.63,	349.23,	369.99,	392.00,	415.30,	440.00,	446.16,	493.88], #oct4
	# [	523.25,	554.37,	587.33,	622.25,	659.25,	698.46,	739.99,	783.99,	830.61,	880.00,	932.33,	987.77], #oct5
	# [	1046.50,1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,1760.00,1864.66,1975.53], #oct6
	# [	2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44,3520.00,3729.31,3951.07], #oct7
	# [	4186.01,4434.92,4698.63,4978.03,5274.04,5587.65,5919.91,6271.93,6644.88,7040.00,7458.62,7902.13]] #oct8
# C,Cs,Db,D,Ds,Eb,E,F,Fs,Gb,G,Gs,Ab,A,As,Bb,B = (0,1,1,2,3,3,4,5,6,6,7,8,8,9,10,10,11)


#newest
# octaves wrap A-Gs
# this makes the most sense to me
#Order:[A,		A#,		B,		C,		C#,		D,		D#,		E,		F,		F#,		G,		G#]
# n =[[	27.50,	29.14,	30.87,	32.70,	34.65,	36.71,	38.89,	41.20,	43.65,	46.25,	49.00,	51.91],	#oct0
	# [	55.00,	58.27,	61.74,	65.41,	69.30,	73.42,	77.78,	82.41,	87.31,	92.50,	98.00,	103.83], #oct1
	# [	110.00,	116.54,	123.47,	130.81,	138.59,	146.83,	155.56,	164.81,	174.61,	185.00,	196.00,	207.65], #oct2
	# [	220.00,	233.08,	246.94,	261.63,	277.18,	293.66,	311.13,	329.63,	349.23,	369.99,	392.00,	415.30], #oct3
	# [	440.00,	446.16,	493.88,	523.25,	554.37,	587.33,	622.25,	659.25,	698.46,	739.99,	783.99,	830.61], #oct4
	# [	880.00,	932.33,	987.77,	1046.50,1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22], #oct5
	# [	1760.00,1864.66,1975.53,2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44], #oct6
	# [	3520.00,3729.31,3951.07,4186.01,4434.92,4698.63,4978.03,5274.04,5587.65,5919.91,6271.93,6644.88], #oct7
	# [	7040.00,7458.62,7902.13]] #oct8
# A,As,Bb,B,C,Cs,Db,D,Ds,Eb,E,F,Fs,Gb,G,Gs,Ab = (0,1,1,2,3,3,4,5,6,6,7,8,8,9,10,10,11)



# # song_low
# # Notes in order: rest, D4, A3, A#3, A3, (rest?), A3, A#3, D4, rest, D4, A3, A#3, A3, (rest), A3, A#3, D4, rest
# # Note converstion: Rest-FF, D4-5_4, A3-0_3, A#3-1_3
# notes = [
		# "54", "03", "13", "03", "ff", 
		# "03", "13", "54", "ff", 
		# "54", "03", "13", "03", "ff", 
		# "03", "13", "54", "ff", 
		# "54", "03", "13", "03", "ff", 
		# "03", "13", "54", "ff", 
		# "54", "03", "13", "03", "ff", 
		# "03", "13", "54", "ff"]
# duration = [
		# 4,4,4,3, 1,
		# 4,4,4, 4, 
		# 4,4,4,3, 1,
		# 4,4,4, 4, 
		# 4,4,4,3, 1,
		# 4,4,4, 4, 
		# 4,4,4,3, 1,
		# 4,4,4, 4] #These numbers were used when 1/4th of second
# print(sum(duration)/4)

# # song_high
# # Notes in order: rest, a4, d5, a4, d5, c5, a4, c5, rest
# #                       a4, d5, a4, d5, c5, a4, d5, rest
# #                       a4, d5, a4, d5, c5, a4, c5, rest
# #                       a4, d5, a4, d5, a4, c5, d5
# # Note converstion: Rest-FF, A4-04, D5-55, C5-35
# notes = [
		# "04", "55", "04", "55", "35", "04", "35", "ff", 
		# "04", "55", "04", "55", "35", "04", "55", "ff",
		# "04", "55", "04", "55", "35", "04", "35", "ff", 
		# "04", "55", "04", "55", "04", "35", "55", "ff"]
# duration = [
		# 2,2,2,2,2,2,2, 2, 
		# 2,2,2,2,2,2,2, 2, 
		# 2,2,2,2,2,2,2, 2, 
		# 2,2,2,2,2,2,2, 2 ] #These numbers were used when 1/4th of second
# print(sum(duration)/4)


