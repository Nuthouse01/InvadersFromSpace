// mfp_ahb.v
// 
// January 1, 2017
//
// AHB-lite bus module with 3 slaves: boot RAM, program RAM, and
// GPIO (memory-mapped I/O: switches and LEDs from the FPGA board).
// The module includes an address decoder and multiplexer (for 
// selecting which slave module produces HRDATA).

// 2/2/2019: changed to SystemVerilog to make use of the (==?) operator

`include "mfp_ahb_const.vh"


module mfp_ahb
(
	input                       HCLK,
	input                       HRESETn,
	input      [ 31         :0] HADDR,
	input      [  2         :0] HBURST,
	input                       HMASTLOCK,
	input      [  3         :0] HPROT,
	input      [  2         :0] HSIZE,
	input      [  1         :0] HTRANS,
	input      [ 31         :0] HWDATA,
	input                       HWRITE,
	output     [ 31         :0] HRDATA,
	output                      HREADY,
	output                      HRESP,
	input                       SI_Endian,

	// memory-mapped I/O
	input      [`MFP_N_SW-1 :0] IO_Switch,
	input      [`MFP_N_PB-1 :0] IO_PB,
	output     [`MFP_N_LED-1:0] IO_LED,

	// NEW - Added outputs for 7SD
	output		[`MFP_N_7SD_DIG-1:0] OUT_7SD_ANODE,		// 8 bits
	output		[`MFP_N_7SD_SEG-1:0] OUT_7SD_CATHODE,	// 8 bits
	// new - result of xadc conversion
	input		[23:0]			IN_analog_result,
	// new - stereo audio out
	output	[1:0]			OUT_audio_pwm_and_enable,
	input					IN_audio_clock,
    
    // new - secondary interface for sprite graphics ram for sprite manager
    input       [11:0]          SPRITE_ADDR, 
    output      [31:0]          SPRITE_DATA
);


	wire [31:0] HRDATA8, HRDATA7, HRDATA6, HRDATA5, HRDATA4, HRDATA3, HRDATA2, HRDATA1, HRDATA0;
	wire [ 8:0] HSEL;	 // NEW - Modified to add extra bit for 7SD
	wire [ 8:0] HSEL_d;	 // NEW - Modified to add extra bit for 7SD

	assign HREADY = 1;
	assign HRESP = 0;

	// Delay select signal to align for reading data
	delaybyx #(.X(1), .WIDTH(9)) delayby1 (.clk(HCLK), .in(HSEL), .out(HSEL_d));

	// Module 0 - boot ram
	mfp_ahb_b_ram mfp_ahb_b_ram(HCLK, HRESETn, HADDR, HBURST, HMASTLOCK, HPROT, HSIZE,
							  HTRANS, HWDATA, HWRITE, HRDATA0, HSEL[0]);
	// Module 1 - program ram
	mfp_ahb_p_ram mfp_ahb_p_ram(HCLK, HRESETn, HADDR, HBURST, HMASTLOCK, HPROT, HSIZE,
							  HTRANS, HWDATA, HWRITE, HRDATA1, HSEL[1]);
	// Module 2 - GPIO
	mfp_ahb_gpio mfp_ahb_gpio(HCLK, HRESETn, HADDR[5:2], HTRANS, HWDATA, HWRITE, HSEL[2], 
							HRDATA2, IO_Switch, IO_PB, IO_LED);
	// Module 3 - 7SD
	mfp_ahb_7sd mfp_ahb_7sd(HCLK, HRESETn, HADDR[5:2], HTRANS, HWDATA, HWRITE, HSEL[3], 
						  HRDATA3, OUT_7SD_ANODE, OUT_7SD_CATHODE);
	// module 4 - timer
	mfp_ahb_timer mfp_ahb_timer(HCLK, HRESETn, HADDR[5:2], HTRANS, HWDATA, HWRITE, HSEL[4], 
							HRDATA4);
	// module 5 - XADC
	mfp_ahb_XADC mfp_ahb_XADC(HCLK, HRESETn, HADDR[5:2], HTRANS, HWDATA, HWRITE, HSEL[5], 
							HRDATA5, IN_analog_result);
	// module 6 - audio
	mfp_ahb_audio mfp_ahb_audio(HCLK, HRESETn, HADDR[5:2], HTRANS, HWDATA, HWRITE, HSEL[6], 
							HRDATA6, OUT_audio_pwm_and_enable, IN_audio_clock);

	// module 7 - sprite ram (dual ported)
	mfp_ahb_sprite_ram mfp_ahb_sprite_ram(HCLK, HRESETn, HADDR, HBURST, HMASTLOCK, HPROT, HSIZE,
                            HTRANS, HWDATA, HWRITE, HRDATA7, HSEL[7], 
                            // secondary interface for sprite manager

                            SPRITE_ADDR, SPRITE_DATA
                            );

    mfp_ahb_sprite_table_ram mfp_ahb_sprite_table_ram(HCLK, HRESETn, HADDR, HBURST, HMASTLOCK, HPROT, HSIZE,
                            HTRANS, HWDATA, HWRITE, HRDATA8, HSEL[8]);
                            
	// decoder: analyze the input address, produce a 1-hot output that enables only 1 of the blocks at a time to receive the input
	ahb_decoder ahb_decoder(HADDR, HSEL);
	// mux: selects which output from the modules (HRDATA0/1/2/3) should be passed to the next level up (HRDATA)
	// uses HSEL_d so its delayed 1 cycle from the "read" command cuz it takes 1 cycle to fetch the data
	ahb_mux ahb_mux(HSEL_d, HRDATA8, HRDATA7, HRDATA6, HRDATA5, HRDATA4, HRDATA3, HRDATA2, HRDATA1, HRDATA0, HRDATA);
		
endmodule


module ahb_decoder
(
	input  [31:0] HADDR,
	output [ 8:0] HSEL
);

	// Decode based on most significant bits of the address
	// these options should be mutually exclusive
	// TODO: consider replacing this with casez and going back to .v extension?
	// 28:20 = 0b???x_xxxx_xxxx??????......
	assign HSEL[0] = (HADDR[28:20] ==? `H_RAM_RESET_ADDR_Match); // 128 KB RAM  at v0xbfc00000 (physical: 0x1fc00000)
	assign HSEL[1] = (HADDR[28:20] ==? `H_RAM_ADDR_Match);       // 256 KB RAM at v0x80000000 (physical: 0x00000000)
	assign HSEL[2] = (HADDR[28:20] ==? `H_LED_ADDR_Match);       // GPIO at v0xbf800000 (physical: 0x1f800000)
	assign HSEL[3] = (HADDR[28:20] ==? `H_7SD_ADDR_Match);		// 7SD  at v0xbf700000 (physical: 0x1f700000)
	assign HSEL[4] = (HADDR[28:20] ==? `H_TIMER_ADDR_Match);	// timer at 6
	assign HSEL[5] = (HADDR[28:20] ==? `H_XADC_ADDR_Match);		// xadc reader at 4
	assign HSEL[6] = (HADDR[28:20] ==? `H_SOUND_ADDR_Match);	// sound block at 3
	assign HSEL[7] = (HADDR[28:20] ==? `H_SPRITE_GFX_ADDR_Match);	// sprite graphics ram
	assign HSEL[8] = (HADDR[28:20] ==? `H_SPRITE_TBL_ADDR_Match);	// sprite table ram
endmodule


module ahb_mux
(
    input      [ 8:0] HSEL,
    input      [31:0] HRDATA8, HRDATA7, HRDATA6, HRDATA5, HRDATA4, HRDATA3, HRDATA2, HRDATA1, HRDATA0,
    output reg [31:0] HRDATA
);

    always @(*)
      casez (HSEL)
	      9'b????????1:	HRDATA = HRDATA0;
	      9'b???????10:	HRDATA = HRDATA1;
	      9'b??????100:	HRDATA = HRDATA2;
		  9'b?????1000:	HRDATA = HRDATA3;
		  9'b????10000:	HRDATA = HRDATA4;
		  9'b???100000:	HRDATA = HRDATA5;
		  9'b??1000000:	HRDATA = HRDATA6;
		  9'b?10000000:	HRDATA = HRDATA7;
		  9'b100000000: HRDATA = HRDATA8;
	      default:	HRDATA = HRDATA1;
      endcase
endmodule
