
// 100MHz, 1-Hz wave has period of 100M, =28 bits
// 100MHz, 100-Hz wave has period of 1M, =20 bits

// output the pulsewidth values to create a sawtooth wave at this frequency
// control freq from period
// asynchrous reset 'resetn'(neg)
// synchrous reset 'mute'(pos)


// within one [period] count from 0 to 15
// 0123 0123 0123 0123

// desired solution:
// scale count (0-period) down to (0-16)
// pulsewidth <= (count * 16) / period;
// but, cannot do "flexible denominator" in hardware!!

// compromise: many compares & interpret result
// if between 0/16 and 1/16, out=0
// if between 1/16 and 2/16, out=1
// if between 2/16 and 3/16, out=2
// if count >= X*period/16, out=X
// if count*16 >= X*period, out=X
// if period=10:
// 0*16=0 >=0  => 0
// 1*16=16>=10 => 1
// 2*16=32>=30 => 3
// 3*16=48>=40 => 4
// 4*16=64>=60 => 6

`include "audio_values.vh"

module audio_sawtooth_pulsewidth (
	input					clk,
	input					resetn,
	input					mute,
	input	[`PERWIDTH-1:0]	period,
	output reg [`BITRES-1:0]pulsewidth
	);
	
	localparam RESOLUTION = 2**`BITRES;         // default 16
	
	reg [`PERWIDTH-1:0] count;
	reg [`BITRES-1:0] currpulsewidth;
	
	//===================
	// generate 'pulsewidth' by determining where 'count' lies between 0-'period'
	// if count*16 >= X*period, out=X
	integer i;
	always @(*) begin
		currpulsewidth = 0;
		for(i=1; i<RESOLUTION; i=i+1) begin
			if((count * RESOLUTION) >= (period * i))
				currpulsewidth = i;
		end
	end
	//===================

	
	
	always@(posedge clk, negedge resetn) begin
		//reset/inc/wrap the counter
		if(~resetn | mute) begin
			// reset
			count <= 0;
			pulsewidth <= `PWM_MUTED;
		end else begin
			// clock the pulsewidth result that was combinationally calculated
			pulsewidth <= currpulsewidth;
			if(count < period)
				// inc
				count <= count + 1;
			else
				// wrap
				count <= 0;
		end
	end
endmodule