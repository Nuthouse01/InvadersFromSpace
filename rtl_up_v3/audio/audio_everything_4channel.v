

// this bundles all of the 5 audio modules into a single module

// asynchrous overall reset 'resetn'(neg)
// synchrous per-channel reset 'mute'(pos)

// TODO: test how the different pulsewidth schemes sound, pick 1 to use for all 4 channels (square/saw/tri)

`include "audio_values.vh"

module audio_everything_4channel (
	input					HCLK,
	input					audclk,
	input					resetn,
	input	[3:0]			mute,
	input	[`PERWIDTH-1:0]	period0,
	input	[`PERWIDTH-1:0]	period1,
	input	[`PERWIDTH-1:0]	period2,
	input	[`PERWIDTH-1:0]	period3,
	output 					audout
	);
	
	wire [`BITRES-1:0] pulsewidth0, pulsewidth1, pulsewidth2, pulsewidth3;

	// audio_squarewave_pulsewidth squarewave_pulsewidth_0 (
			// .clk(audclk),
			// .resetn(resetn),
			// .mute(mute[0]),
			// .period(period0),
			// .duty({1'b0, period0[`PERWIDTH-1:1]}), // >>1 yields max volume
			// .pulsewidth(pulsewidth0)
			// );
	audio_triwave_pulsewidth triwave_pulsewidth_0 (
			.clk(audclk),
			.resetn(resetn),
			.mute(mute[0]),
			.period(period0),
			.pulsewidth(pulsewidth0)
			);
	
	// audio_sawtooth_pulsewidth sawtooth_pulsewidth_1 (
			// .clk(audclk),
			// .resetn(resetn),
			// .mute(mute[1]),
			// .period(period1),
			// .pulsewidth(pulsewidth1)
			// );
	
	audio_triwave_pulsewidth triwave_pulsewidth_1 (
			.clk(audclk),
			.resetn(resetn),
			.mute(mute[1]),
			.period(period1),
			.pulsewidth(pulsewidth1)
			);
	
	// audio_triwave_pulsewidth triwave_pulsewidth_2 (
			// .clk(audclk),
			// .resetn(resetn),
			// .mute(mute[2]),
			// .period(period2),
			// .pulsewidth(pulsewidth2)
			// );
	// audio_triwave_pulsewidth triwave_pulsewidth_3 (
			// .clk(audclk),
			// .resetn(resetn),
			// .mute(mute[3]),
			// .period(period3),
			// .pulsewidth(pulsewidth3)
			// );
			
	audio_sawtooth_pulsewidth sawtooth_pulsewidth_2 (
			.clk(audclk),
			.resetn(resetn),
			.mute(mute[2]),
			.period(period2),
			.pulsewidth(pulsewidth2)
			);
	
	audio_sawtooth_pulsewidth sawtooth_pulsewidth_3 (
			.clk(audclk),
			.resetn(resetn),
			.mute(mute[3]),
			.period(period3),
			.pulsewidth(pulsewidth3)
			);
	

	// here is where they are combined
	audio_pwm_audout_4chan pwm_audout_4chan (
			.clk(HCLK),
			.resetn(resetn),
			.mute(& mute), // unary AND of all 4 signals
			.pulsewidth0(pulsewidth0),
			.pulsewidth1(pulsewidth1),
			.pulsewidth2(pulsewidth2),
			.pulsewidth3(pulsewidth3),
			.audout(audout)
			);

endmodule