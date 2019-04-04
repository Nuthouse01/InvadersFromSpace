// this module is responsible for stepping thru the period/duration pairs in the sound memory
// in soundmem, there are 2^5 possible SFX IDs
// each effect has 2^4 period/duration pairs (depth)
// each address holds 32 bits, period=23 and duration=9
// loop code: when finding duration=000, interpret period as SFX ID 0-31 to begin playing next (usually self)
// stop code: when finding duration=000, if period=111111111, then don't jump, just stop

// durclkdiv is frozen when state=paused, reset when state=idle

`include "audio_values.vh"

module audio_sfx_manager (
	input						clk,
	input						resetn,
	// inputs
	// current stopped/paused/playing state of the FLIPFLOPS 
	input						pause,
	input						stop,
	input	[`SFXIDBITS-1:0]	newsfxid,
	input						grabnewsfxid,
	// outputs
	output	reg [`PERWIDTH-1:0]	playperiod,
	output 	reg					sfxplaying,
	// memory
	input	[`READWIDTH-1:0]	sfxmemread,
	output	[`ADDRWIDTH-1:0]	sfxmemaddr
	);


typedef enum logic[4:0] {
	IDLE = 		5'b00001,
	LOADING1 = 	5'b00010,
	LOADING2 = 	5'b00100, 
	BRANCHING = 5'b01000,
	COUNTING = 	5'b10000
} manager_state;
manager_state currstate, nextstate;

// where it connects to the block memory, break the input/address into named subsections for readability
// inputs
wire [`DURWIDTH-1:0]    new_duration;
//assign new_duration = sfxmemread[`DURWIDTH-1:0];
wire [`PERWIDTH-1:0]    new_period;
//assign new_period = sfxmemread[`READWIDTH-1:`DURWIDTH];
assign {new_period, new_duration} = sfxmemread;
// address (output)
reg [`SFXIDBITS-1:0]		sfx_id;
reg [`SFXDEPTHBITS-1:0]	sfx_depth;
assign sfxmemaddr = {sfx_id, sfx_depth};

// high when it should copy the sfxmemlookup result to the local registers (period and duration)
// combinational output of state machine
reg load_new_sound;
// high when it should increment the currently-held address, also when reading a normal per/dur pair
// combinational output of state machine
reg load_next_addr;
// high when it should directly load an address from new_period
// combinational output of state machine
reg load_jump_addr;
// high when the duration has elapsed
// input to state machine
wire duration_match;

reg  [`DURWIDTH-1:0]    duration_curr;
reg  [`DURCLKWIDTH-1:0] durclkdiv;
assign duration_match = (duration_curr == 0) && (durclkdiv == 0);



// ==================================================================
// begin state machine
/*
STATE MACHINE:
inputs:
outputs:

from any state, if 'stop' is high, then go to 'idle'
from any state, if grabnewsfxid=1, then go to 'loading' and next clock will store the incoming id

idle
	forcing all regs to reset so long as in this state
loading
	COUNTING for sfxid reg to percolate thru the sfx mem
	can't do anything yet
loading2
	same
branching
	waited enough, now have valid lookup results
	determine what to do next based on lookup result
	if its a normal period/duration, load it into playperiod/duration_target/resetdurclkdiv and go to 'COUNTING'
		also load addr+1 into addr: pre-emptive lookup
	if its a jump, load it into addr and go to 'loading'
		need to wait for lookup cuz it is jumping twice in rapid succession
	if its a stop, go to 'idle'
COUNTING
	COUNTING for duration to expire
	assuming minimum duration of 1 currcnt in this state
	durclkdiv, currcnt are unfrozen now (frozen all other times)
	when durclkdiv=0 and currcnt=target go to 'branching'
*/

// combinationally generate nextstate and a few outputs
always @ (*) begin
	// set default values to use unless a path explicitly overrides it
	nextstate = currstate;
	load_new_sound = 0;
	load_jump_addr = 0;
	load_next_addr = 0;

	// 'stop' -> idle and 'grabnewsfxid' -> loading1 have maximum priority, are present on all states
	case(currstate)
		IDLE: begin
			if(stop)
				nextstate = IDLE;
			else if(grabnewsfxid)
				nextstate = LOADING1; // this is the only way to escape 'idle' state
		end
		LOADING1: begin
			if(stop)
				nextstate = IDLE;
			else if(grabnewsfxid)
				nextstate = LOADING1;
			else
				nextstate = LOADING2;
		end
		LOADING2: begin
			if(stop)
				nextstate = IDLE;
			else if(grabnewsfxid)
				nextstate = LOADING1;
			else
			nextstate = BRANCHING;
		end
		BRANCHING: begin
			// waited enough, now have valid lookup results
			// determine what to do next based on lookup result
			if(stop)
				nextstate = IDLE;
			else if(grabnewsfxid)
				nextstate = LOADING1;
			else if(new_duration == 0) begin
				// this means its a jump code or stop code
				if(new_period < (2**`SFXIDBITS)) begin
					// its a jump code!
					load_jump_addr = 1;
					nextstate = LOADING1;
				end else
					// its a stop code!
					nextstate = IDLE;
			end else begin
				// this means its an ordinary period/duration pair
				load_new_sound = 1;
				load_next_addr = 1;
				nextstate = COUNTING;
			end
		end
		COUNTING: begin
			if(stop)
				nextstate = IDLE;
			else if(grabnewsfxid)
				nextstate = LOADING1;
			else if(duration_match)
				// once the duration has elapsed, go back to the branching state
				nextstate = BRANCHING;
		end
	endcase
end

// copy nextstate to currstate
always @ (posedge clk, negedge resetn) begin
	if (~resetn) begin
		// reset state & stuff
		currstate <= IDLE;
	end else begin
		// advance the state
		currstate <= nextstate;
	end
end

// end state machine
// ==================================================================



// sfxdone is a set-reset ff: when in idle state, at clock, set it to 1. when in COUNTING state, at clock, set it to 0.
// output to indicate to the outer system when this is/isn't playing something
always @ (posedge clk, negedge resetn) begin
	if(~resetn)
		sfxplaying <= 1'b0;
	else if(currstate == IDLE)
		sfxplaying <= 1'b0;
	else if(currstate == COUNTING)
		sfxplaying <= 1'b1;
end



// manage sfx_id and sfx_depth, the address that is connected to the sfx block memory
always @ (posedge clk, negedge resetn) begin
	if(~resetn) begin
		{sfx_id, sfx_depth} <= 0;
	end else if(grabnewsfxid) begin
		// new ID coming in from outside the module, gotta grab it now
		// this signal also is the only way to move out of IDLE state
		sfx_id <= newsfxid;
		sfx_depth <= 0;
	end else if(currstate == IDLE) begin
		{sfx_id, sfx_depth} <= 0;
	end else if(load_next_addr) begin
		// while COUNTING for a duration to elapse, pre-load the address of the following sound so it trickles thru the block mem faster
		{sfx_id, sfx_depth} <= {sfx_id, sfx_depth} + 1;
	end else if(load_jump_addr) begin
		// when a jump happens, the new address is got from whatever period was just read
		sfx_id <= new_period[`SFXIDBITS-1:0];
		sfx_depth <= 0;
	end
end





// durclkdiv: counter running each hclk (unless frozen?), counting UP, 200Hz
// duration_curr: counting DOWN from where initally set, decrease whenever durclkdiv = TOP
// considered done when durclkdiv=0 and duration_curr=0
// note: not supporting when target duration=0 because duration=0 means loop or stop, cannot be loaded as target
// pauses counting while 'pause' = 1
always @ (posedge clk, negedge resetn) begin
	if(~resetn | (currstate == IDLE)) begin
		duration_curr <= 0;
		durclkdiv <= 0;
	end else begin
		if(load_new_sound) begin
			// when a new duration is loaded, also reset durclkdiv
			duration_curr <= new_duration;
			durclkdiv <= 0;
		end else begin
			// if the timer is not paused AND the timer has time remaining
			// AKA if not paused and if not already done counting
			if((pause == 0) && (~duration_match)) begin
				if(durclkdiv == `DURCLK_TOP_CNT) begin
					// wrap and dec
					duration_curr <= duration_curr - 1;
					durclkdiv <= 0;
				end else
					durclkdiv <= durclkdiv + 1;
			end
		end
	end
end


// manage playperiod (output)
// read in from sfx mem once it is determined to be not a jump/stop code
always @ (posedge clk, negedge resetn) begin
	if(~resetn | (currstate == IDLE)) begin
		playperiod <= 0;
	end else if(load_new_sound) begin
		playperiod <= new_period;
	end
end





endmodule