/*
 * Seven-segment display timer for the Nexys4 DDR board
 *
 * January 25, 2017
 * 
 * On August 9th, 2017
 * Modified by Srivatsa Yogendra to include the decimal points and to 
 * display special characters on the seven segment display.
 *
 * Feb 1, 2019
 * Brian Henson & Jamie Williams: changed things to make the code more efficient, added lots
 * of comments, and changed the digit-enable and decimal-point-enable to both be active-high.
 * Also renamed to "mfp_ahb_7sd_driver"
 */

module mfp_ahb_7sd_driver(
                     input            clk,
                     input            resetn,
                     input     [ 7:0] EN,
                     input     [63:0] DIGITS,
                     input     [ 7:0] DP,
                     output    [ 7:0] DISP_EN_OUT,
                     output    [ 7:0] DISP_SEG_OUT);
	// DISP_SEG_OUT is active-low
	// DISP_EN_OUT is also active-low
	// EN input is active-high!
	// this doesn't even need an enable input, does it? one of the 7seg patterns is "blank"=off
	// DP input is active-high! but the segments are active-low so they need inverted
	// within DIGITS, each group of 8 corresponds to one displayed digit, tho only 5 of the bits are used
	
	wire [15:0] cnt16;
	wire [ 2:0] cntSel;
	wire [ 5:0] numout;
	wire [ 7:0] DP_inv;
	assign DP_inv = ~DP;
	
	wire [ 7:0] curr_enable;
	// this creates a one-high signal corresponding to the currently active digit...
	assign curr_enable = (8'd1 << cntSel);
	// ... but only actually turn it on if its my turn AND the input enable mask says to turn it on (active high)
	// but the output should be active-low
	assign DISP_EN_OUT = ~(EN & curr_enable);
	
	// instantiate the 7seg decoder
	// outputs segment g on bit 0, segment a on bit 6, and decimal point on bit 7
	mfp_ahb_7sd_decoder sevseg_decoder(.data(numout), .seg(DISP_SEG_OUT));

	// this counter slows down the digit-cycle from 50Mhz to 762Hz (1.3ms per digit, 10ms to do all digits)
	basic_counter #(16) counter16(.clk(clk), .rst_n(resetn), .inc(1), .curr_val(cnt16));
	// this counter counts 0-7 to enable one digit at a time
	basic_counter #(3)  counterSel(.clk(clk), .rst_n(resetn), .inc(cnt16 == 0), .curr_val(cntSel));

	mux8    #(6) 	mux8_7segdigits({DP_inv[0],DIGITS[ 4: 0]},
									{DP_inv[1],DIGITS[12: 8]},
									{DP_inv[2],DIGITS[20:16]},
									{DP_inv[3],DIGITS[28:24]},
									{DP_inv[4],DIGITS[36:32]},
									{DP_inv[5],DIGITS[44:40]},
									{DP_inv[6],DIGITS[52:48]},
									{DP_inv[7],DIGITS[60:56]},
									cntSel, numout);

endmodule


// 8-bit multiplexer
module mux8 #(parameter WIDTH = 8)
             (input  [WIDTH-1:0]     d0, d1, d2, d3, d4, d5, d6, d7,
              input  [2:0]           s, 
              output reg [WIDTH-1:0] y);

  always @(*)
    case (s)
      3'b000:    y = d0;
      3'b001:    y = d1;
      3'b010:    y = d2;
      3'b011:    y = d3;
      3'b100:    y = d4;
      3'b101:    y = d5;
      3'b110:    y = d6;
      3'b111:    y = d7;
      default:   y = d0;
    endcase

endmodule

