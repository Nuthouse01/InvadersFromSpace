// mfp_ahb_7sd.v
//
// Brian Henson & Jamie Williams
//
// Custom written 7sd module for
// Digilent's (Xilinx) Nexys4-DDR board


`include "mfp_ahb_const.vh"

module mfp_ahb_7sd(
    input                        HCLK,
    input                        HRESETn,
    input      [  3          :0] HADDR, // Bits [5:2] of actual address (NOTE: we only use [3:2])
    input      [  1          :0] HTRANS,
    input      [ 31          :0] HWDATA,
    input                        HWRITE,
    input                        HSEL,
    output reg [ 31          :0] HRDATA,

	// Memory-mapped I/O - 7SD specific
	output		[`MFP_N_7SD_DIG-1:0] OUT_7SD_ANODE,		// 8 bits
	output		[`MFP_N_7SD_SEG-1:0] OUT_7SD_CATHODE	// 8 bits
);
// NOTE: anodes (enable) were active-low (since the provided sevensegtimer module creates active-low output)
// NOTE: cathode (segments) hooked up in this order: {DP, CA, CB, CC, CD, CE, CF, CG}
// confirmed that the decoder module matches this order

// This module will have 2 32-bit registers, for the HSEG==x4 and LSEG==x8 addresses
// It will also have 2 8-bit registers for the 8 enable bits(active high) and 8 dec point bits(active high)
// Those registers will be directly/literally written by HWDATA, no translation needed
// Those registers will feed into the sevensegtimer module, and the sevensegtimer module will create the anode/cathode outputs

// In the GPIO module the writes are delayed by 1 cycle... but the reads happen instantly. We follow that convention.


	reg  [3:0]  HADDR_d;
	reg         HWRITE_d;
	reg         HSEL_d;
	reg  [1:0]  HTRANS_d;
	wire        we_d;            // write enable, delayed

	reg  [31:0] reg_7sd_high_digits, reg_7sd_low_digits;
	reg  [7:0]  reg_7sd_dp, reg_7sd_enable;

	// Delay HADDR, HWRITE, HSEL, and HTRANS to align with HWDATA for writing
	always @ (posedge HCLK) 
	begin
		HADDR_d  <= HADDR;
		HWRITE_d <= HWRITE;
		HSEL_d   <= HSEL;
		HTRANS_d <= HTRANS;
	end

	// Overall write enable signal
	assign we_d = (HTRANS_d != `HTRANS_IDLE) & HSEL_d & HWRITE_d;

	// Instantiate the sevensegtimer and hook it up to the right places
	mfp_ahb_7sd_driver sevseg_driver(
						.clk(HCLK),
						.resetn(HRESETn),
						.EN(reg_7sd_enable),
						.DIGITS({reg_7sd_high_digits, reg_7sd_low_digits}),
						.DP(reg_7sd_dp),
						.DISP_EN_OUT(OUT_7SD_ANODE),
						.DISP_SEG_OUT(OUT_7SD_CATHODE)
						);

	// This is the "write" block
	always @(posedge HCLK or negedge HRESETn)
		// If neg edge reset, set all held info to 0
		if (~HRESETn) begin	
			reg_7sd_enable <= 8'b0;
			reg_7sd_high_digits <= 32'b0;
			reg_7sd_low_digits <= 32'b0;
			reg_7sd_dp <= 8'b0;
		end
		else if (we_d)
			case (HADDR_d)
				`H_7SD_DIGEN_IONUM: reg_7sd_enable <=      HWDATA[`MFP_N_7SD_DIG-1:0];
				`H_7SD_HSEG_IONUM:  reg_7sd_high_digits <= HWDATA;
				`H_7SD_LSEG_IONUM:  reg_7sd_low_digits <=  HWDATA;
				`H_7SD_DP_IONUM:    reg_7sd_dp <=          HWDATA[`MFP_N_7SD_DIG-1:0];
			endcase

	// This is the "read" block
	always @(posedge HCLK or negedge HRESETn)
		if (~HRESETn)
			HRDATA <= 32'h0;
		else
		case (HADDR)
			// HRDATA gets {a whole bunch of zeros} + the 8-bit reg
			`H_7SD_DIGEN_IONUM: HRDATA <= {{32 - `MFP_N_7SD_DIG {1'b0}}, reg_7sd_enable};
			`H_7SD_HSEG_IONUM:  HRDATA <= reg_7sd_high_digits;
			`H_7SD_LSEG_IONUM:  HRDATA <= reg_7sd_low_digits;
			`H_7SD_DP_IONUM:    HRDATA <= {{32 - `MFP_N_7SD_DIG {1'b0}}, reg_7sd_dp};
			default:            HRDATA <= 32'b0;
		endcase

endmodule

