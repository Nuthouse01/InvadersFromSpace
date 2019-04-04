// sound_generator.v
// Jamie Williams 2019
// 
//
// Creates a PWM signal to connect to the Nexys A7 audio jack
// 	Audio jack needs 2 1-bit signals: pwm_signal, and audio_enable

module sound_generator(
		input 			clock_in100MHz,		// Input clock 100MHz (only for pwm generators)
		input			clock_in25MHz,		// Input clock 25MHz (system clock)
		input	 	    music_en,			// To control play/pause of music
		input [15:0]	switches,			// For testing, using switches to control audio enable, duty values
		output reg    	PWM_out,			// PWM output signal
		output reg		AUDIO_en			// Audio enable
);
	localparam period_WIDTH = 32; // Bit witdth of period value
	
	// *************************************
	// **** Initialize Block Memories ******
	// *************************************
	
	// Tones tables: Used to translate "music note" values to period value 
	//		See the tone table .coe file for more details on format
	blk_mem_tones tone_table_0 (period_val_0, tone_val_0, clock_in25MHz);
	blk_mem_tones tone_table_1 (period_val_1, tone_val_1, clock_in25MHz);
	blk_mem_tones tone_table_2 (period_val_2, tone_val_2, clock_in25MHz);
	blk_mem_tones tone_table_3 (period_val_3, tone_val_3, clock_in25MHz);
	
	// Block memories for the background sounds
	//	Store "music note" values (see .coe files for more details)
	//
	// 		Both memories are 8 bits wide, 512 deep (8 bit data, 9 bit address)
	//		The blocks use the initialization files: 
	//			space_theme_high.coe and space_theme_low.coe
	blk_mem_song_low song_low (tone_val_0, sec_sixteenth, clock_in25MHz);
	blk_mem_song_high song_high (tone_val_1, sec_sixteenth, clock_in25MHz);
	
	// Block memories for the sound effects
	//	Store "music note" values (see .coe files for more details)
	//
	// 		Both memories are 8 bits wide, 256 deep (8 bit data, 8 bit address)
	//		The blocks use the initialization files: 
	//			space_theme_high.coe and space_theme_low.coe
	blk_mem_player_sfx player_sfx (tone_val_2, sec_hundredth_0, clock_in25MHz);
	blk_mem_enemy_sfx enemy_sfx (tone_val_3, sec_hundredth_1, clock_in25MHz);
	
	// ******************************************
	// **** Create Clock Dividers/Counters ******
	// ******************************************
	
	// Clock divider code to lower clock frequency 
	// This uses the 25MHz clock 
	reg [22:0] clock_divider_0 = 0;	//To power 16th sec counter (for music)
	reg [22:0] clock_divider_1 = 0; //To power 1/100th sec counter (for sfx)
	reg [22:0] clock_divider_2 = 0; //To power 1/100th sec counter 2 (for sfx)
	
	reg [6:0] sec_hundredth_0 = 0;
	reg [6:0] sec_hundredth_1 = 0;
	reg [8:0] sec_sixteenth = 0;	// 512 sixteenth seconds, 32 seconds worth of music
	
	// Designed for 25MHz clock
	// For a different clock, can do clock_freq/#_times_a_sec_to_increment to get values
	always @(posedge clock_in25MHz)
	begin
		// Only increment music clock divider on music enable
	   if(music_en)
			clock_divider_0 <= clock_divider_0 + 1;
	   clock_divider_1 <= clock_divider_1 + 1;
	   clock_divider_2 <= clock_divider_2 + 1;
	   
	   // Counter for the music (determines address of current music note)
	   if((clock_divider_0 > 23'd1_562_500)) // Should increment every 1/16th of a second
	   begin
	       clock_divider_0 <= 0;       
	       sec_sixteenth <= sec_sixteenth + 1;
	   end
	   
	   // Counter for the player sfx (determines address of current music note)
	   if((clock_divider_1 > 23'd250_000)) // Should increment every 1/100th of a second
	   begin
	       clock_divider_1 <= 0;       
	       sec_hundredth_0 <= sec_hundredth_0 + 1;
	   end
	   
	   // Counter for the enemy sfx (determines address of current music note)
	   if((clock_divider_2 > 23'd50_000)) // Should increment every 1/100th of a second
	   begin
	       clock_divider_2 <= 0;       
	       sec_hundredth_1 <= sec_hundredth_1 + 1;
	   end	   
	end
	
	
	
	
	
	
	// Duty cycle values, determine volume of each pwm generators
	// 		For testing, these are determined by switches, but can be hard coded for game
    reg [(period_WIDTH-1):0] duty_val_0 = 0;
	reg [(period_WIDTH-1):0] duty_val_1 = 0;
	reg [(period_WIDTH-1):0] duty_val_2 = 0;
	reg [(period_WIDTH-1):0] duty_val_3 = 0;

	// For testing, store switch values for duty shift values
	reg [15:0]	duty_shift = 0;

	// Use switches to define volume of pwm output
	always @(posedge clock_in25MHz)
	begin
		AUDIO_en <= 1'b1;
 
		// For testing, change duty value (volume) based on switches
		duty_shift <= switches[15:0];
		
		// To use less switches, concatonate constants to the front
		duty_val_0 <= (period_val_0 >> {2'b10,duty_shift[2:0]}); 
		duty_val_1 <= (period_val_1 >> {2'b10,duty_shift[5:3]});
		duty_val_2 <= (period_val_2 >> {2'b10,duty_shift[8:6]});
		duty_val_3 <= (period_val_3 >> {2'b10,duty_shift[11:9]});
	end

	always @ (*)
	begin
		// Take all pwm generator outputs and combine them to produce output
		// Music enable allows for pausing (disabling) of background music
       PWM_out <= ((pwm_out0 | pwm_out1) & music_en) | pwm_out2 | pwm_out3;
	end
	
	// PWM generators
	// First 2 generators are for music (low and high parts)
	// Second 2 generators are for sound effects (player and enemies)
	pwm_generator #(period_WIDTH) pwm_gen0 (.clock(clock_in100MHz), .period(period_val_0),  .duty_value(duty_val_0),  .PWM_out(pwm_out0));
	pwm_generator #(period_WIDTH) pwm_gen1 (.clock(clock_in100MHz), .period(period_val_1),  .duty_value(duty_val_1),  .PWM_out(pwm_out1));
	pwm_generator #(period_WIDTH) pwm_gen2 (.clock(clock_in100MHz), .period(period_val_2),  .duty_value(duty_val_2),  .PWM_out(pwm_out2));
	pwm_generator #(period_WIDTH) pwm_gen3 (.clock(clock_in100MHz), .period(period_val_3),  .duty_value(duty_val_3),  .PWM_out(pwm_out3));



endmodule