// mfp_ahb_XADC.v
//





`include "mfp_ahb_const.vh"

module mfp_ahb_XADC(
    input                        HCLK,
    input                        HRESETn,
    input      [  3          :0] HADDR,
    input      [  1          :0] HTRANS,
    input      [ 31          :0] HWDATA,
    input                        HWRITE,
    input                        HSEL,
    output reg [ 31          :0] HRDATA,

// the result of the XADC, concatenated into one signal { resultB, resultA }
	input [23:0]				analog_result
);

	
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

	// write block (no need to write into the XADC module!)

	// read block
	always @(posedge HCLK or negedge HRESETn)
		if (~HRESETn)
			HRDATA <= 32'h0;
		else
			case (HADDR)
				`H_XADC_RESULTA_IONUM: 	HRDATA <= { 20'b0, analog_result[11:0] }; // 20 + 12 = 32
				`H_XADC_RESULTB_IONUM: 	HRDATA <= { 20'b0, analog_result[23:12] };
				default:				HRDATA <= 32'h0;
			endcase

endmodule

