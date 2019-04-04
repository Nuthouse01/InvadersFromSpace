
// take a PWM/pulsewidth value and create the audio output
// resolution bits controlled by BITRES

// asynchrous reset 'resetn'(neg)
// synchrous reset 'mute'(pos)


// if pulsewidth=0000, audout is flat 0
// if pulsewidth=1111, audout is almost flat 1

`include "audio_values.vh"

module audio_pwm_audout_1chan (
	input					clk,
	input					resetn,
	input					mute,
	input	[`BITRES-1:0]	pulsewidth,
	output 	reg				audout
	);
	
	reg [`BITRES-1:0] count;
	
	always@(posedge clk, negedge resetn) begin
		//reset/inc the counter
		if(~resetn | mute) begin
			// reset
			count <= 0;
			audout <= 0;
		end else begin
			// set audout signal based on above/below the duty point
			audout <= (count < pulsewidth);
			// inc
			count <= count+1;
		end
	end
endmodule