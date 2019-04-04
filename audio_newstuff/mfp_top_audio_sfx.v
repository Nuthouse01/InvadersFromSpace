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
module mfp_top_audio_sfx(
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
	
	
	// clock generator?
  clk_wiz_0 instance_name
	 (
	  // Clock out ports
	  .clk_out_25mhz(clk25),     // output clk_out_25mhz
	  .clk_out_5mhz(clk5),     // output clk_out_5mhz
	 // Clock in ports
	  .clk_in1(CLK100MHZ));      // input clk_in1


	// add 1 to number of PB cuz the resetbutton isn't counted in that number but is still debounced
    debounce #(.SIMULATE(0), .NUM_BUTTONS(`MFP_N_PB + 1), .NUM_SWITCHES(`MFP_N_SW)) debounce(
			.clk(clk25),
			.pbtn_in({BTNU, BTND, BTNL, BTNC, BTNR, CPU_RESETN}),
			.pbtn_db({pushbutton_debounced, resetbutton_debounced}),
			.switch_in(SW),
			.switch_db(switches_debounced)
			);

	/* BEHAVIOR: 
	when press a button, play one of the sounds
	channels determined by switches
	when press center, stop all
	*/
	
	
    wire [`MFP_N_PB-1:0] pushbutton_delay, butt_posedge;
    delaybyx #(.X(1), .WIDTH(`MFP_N_PB)) delaybutts(
    	.clk(clk25),
    	.in(pushbutton_debounced),
    	.out(pushbutton_delay)
    	);
	assign butt_posedge = pushbutton_debounced & ~pushbutton_delay;
	
	
	reg [1:0] HTRANS = `HTRANS_NONSEQ;
	reg HWRITE = 1;
	
	reg [3:0] HADDR; // depends on what button is pressed
	reg HSEL;
	reg [31:0] HWDATA; // depends on buttons & switches
	
	wire [31:0] HWDATA_d;
    delaybyx #(.X(1), .WIDTH(32)) delayhwdata(
		.clk(clk25),
		.in(HWDATA),
		.out(HWDATA_d)
		);

	always @(*) begin
		HSEL = | butt_posedge;
		HADDR = 0;
		HWDATA = 0;
		
		// testfile4: ID=5, DEPTH=2
		// testfile4: 0=player explode, 3=enemy explode, 6=player shoot, 9=loop1(A4), 10=loop2(D4), 11=looping scale, 14=song_low, 19=song_high
		
		
		// up=explode
		// right=shoot
		// left=tone D
		// down=tone A
		// center=stop
		
//		if(butt_posedge[`BUTT_UP_IDX]) begin
//			// UP: sfx1 (id 0-1)
//			HADDR = `H_SOUND_SOUNDFX_IONUM;
//			HWDATA = {switches_debounced[3:0], 28'h0};
//		end else if(butt_posedge[`BUTT_LEFT_IDX]) begin
//			// LEFT: looping test 2
//			HADDR = `H_SOUND_SOUNDFX_IONUM;
//			HWDATA = {switches_debounced[3:0], 28'hA};
//		end else if(butt_posedge[`BUTT_RIGHT_IDX]) begin
//			// RIGHT: sfx3 (id 4-5)
//			HADDR = `H_SOUND_SOUNDFX_IONUM;
//			HWDATA = {switches_debounced[3:0], 28'h6};
//		end else if(butt_posedge[`BUTT_DOWN_IDX]) begin
//			// DOWN: looping test
//			HADDR = `H_SOUND_SOUNDFX_IONUM;
//			HWDATA = {switches_debounced[3:0], 28'h9};
//		end else if(butt_posedge[`BUTT_CENTER_IDX]) begin
//			// CENTER: stop everything
//			HADDR = `H_SOUND_STATUS_IONUM;
//			HWDATA = 32'h000f;
//		end
		
		// mode 2: scale, pause & resume
		if(butt_posedge[`BUTT_UP_IDX]) begin
			// UP: begin the scale
			HADDR = `H_SOUND_SOUNDFX_IONUM;
			HWDATA = {switches_debounced[3:0], 28'hB};
		end else if(butt_posedge[`BUTT_LEFT_IDX]) begin
			// LEFT: song low
			HADDR = `H_SOUND_SOUNDFX_IONUM;
			HWDATA = {switches_debounced[3:0], 28'hE};
		end else if(butt_posedge[`BUTT_RIGHT_IDX]) begin
			// RIGHT: song high
			HADDR = `H_SOUND_SOUNDFX_IONUM;
			HWDATA = {switches_debounced[3:0], 28'h13};
		end else if(butt_posedge[`BUTT_DOWN_IDX]) begin
			// DOWN: looping test
			HADDR = `H_SOUND_SOUNDFX_IONUM;
			HWDATA = {switches_debounced[3:0], 28'h9};
		end else if(butt_posedge[`BUTT_CENTER_IDX]) begin
			// CENTER: stop everything
			HADDR = `H_SOUND_STATUS_IONUM;
			HWDATA = 32'h000f;
		end
	end
	
	mfp_ahb_audio audio (
			.HCLK(clk25),
			.HRESETn(resetbutton_debounced),
			.HADDR(HADDR),
			.HTRANS(HTRANS),
			.HWDATA(HWDATA_d),
			.HWRITE(HWRITE),
			.HSEL(HSEL),
			.HRDATA(),
			.OUT_audio_pwm_and_enable({AUD_SD, AUD_PWM}),
			.IN_audio_clock(clk5)
			);
	
endmodule
