
// synchronous counter with asynchronous rst_n
// variable output bit size and top-count (counts from 0 - TOP, inclusive)

// just supports inc and hold

module basic_counter #(parameter WIDTH=5, parameter TOP=((2**WIDTH)-1)) (
	input 					clk,
	input 					rst_n, 
	input 					inc,
	output reg	[WIDTH-1:0] curr_val);
	
	always_ff @(posedge clk, negedge rst_n) begin
		if (~rst_n)						curr_val <= 0;				// reset
		else if(inc && (curr_val==TOP))	curr_val <= 0;				// if at max, next inc will wrap
		else if(inc)					curr_val <= curr_val + 1;	// count up on inc
		else							curr_val <= curr_val;		// hold the current count
	end
	
endmodule

