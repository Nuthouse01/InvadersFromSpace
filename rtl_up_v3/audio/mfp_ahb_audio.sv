// mfp_ahb_audio.v

/*
GOAL: sfx-on-demand architecture
4 channels, independently selectable, each running (triwave/squarewave/sawtooth) (parameterizable?)
4bit for channel select (write any/many, more chan = more vol)
5bit for SFX select, 32 slots for sound sequences, but they can use more than 1 i guess
=32 bit total

stop: write 1 to stop bit, or when SFX is completed
pause: write 1 to pause bit (ignored if currently stopped)
resume: write 1 to playing bit (ignored if currently stopped)

per-channel mute: not directly writeable, derived from stop/pause
mute forces the PWM to be 7(half)
AUD_SD = 4-way NAND (~(AND))


SFX stored in memory as sequence of period/duration pairs
	use COE/block mem/maybe the TXT method?
play until jump code(for looping)/stop code
pause/resume will be supported
??? what scale to use for "duration" values? 9 bits to work with!=~500x
0.005s = 1/200th second, therefore max time is ~2.5s
*/



// ALSO: control logic clocked with HCLK
// duration/freq clocked with IN_audclk
// NOTE: consider reducing sound-clock somewhat, 100MHz means the periods are needlessly large
// slower clock would mean smaller numbers, less bits, smaller memory

// 100MHz = 24period, 8duration
// 50MHz = 23period, 8duration
// 25MHz = 22period, 8duration
// etc

//=================================================
// TODO: break the AUD_PWM and AUD_SD signals apart (bundling signals controlled by different clocks won't end well)
// TODO: re-evaluate audclk freq, period bitwidth
// TODO: move paused/play FF out of here and into the manager file
// TODO: eliminate the ID/DEPTH split, make all addresses TIGHT
// TODO: debug the actual pitch/tonality of the sounds, try different generation profiles
// TODO: try PWM clock as audclk/HCLK

// TODO: write music

// TODO: integrate with full Space Invaders system (add hardware & modify software)



`include "mfp_ahb_const.vh"
`include "audio_values.vh"

module mfp_ahb_audio(
    input                        HCLK, //25MHz? clock
    input                        HRESETn,
    input      [  3          :0] HADDR,
    input      [  1          :0] HTRANS,
    input      [ 31          :0] HWDATA,
    input                        HWRITE,
    input                        HSEL,
    output reg [ 31          :0] HRDATA,
	// audio output
	output     [1:0]			OUT_audio_pwm_and_enable,// [1]=AUD_SD, [0]=AUD_PWM,
	input						IN_audio_clock //100MHz clock
);

// positions of bits in the words
localparam OFFSET_PAUSE = 8;
localparam OFFSET_PLAY = 4;
localparam OFFSET_STOP = 0;
localparam OFFSET_CHANNEL = 28;

// write 1<<1 = begin player explode sound
// write 1<<2 = begin player shoot sound
// write 1<<3 = begin enemy death sound
// write 1<<4 = begin wavestart (maybe)
// write 1<<5 = begin gameover (maybe)

// AHB stuff
wire  [3:0]	HADDR_d;
wire		HWRITE_d;
wire		HSEL_d;
wire  [1:0]	HTRANS_d;
wire        we_d;            // write enable

// delay HADDR, HWRITE, HSEL, and HTRANS to align with HWDATA for writing
delaybyx #(.X(1), .WIDTH(8)) delayby1 (.clk(HCLK), .in({HADDR,HWRITE,HSEL,HTRANS}), .out({HADDR_d,HWRITE_d,HSEL_d,HTRANS_d}));

// overall write enable signal
assign we_d = (HTRANS_d != `HTRANS_IDLE) & HSEL_d & HWRITE_d;
wire write_soundfx, write_status, write_tonegen;
assign write_soundfx = we_d && (HADDR_d == `H_SOUND_SOUNDFX_IONUM);
assign write_tonegen = we_d && (HADDR_d == `H_SOUND_TONEGEN_IONUM);
assign write_status = we_d && (HADDR_d == `H_SOUND_STATUS_IONUM);


// new interface
//`H_SOUND_SOUNDFX_IONUM=1f300000 //write
// one word(32): channel(4) + sfx(5)
//`H_SOUND_STATUS_IONUM= 1f300008 //read/write
// arrangement B
// X X X X   PS3 PS2 PS1 PS0   PL3 PL2 PL1 PL0   ST3 ST2 ST1 ST0
// pause = 8+i
// play = 4+i
// stop = 0+i


// for each channel:
// one set/reset FF for paused/unpaused
// one state machine for reading & progressing thru the sound sequence


reg			grabnewsfxid [3:0];	// soundmgr input, time to read&store new sfx addr
reg			stop [3:0]; 		// input signal to stop the system, aborts the current-playing sound
reg			pause [3:0]; 		// input signal to pause the system, flip the SRFF
reg			resume [3:0]; 		// input signal to unpause the system, flip the SRFF
reg			sfxpaused [3:0]; 	// set-reset FF
wire 		sfxplaying [3:0];	// soundmgr output, high if playing something
wire [3:0] mute; // link state to soundgen, == (!playing | paused)
wire [11:0] readme; // link state to HRDATA on status read
wire [`PERWIDTH-1:0] period [3:0]; // link "manager" to soundgen

wire [`READWIDTH-1:0] sfxmemread [3:0];
wire [`ADDRWIDTH-1:0] sfxmemaddr [3:0];

// ============================================================
// beginning of FOR-LOOP

// STOPPED: playing=0, paused=0
// INVALID: playing=0, paused=1
// PLAYING: playing=1, paused=0
// PAUSED:  playing=1, paused=1

genvar i;
generate
for(i = 0; i < 4; i = i + 1) begin : perchannel

	// this manages the "paused" flag: unpause on start, resume, stop, or if it somehow stops playing via some unexpected path
	always @ (posedge HCLK, negedge HRESETn) begin
		if(~HRESETn)
			sfxpaused[i] <= 1'b0;
		else if(grabnewsfxid[i] | resume[i] | stop[i] | ~sfxplaying[i])
			sfxpaused[i] <= 1'b0;
		else if(pause[i])
			sfxpaused[i] <= 1'b1;
	end

	// the sound manager that steps thru the tones in a sound sequence
	audio_sfx_manager mgr (
			.clk(HCLK),
			.resetn(HRESETn),
			// SFX block mem
			.sfxmemread( sfxmemread[i] ),
			.sfxmemaddr( sfxmemaddr[i] ),
			//inputs
			.pause( sfxpaused[i] ),
			.stop( stop[i] ),
			.newsfxid(HWDATA[`SFXIDBITS-1:0]),
			.grabnewsfxid(grabnewsfxid[i]),
			//outputs
			.playperiod(period[i]),
			.sfxplaying(sfxplaying[i])
			);
			
	// combinationally set these signals depending on inputs
	always @(*) begin
		// use these values on any path they aren't explicitly set
		grabnewsfxid[i] = 0;
		stop[i] = 0;
		pause[i] = 0;
		resume[i] = 0;
		
		if(write_soundfx && HWDATA[OFFSET_CHANNEL+i]) begin
			// if writing to the soundfx addr,
			grabnewsfxid[i] = 1;
		end else begin
			// if the sound is playing & there is a write to the status bits,
			if(sfxplaying[i] && write_status)
				if(HWDATA[OFFSET_STOP+i])
					// if stop bit is high, create signal
					stop[i] = 1;
				else if(HWDATA[OFFSET_PAUSE+i])
					// if pause bit is high, create signal
					pause[i] = 1;
				else if(HWDATA[OFFSET_PLAY+i])
					// if play bit is high, create signal
					resume[i] = 1;
		end
	end


	// connect the readable portion
	assign readme[OFFSET_STOP+i]  = ~sfxplaying[i];
	assign readme[OFFSET_PLAY+i]  = sfxplaying[i] && ~sfxpaused[i];
	assign readme[OFFSET_PAUSE+i] = sfxplaying[i] && sfxpaused[i];

	// connect mute: mute if paused or not playing
	assign mute[i] = ~sfxplaying[i] | sfxpaused[i];
end
endgenerate

// end of FOR-LOOP		
// ============================================================

blk_mem_gen_0 blkmem0and1 (
  .clka(HCLK),    // input wire clka
  .addra(sfxmemaddr[0]),  // input wire [6 : 0] addra
  .douta(sfxmemread[0]),  // output wire [27 : 0] douta
  .clkb(HCLK),    // input wire clkb
  .addrb(sfxmemaddr[1]),  // input wire [6 : 0] addrb
  .doutb(sfxmemread[1])  // output wire [27 : 0] doutb
);

blk_mem_gen_0 blkmem2and3 (
  .clka(HCLK),    // input wire clka
  .addra(sfxmemaddr[2]),  // input wire [6 : 0] addra
  .douta(sfxmemread[2]),  // output wire [27 : 0] douta
  .clkb(HCLK),    // input wire clkb
  .addrb(sfxmemaddr[3]),  // input wire [6 : 0] addrb
  .doutb(sfxmemread[3])  // output wire [27 : 0] doutb
);




// read block
always @(posedge HCLK or negedge HRESETn)
	if (~HRESETn)
		HRDATA <= 32'h0;
	else
		case (HADDR)
			`H_SOUND_STATUS_IONUM: 	HRDATA <= { 20'h0, readme };
			default:				HRDATA <= 32'h0;
		endcase


// ========================================
// actual output-generating section!!!!
// inputs: resetn, audclk, mute(4), period(4)
// outputs: AUD_PWM, AUD_SD
wire AUD_PWM, AUD_SD;
audio_everything_4channel everything (
		.audclk(IN_audio_clock),
		.HCLK(HCLK),
		.resetn(HRESETn),
		.mute(mute),
		.period0(period[0]),
		.period1(period[1]),
		.period2(period[2]),
		.period3(period[3]),
		.audout(AUD_PWM)
		);
assign AUD_SD = ~& mute;
assign OUT_audio_pwm_and_enable = {AUD_SD, AUD_PWM};
// ========================================
	
endmodule

