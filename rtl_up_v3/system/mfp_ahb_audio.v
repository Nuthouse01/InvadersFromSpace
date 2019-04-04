// mfp_ahb_audio.v
//





`include "mfp_ahb_const.vh"

module mfp_ahb_audio(
    input                        HCLK, //25MHz clock
    input                        HRESETn,
    input      [  3          :0] HADDR,
    input      [  1          :0] HTRANS,
    input      [ 31          :0] HWDATA,
    input                        HWRITE,
    input                        HSEL,
    output reg [ 31          :0] HRDATA = 32'b0,
	// audio output
	output reg [1:0]			OUT_audio_pwm_and_enable,// [0]=AUD_PWM, [1]=AUD_SD
	input						IN_audio_clock //100MHz clock
);
// audio is only 1-bit resolution, but stereo
// control volume via pulse-width modulation
// control tone via frequency

//`H_SOUND_SOUNDFX_IONUM
// write 1<<1 = begin player explode sound
// write 1<<2 = begin player shoot sound
// write 1<<3 = begin enemy death sound
// write 1<<4 = begin wavestart (maybe)
// write 1<<5 = begin gameover (maybe)
//`H_SOUND_MUSICSTATE_IONUM
// 1 = be playing music
// 0 = pause the music
//`H_SOUND_CURRWAVE_IONUM
// wave number


	//
	//
	//
	// PWM stuff and/or block memory goes here!
	//
	//
	//
	
	// option A: MIDI/tone generation
	// have 1 or more counters, each will toggle the output every X cycles for a duration Y cycles
	// if multiple counters, we could create "chords" by having multiple tones running at once
	// human hearing goes up to 20KHz, so we should have plenty of precision within that range
	
	// option B: block memory
	// store obscenely bitcrushed audio (only 1-bit resolution!) in several "files", AHB will say "play file 2" or whatever
	
	// option C: combine A and B? create our own "audio files" that are a series of freq-duration pairs ending with a stop code?
	// better memory density than option B
	
	// AHB stuff
	wire  [3:0]	HADDR_d;
	wire		HWRITE_d;
	wire		HSEL_d;
	wire  [1:0]	HTRANS_d;
	wire        we_d;            // write enable

	// delay HADDR, HWRITE, HSEL, and HTRANS to align with HWDATA for writing
	delaybyx #(.X(1), .WIDTH(8)) delayby1 (.clk(HCLK), .in({HADDR,HWRITE,HSEL,HTRANS}), .out({HADDR_d,HWRITE_d,HSEL_d,HTRANS_d}));

	// overall write enable signal
	assign we_d = (HTRANS_d != `HTRANS_IDLE) & HSEL_d & HWRITE_d;

	// write block: TODO
	// always @(posedge HCLK or negedge HRESETn)
		// if (~HRESETn) begin
			// // reset the outputs
			// OUT_synchro_ack <= 1'b0;
			// OUT_motorctrl <= 8'b0;
			// OUT_botconfig <= 8'b0;
		// end else if (we_d)
			// case (HADDR_d)
				// `H_ROJO_ACK_IONUM: 		OUT_synchro_ack <= HWDATA[0];
				// `H_ROJO_MTRCTRL_IONUM: 	OUT_motorctrl <= HWDATA[7:0];
				// `H_ROJO_CONFIG_IONUM: 	OUT_botconfig <= HWDATA[7:0];
			// endcase

	// read block: NO READING, this module is write-only, don't touch HRDATA
	// always @(posedge HCLK or negedge HRESETn)
		// if (~HRESETn)
			// HRDATA <= 32'h0;
		// else
			// case (HADDR)
				// `H_XADC_RESULTA_IONUM: 	HRDATA <= { 20'b0, analog_result[11:0] }; // 20 + 12 = 32
				// `H_XADC_RESULTB_IONUM: 	HRDATA <= { 20'b0, analog_result[23:12] };
				// default:				HRDATA <= 32'h0;
			// endcase

endmodule

