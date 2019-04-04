// mfp_nexys4_ddr.v
// January 1, 2017
//
// Instantiate the mipsfpga system and rename signals to
// match the GPIO, LEDs and switches on Digilent's (Xilinx)
// Nexys4 DDR board

// Outputs:
// 16 LEDs (IO_LED) 
// Inputs:
// 16 Slide switches (IO_Switch),
// 5 Pushbuttons (IO_PB): {BTNU, BTND, BTNL, BTNC, BTNR}
//

`include "mfp_ahb_const.vh"

module mfp_nexys4_ddr( 
                        input                   CLK100MHZ,
                        input                   CPU_RESETN,
                        input                   BTNU, BTND, BTNL, BTNC, BTNR, 
                        input  [`MFP_N_SW-1 :0] SW,
                        output [`MFP_N_LED-1:0] LED,
                        inout  [ 8          :1] JB,
                        input                   UART_TXD_IN,
                        output [7:0]            AN, // anode output for 7seg (digit enable)
                        output                  CA, CB, CC, CD, CE, CF, CG, // all segment cathodes
                        output                  DP, // dot pointer cathode
						//output [3:0]			VGA_R, VGA_G, VGA_B,
						//output					VGA_HS, VGA_VS
						
						// Audio output signals
						output					AUD_PWM,	// PWM output to audio jack
						output					AUD_SD		// PWM output enable (? TODO: check this)
                        );

	// Press btnCpuReset to reset the processor. 
    
	wire clk50, clk75, clk25;
	wire tck_in, tck;
	wire [ 4:0] pushbutton_debounced;
	wire        resetbutton_debounced;
	wire [15:0] switches_debounced;
	
	wire 		IO_UPDATE, IO_ACK, IO_SET;						// for synchronizer ff
	wire [13:0]	worldmap_addr0, worldmap_addr1;
	wire [1:0]	worldmap_data0, worldmap_data1;

	clk_wiz_0 clk_wiz_0(.clk_in_100mhz(CLK100MHZ), .clk_out_50mhz(clk50), .clk_out_75mhz(clk75), .clk_out_25mhz(clk25));
	IBUF IBUF1(.O(tck_in),.I(JB[4]));
	BUFG BUFG1(.O(tck), .I(tck_in));
  
	debounce debounce(
                    .clk(clk50),
                    .pbtn_in({CPU_RESETN, BTNU, BTND, BTNL, BTNC, BTNR}),
                    .switch_in(SW),
                    .pbtn_db({resetbutton_debounced, pushbutton_debounced}),
                    .switch_db(switches_debounced));

	mfp_sys mfp_sys(
			        .SI_Reset_N(resetbutton_debounced),
                    .SI_ClkIn(clk50),
                    .HADDR(),
                    .HRDATA(),
                    .HWDATA(),
                    .HWRITE(),
					.HSIZE(),
                    .EJ_TRST_N_probe(JB[7]),
                    .EJ_TDI(JB[2]),
                    .EJ_TDO(JB[3]),
                    .EJ_TMS(JB[1]),
                    .EJ_TCK(tck),
                    .SI_ColdReset_N(JB[8]),
                    .EJ_DINT(1'b0),
                    .IO_Switch(switches_debounced),
                    .IO_PB(pushbutton_debounced),
                    .IO_LED(LED),
                    .UART_RX(UART_TXD_IN),
                    .OUT_7SD_ANODE(AN),
                    .OUT_7SD_CATHODE({DP, CA, CB, CC, CD, CE, CF, CG})
                    );

    //	Sound module for FPGA - turns switch data into audio out
    //      Generates PWM data and outputs it throught audio jack			
    sound_generator sound1 (
                    .clock_in100MHz(CLK100MHZ), 
                    .clock_in25MHz(clk25), 
                    .music_en(1'b1), // Currently hardcoded for testing
                    .switches(switches_debounced), 
                    .PWM_out(AUD_PWM), 
                    .AUDIO_en(AUD_SD));
	
endmodule
