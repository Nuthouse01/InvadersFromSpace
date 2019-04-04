// mfp_nexys4_ddr.v
// January 1, 2017
//
// Instantiate the mipsfpga system and rename signals to
// match the GPIO, LEDs and switches on Digilent's (Xilinx)
// Nexys4 DDR board

// Outputs:
// 16 LEDs (IO_LED)
// 7-Seg display
// VGA display 
// Inputs:
// 16 Slide switches (IO_Switch),
// 5 Pushbuttons (IO_PB): {BTNU, BTND, BTNL, BTNC, BTNR}
// 2 Paddle buttons
// 2 Paddle analog positions (XADC)
//

`include "mfp_ahb_const.vh"
`include "audio_values.vh"

`define MUTETHRESH			32


// module mfp_nexys4_ddr()
module mfp_top_audio_tonegen(
                        input                   CLK100MHZ,
                        input                   CPU_RESETN,
                        input                   BTNU, BTND, BTNL, BTNC, BTNR,
                        input  [`MFP_N_SW-1 :0] SW,
                        output [`MFP_N_LED-1:0] LED,
                        inout  [ 8          :1] JB,
                        input                   UART_TXD_IN,
                        output [7:0]            AN, // anode output for 7seg (digit enable)
                        output                  CA, CB, CC, CD, CE, CF, CG, // all segment cathodes
                        output                  DP, // decimal point cathode
                        output [3:0]            VGA_R, VGA_G, VGA_B,
                        output                  VGA_HS, VGA_VS,
                        // sound pin and audio enable
                        output					AUD_PWM, AUD_SD,
                        // paddle buttons
                        input                   PADDLE_A_BTN, PADDLE_B_BTN,
                        // Analog inputs for XADC
                        input                   vauxp2, vauxn2,
                        input                   vauxp3, vauxn3,
                        input                   vp_in, vn_in
                        );

    // Press btnCpuReset to reset the processor.


    wire clk25, clk5;
    wire tck_in, tck;
    wire [`MFP_N_PB-1:0] pushbutton_debounced;
    wire                 resetbutton_debounced;
    wire [`MFP_N_SW-1:0] switches_debounced;
	
	wire [`MFP_N_7SD_DIG-1:0] OUT_7SD_ANODE;
	wire [`MFP_N_7SD_SEG-1:0] OUT_7SD_CATHODE;
	assign AN = OUT_7SD_ANODE;
	assign {DP, CA, CB, CC, CD, CE, CF, CG} = OUT_7SD_CATHODE;
	
	
	// TODO: clock generator?
  clk_wiz_0 instance_name
	 (
	  // Clock out ports
	  .clk_out_25mhz(clk25),     // output clk_out_25mhz
	  .clk_out_5mhz(clk5),     // output clk_out_5mhz
	 // Clock in ports
	  .clk_in1(CLK100MHZ));      // input clk_in1


	// add 1 to number of PB cuz the resetbutton isn't counted in that number but is still debounced
    debounce #(.SIMULATE(1), .NUM_BUTTONS(`MFP_N_PB + 1), .NUM_SWITCHES(`MFP_N_SW)) debounce(
			.clk(clk25),
			.pbtn_in({BTNU, BTND, BTNL, BTNC, BTNR, CPU_RESETN}),
			.pbtn_db({pushbutton_debounced, resetbutton_debounced}),
			.switch_in(SW),
			.switch_db(switches_debounced)
			);

	// HAVE: square wave pwm generator
	// HAVE: sawtooth pwm generator
	// HAVE: triwave pwm generator
	// HAVE: pwm-to-sound generator (4 channels)

	// NEED: manual & constant tone generation project (and simulation) to sanity-check
	// switches=value (all sw high=330hz lowest? thats acceptable for testing, but in real version goes down to 20Hz)
	// buttons=which mode/channel
	// constraints & top level & debounce only & sevseg, no MFP

	// idea: "latch" the switches with the buttons
	// 7seg displays what each channel is set to
	// if channel is set to 0, mute itself
	// testing the 4 channels: square=UP, saw=LEFT, triangle=RIGHT, triangle=DOWN
	// center button: while pressed, mux to square-wave pure tone generator, bypass pwm thing

	// first get the "latching" thing down right
	reg [15:0] period0, period1, period2, period3;	
	always@(posedge clk25, negedge resetbutton_debounced) begin
		if(~resetbutton_debounced) begin
			period0 <= 16'h0; //up
			period1 <= 16'h0; //left
			period2 <= 16'h0; //right
			period3 <= 16'h0; //down
		end else begin
			if(pushbutton_debounced[`BUTT_UP_IDX])
				period0 <= switches_debounced;
			if(pushbutton_debounced[`BUTT_LEFT_IDX])
				period1 <= switches_debounced;
			if(pushbutton_debounced[`BUTT_RIGHT_IDX])
				period2 <= switches_debounced;
			if(pushbutton_debounced[`BUTT_DOWN_IDX])
				period3 <= switches_debounced;
		end
	end
	
	// then hook up the 7seg
	// Instantiate the sevensegtimer and hook it up to the right places
	
	wire [15:0] input_high, input_low;
	// can only hook up 2 values tho :(
	assign input_high = period1;
	assign input_low = period2;
	
	wire [31:0] reg_7sd_high_digits, reg_7sd_low_digits;
	assign reg_7sd_high_digits = {4'b0, input_high[15:12], 4'b0, input_high[11:8], 4'b0, input_high[7:4], 4'b0, input_high[3:0]};
	assign reg_7sd_low_digits =  {4'b0, input_low[15:12], 4'b0, input_low[11:8], 4'b0, input_low[7:4], 4'b0, input_low[3:0]};
	mfp_ahb_7sd_driver sevseg_driver(
			.clk(clk25),
			.resetn(resetbutton_debounced),
			.EN(8'hFF),
			.DP(8'h00),
			.DIGITS({reg_7sd_high_digits, reg_7sd_low_digits}),
			.DISP_EN_OUT(OUT_7SD_ANODE),
			.DISP_SEG_OUT(OUT_7SD_CATHODE)
			);

	
	
	wire [3:0] mute;
	assign mute = {(period3 < `MUTETHRESH), (period2 < `MUTETHRESH), (period1 < `MUTETHRESH), (period0 < `MUTETHRESH)};
	// LEDs ON=mute
	assign LED = {4{mute}};
	// only set AUD_SD (enable) low if all MUTE signals are high
	assign AUD_SD = ~(mute[3] & mute[2] & mute[1] & mute[0]) | pushbutton_debounced[`BUTT_CENTER_IDX];

	wire	audout_4chan, audout_squarewave;
	
	audio_everything_4channel everything (
			.HCLK(clk25),
			.audclk(clk5),
			.resetn(resetbutton_debounced),
			.mute(mute),
			.period0(period0),
			.period1(period1),
			.period2(period2),
			.period3(period3),
			.audout(audout_4chan)
			);
	
	
	// this one isn't latched, just driven by wherever the switches are at the moment
	// switch between this audout and 4channel audout via center button
//	audio_squarewave_audout squarewave_audout (
//			.clk(clk5),
//			.resetn(resetbutton_debounced),
//			.mute(0),
//			.period(switches_debounced),
//			.duty({1'b0, switches_debounced[15:1]}), // >>1 yields max volume
//			.audout(audout_squarewave)
//			);
	wire [`BITRES-1:0] pulsewidth5;
	audio_triwave_pulsewidth triwave_pulsewidth_3 (
					.clk(clk5),
					.resetn(resetbutton_debounced),
					.mute(0),
					.period(switches_debounced),
					.pulsewidth(pulsewidth5)
					);
	audio_pwm_audout_1chan pwm_audout_1chan (
							.clk(clk25),
							.resetn(resetbutton_debounced),
							.mute(0), // unary AND of all 4 signals
							.pulsewidth(pulsewidth5),
							.audout(audout_squarewave)
							);

	// mux the two outputs depending on the center button
	assign AUD_PWM = (pushbutton_debounced[`BUTT_CENTER_IDX])?(audout_squarewave):(audout_4chan);
	

	// LATER: "tone sequence on demand" architecture
	// MAYBE: decide how best to pause/freeze things
	
endmodule
