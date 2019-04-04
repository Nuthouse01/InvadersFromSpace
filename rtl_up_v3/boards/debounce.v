// debounce.v - debounces pushbuttons and switches
//
// Copyright Roy Kravitz, 2014-2015, 2016
// 
// Created By:		Roy Kravitz
// Modified By:     Srivatsa Yogendra
// Last Modified:	9-Aug-2017 (SY)
//
// Revision History:
// -----------------
// Sep-2008		RK		Created this module for the Digilent S3E Starter Board
// Sep-2012		RK		Modified module for the Digilent Nexys 3 board
// Dec-2014		RK		Cleaned up the formatting.  No functional changes
// Mar-2014		CZ		Modified module for the Digilent Nexys 4 board
// Aug-2014		RK		Parameterized module.  Modified for Vivado and Nexys4	
// Aug-2017     SY      Modified to change the closk Frequency to 50MHz
// Feb-2019		BH		Modified to use arrays and for-loops for cleaner code, no functional change
//
// Description:
// ------------
// This circuit filters out mechanical bounce. It works by taking
// several time samples of the pushbutton and changing its output
// only after several sequential samples are the same value
// 
///////////////////////////////////////////////////////////////////////////

module debounce
#(
	// parameters
	parameter integer	DEBOUNCE_FREQUENCY_HZ	= 250,
	parameter integer	DEBOUNCE_WIDTH			= 4,
	parameter integer	NUM_BUTTONS				= 6,
	parameter integer	NUM_SWITCHES			= 16,
	// you can change these, technically, but dont
	parameter integer	CLK_FREQUENCY_HZ		= 50_000000, 
	parameter integer	RESET_POLARITY_LOW		= 1,
	parameter integer 	CNTR_WIDTH 				= 32,
	// for simulating easier
	parameter integer	SIMULATE				= 0,
	parameter integer	SIMULATE_FREQUENCY_CNT	= 5
)
(
	// ports
	input						clk,				// clock
	input	[NUM_BUTTONS-1:0]	pbtn_in,			// pushbutton inputs - including CPU RESET button (in LSB)
	input 	[NUM_SWITCHES-1:0]	switch_in,			// slider switch inputs

	output	[NUM_BUTTONS-1:0]	pbtn_db, 	        // debounced outputs of pushbuttons	
	output	[NUM_SWITCHES-1:0]	switch_db	        // debounced outputs of slider switches
);

	// CPU reset is on pb0.  need to take it's polarity into account
	// NOTE: i dont know what this does! and besides this only makes a 1-bit difference! ah whatever
	localparam		PB0_IN = RESET_POLARITY_LOW ? {DEBOUNCE_WIDTH{1'b1}} : {DEBOUNCE_WIDTH{1'b0}};
	// debounce clock divider 
	localparam		TOP_CNT = SIMULATE ? SIMULATE_FREQUENCY_CNT : ((CLK_FREQUENCY_HZ / DEBOUNCE_FREQUENCY_HZ) - 1);
	// total number of things to debounce, treat them all the same
	localparam		NUM_INPUTS = NUM_BUTTONS + NUM_SWITCHES;
	// used for counting in for-loops
	integer 		idx; 
	
	wire [NUM_INPUTS-1:0] all_in;
	assign all_in = {switch_in, pbtn_in}; // group all the inputs into one signal
	reg [NUM_INPUTS-1:0] all_db = {NUM_INPUTS{1'b0}}; // holds the debounced version of the output
	assign {switch_db, pbtn_db} = all_db; // connect it to the ports
	
	
	//shift registers used to debounce switches and buttons
	reg [(DEBOUNCE_WIDTH-1):0]	shift_all [NUM_INPUTS-1:0]; 			// all the shift regs
	initial begin
		shift_all[0] <= PB0_IN; 								// reset button is a special case
		for (idx=1; idx<NUM_INPUTS; idx=idx+1)
			shift_all[idx] <= {DEBOUNCE_WIDTH{1'b0}};		// init all of the shift regs to full 0s, except the reset button
	end
	
	// create the clock divider
	reg		[CNTR_WIDTH-1:0]	db_count = 0;
	always @(posedge clk)
		if (db_count == TOP_CNT)	db_count <= 1'b0;				// if at top, reset
		else						db_count <= db_count + 1'b1;	// otherwise, inc

	// shift in the new sample each time db_count reaches top
	always @(posedge clk) begin
		if (db_count == TOP_CNT) begin
			//shift registers for everything
			for (idx=0; idx<NUM_INPUTS; idx=idx+1)
				shift_all[idx] <= (shift_all[idx] << 1) | all_in[idx];
		end
		
		// if the shift reg holds all 0s or 1s, then set output to 0 or 1; otherwise hold
		//debounced outputs
		for (idx=0; idx<NUM_INPUTS; idx=idx+1)
			case(shift_all[idx])
				{DEBOUNCE_WIDTH{1'b0}}:		all_db[idx] <= 0;				// if steady 0s, set out to 0
				{DEBOUNCE_WIDTH{1'b1}}:		all_db[idx] <= 1;				// if steady 1s, set out to 1
				default:					all_db[idx] <= all_db[idx];		// otherwise, hold last
			endcase
	end
	
endmodule
