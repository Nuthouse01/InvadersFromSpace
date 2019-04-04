SoundProject

-Jamie Williams	2019

A test project to work with sound generation on the Nexys A7

END GOAL - interface
	Far left switch - on/off audio enable
	Other switches for controling sound volume (duty value) of each pwm generator
	No particular software needed
		-Except perhaps to change the LEDs or 7seg if need visuals

Done:
	- PWM generator module
		pwm_generator.v 
			-Generates PWM signal given a period value
			-Period value = clock frequency/target frequency
					
	- Sound generator module
		sound_generator.v
			-Takes in clocks, switches, outputs final pwm values
			-Instantiates memories storing all music/sfx values
			-Creates clock divider/counters to determine tones to play
	
	- Top level board verilog file that connects to sound generator module

Not finished:
	- Create version of sound files that integrate into the main project
		Current version breaks the main game (block memory issue?)
		
	- Finish mfp_sound_gen_decoder.sv
		Currently only generates music enable
		Needs logic to generate signals when sound effects need to be played
			See verilog header files for correct memory locations and values
		
	- Create logic to play sound effects when game action occurs
		Need to create more inputs to trigger sound effects
		
		