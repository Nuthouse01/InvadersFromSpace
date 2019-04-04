// Brian Henson, 5/26/2017

// delays the input signal (default width=32, configurable) by 1 clock cycle
// asynchronous negative reset

module delayby1 #(parameter WIDTH=32) (
	input 					clk,
	input 					resetn,
	input		[WIDTH-1:0] in,
	output reg	[WIDTH-1:0] out
	);
	
	always @(posedge clk, negedge resetn) begin
		if (~resetn)	out <= {WIDTH{1'b0}};			// reset
		else			out <= in;					// pass
	end
	
endmodule

