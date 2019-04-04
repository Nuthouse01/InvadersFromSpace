// Brian Henson, 5/26/2017

// TOP is the max count it CAN HOLD
// BOTTOM is the bottom valid count, inclusive (almost always going to be 0)
// supports inc by 1, dec by 1, and load

module fancy_counter #(parameter WIDTH=5, parameter TOP=((2**WIDTH)-1), parameter BOTTOM=0) (
	input 					clk,
	input 					rst_n,
	input 					inc,
	input 					dec,
	input 					load,
	input		[WIDTH-1:0] next_val,
	output reg	[WIDTH-1:0] curr_val
	);
	
	always_ff @(posedge clk, negedge rst_n) begin
		if (~rst_n)							curr_val <= BOTTOM;			// reset
		else if(load)						curr_val <= next_val;		// load new
		else if(inc & dec)					curr_val <= curr_val;		// if inc&dec, then hold
		else if(inc&&(curr_val==TOP))		curr_val <= BOTTOM;			// if at max, next inc will wrap
		else if(inc)						curr_val <= curr_val + 1;	// count up on inc
		else if(dec&&(curr_val==BOTTOM))	curr_val <= TOP;			// if at min, next dec will wrap
		else if(dec)						curr_val <= curr_val - 1;	// count down on dec
		else								curr_val <= curr_val;		// hold the current count
	end
	
endmodule

