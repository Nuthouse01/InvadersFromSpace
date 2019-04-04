
// take 4 pwm/pulsewidth values and combines them additively to produce a combined audio out
// upside: true constructive/destructive interference
// downside: when playing only one channel, max volume is only 1/4 what it could be
// donwside: when playing only one channel, get some inaudibly-high frequency buzzing maybe? ~1.5MHz
// resolution bits controlled by BITRES

// asynchrous reset 'resetn'(neg)
// synchrous reset 'mute'(pos)

// if pwm=0000, audout is flat 0
// if pwm=1111, audout is almost flat 1

`include "audio_values.vh"

module audio_pwm_audout_4chan (
	input					clk,
	input					resetn,
	input					mute,
	input	[`BITRES-1:0]	pulsewidth0,
	input	[`BITRES-1:0]	pulsewidth1,
	input	[`BITRES-1:0]	pulsewidth2,
	input	[`BITRES-1:0]	pulsewidth3,
	output 	reg				audout
	);
	
	reg  [`BITRES+1:0] 	fullcount;
	// use [BITRES+1:BITRES] to select pwm input
	// use [BITRES-1:0] to compare against selected pwm input
	
	always@(posedge clk, negedge resetn) begin
		//reset/inc the counter
		if(~resetn | mute) begin
			// reset
			fullcount <= 0;
			audout <= 0;
		end else begin
			// set audout signal based on above/below the duty point
			// cycling between the 4 input duty points
			case(fullcount[`BITRES+1:`BITRES])
				2'b00: audout <= (fullcount[`BITRES-1:0] < pulsewidth0);
				2'b01: audout <= (fullcount[`BITRES-1:0] < pulsewidth1);
				2'b10: audout <= (fullcount[`BITRES-1:0] < pulsewidth2);
				2'b11: audout <= (fullcount[`BITRES-1:0] < pulsewidth3);
			endcase
			// inc, no need for wrap cuz it uses all of the bits in the counter
			fullcount <= fullcount+1;
		end
	end
endmodule