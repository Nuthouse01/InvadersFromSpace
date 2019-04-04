
// 100MHz, 1-Hz wave has period of 100M, =28 bits
// 100MHz, 100-Hz wave has period of 1M, =20 bits

// output the pulsewidth to make a square wave with this frequency
// outputs only 0000 or 1111
// control freq from period
// control volume from duty threshold 
// asynchrous reset 'resetn'(neg)
// synchrous reset 'mute'(pos)

`include "audio_values.vh"

module audio_squarewave_pulsewidth (
	input					clk,
	input					resetn,
	input					mute,
	input	[`PERWIDTH-1:0]	period,
	input	[`PERWIDTH-1:0]	duty,
	output reg [`BITRES-1:0]pulsewidth
	);
		
	reg [`PERWIDTH-1:0] count;
	
	always@(posedge clk, negedge resetn) begin
		//reset/inc/wrap the counter
		if(~resetn | mute) begin
			// reset
			count <= 0;
			pulsewidth <= `PWM_MUTED;
		end else begin
			// set audout signal based on above/below the duty point
			// 1111 or 0000
			pulsewidth <= {`BITRES{(count < duty)}};
			if(count >= period)
				// wrap
				count <= 0;
			else
				// inc
				count <= count + 1;
		end
	end
endmodule