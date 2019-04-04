// Brian Henson, 5/26/2017

// delays the input signal (default width=32, configurable) by X clock cycles
// no reset signal

module delaybyx #(parameter X=1, parameter WIDTH=32) (
	input 					clk,
	input		[WIDTH-1:0] in,
	output 		[WIDTH-1:0] out
	);
	
	reg [WIDTH-1:0] pipe [X:0];
	
	always@(*)
		pipe[X] <= in;	// copy of in, not really a reg at all, just an alias
	assign out = pipe[0];	// copied to out
	integer i;
	
	always @(posedge clk) begin
		for (i=0; i<X; i=i+1)
			pipe[i] <= pipe[i+1]; // if X=1, i=0 and theres 1 shift. if X=2, i=0/1 and there's 2 shifts. perfect.
	end
	
endmodule

