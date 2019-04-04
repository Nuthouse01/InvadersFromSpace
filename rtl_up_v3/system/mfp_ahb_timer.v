// mfp_ahb_timer.v
//
// Brian Henson
//
// Custom written timer module for
// Digilent's (Xilinx) Nexys4-DDR board


`include "mfp_ahb_const.vh"

module mfp_ahb_timer(
    input                        HCLK,
    input                        HRESETn,
    input      [  3          :0] HADDR, // unused
    input      [  1          :0] HTRANS,
    input      [ 31          :0] HWDATA,
    input                        HWRITE,
    input                        HSEL,
    output reg [ 31          :0] HRDATA
);

// added randomizer module using fibonacci LFSR in position 4, H_RANDOM_ADDR
// when writing to that address, copy timer0 or timer1 into the reg to act as a seed
// controlled by this param
localparam SEED_FROM_WHICH = 0;

/*
Timer module thoughts:
Primarily useful for maintaining a very specific main-loop frequency when there are many
possible execution branches that take different lengths.
Since incrementing by 1 in a basic delay-loop takes 3 processor cycles(?), and that would be 
the main use, this should count up by 1 every 3 clock cycles instead of each cycle.

EXAMPLE:
main_loop_top:
	sw		$0, 0($timeraddr)			# Restart counting from 0
	...everything...
	li 		$t0, 0						# Clear the register to use for counting
	# desired freq is 40Hz -> 1,250,000 clock cycles -> count up to 416,666
	li		$t1, 416666					# Load maximum value for counter
	lw		$t2, 0($timeraddr)			# "how much have i already counted up to?"
		li		$t3, 3		# NOTE: if the timer module increments every clock instead of
		div		$t2, $t3	# every third clock, then these 3 commands would be necessary
		mflo	$t2			#
	sub		$t1, $t1, $t2				# "how much more do i need to count"
	delay_loop:							# Main loop of delay
		addi 	$t0, $t0, 1				# - Add 1 to counter register
		bne		$t0, $t1, delay_loop	# - If counter != max value, loop
	j	main_loop_top	# When we've delayed down to 40Hz, go back to main loop

Note: the increment register may as well be 32 bits... doing the math, 
1/50MHz * 3 * 2^32 = ~260 seconds to overflow, WAY more than i need, so I don't need to worry
about overflows.
Behavior: on read, return the currently held value of the timer/counter... on write, set the
value to HWDATA... almost always going to be 32'b0 but whatever, more options are better.

2 or more 32-bit timers, but only 1 clock divider reg (slowdown)
*/


	wire  [3:0]  HADDR_d;
	wire         HWRITE_d;
	wire         HSEL_d;
	wire  [1:0]  HTRANS_d;
	wire        we_d;            // write enable, delayed
	integer i;


	wire [31:0] timer0_out, timer1_out;
	wire [1 :0] slowdown_out;
	wire 		third_clock;

	// delay HADDR, HWRITE, HSEL, and HTRANS to align with HWDATA for writing
	delaybyx #(.X(1), .WIDTH(8)) delayby1 (.clk(HCLK), .in({HADDR,HWRITE,HSEL,HTRANS}), .out({HADDR_d,HWRITE_d,HSEL_d,HTRANS_d}));

	// Overall write enable signal
	assign we_d = (HTRANS_d != `HTRANS_IDLE) & HSEL_d & HWRITE_d;

	// create the slowed-down clock signal: every 3 clock pulses
	// values are 0/1/2, so 2=3rd
	basic_counter #(.WIDTH(2), .TOP(2)) slowdown(
		.clk(HCLK), .rst_n(HRESETn), .inc(1'b1), .curr_val(slowdown_out));
		
	// This is the "write" block
	// each increments on HCLK when slowdown_out == 0, so every 3rd clock
	// each loads when we_d is high and HADDR_d matches
	fancy_counter #(.WIDTH(32)) timer0(
		.clk(HCLK), .rst_n(HRESETn), .inc(slowdown_out == 0), .dec(0), 
		.load(we_d && (`H_TIMER0_IONUM == HADDR_d)), .next_val(HWDATA), .curr_val(timer0_out));
		
	fancy_counter #(.WIDTH(32)) timer1(
		.clk(HCLK), .rst_n(HRESETn), .inc(slowdown_out == 0), .dec(0), 
		.load(we_d && (`H_TIMER1_IONUM == HADDR_d)), .next_val(HWDATA), .curr_val(timer1_out));
		
	// random!
	// linear-feedback shift register
	// if writing to this address, copy timer0 to this as the seed
	// if reading this address, read as normal
	// regardless, always be shifting
	// uses fibonacci LFSR strategy
	reg [15:0] random;
	always @(posedge HCLK or negedge HRESETn) begin
		if (~HRESETn)
			// reset
			random <= 16'b0;
		else if (we_d && (`H_RAND_IONUM == HADDR_d))
			// seed
			random <= SEED_FROM_WHICH ? timer1_out[15:0] : timer0_out[15:0];
		else begin
			// shift and "randomize"
			for(i = 1; i < 16; i = i+1)
				random[i] <= random[i-1];
			// this is the fibonacci part!
			random[0] <= random[10] ^ random[12] ^ random[13] ^ random[15];
		end
	end
	
	
	// This is the "read" block
	always @(posedge HCLK or negedge HRESETn)
		if (~HRESETn)
			HRDATA <= 32'b0;
		else 
			case(HADDR)
				`H_TIMER0_IONUM:	HRDATA <= timer0_out;
				`H_TIMER1_IONUM:	HRDATA <= timer1_out;
				`H_RAND_IONUM:		HRDATA <= {16'b0, random};
				default:			HRDATA <= 32'b0;
			endcase
endmodule

