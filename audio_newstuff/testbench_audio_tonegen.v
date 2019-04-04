


`timescale 100ps/1ps

`include "mfp_ahb_const.vh"

module testbench_audio_tonegen;

	// define input/output signals (inputs are regs)
    reg                    SI_Reset_N, SI_ClkIn;
    reg  [`MFP_N_SW-1  :0] IO_Switch;
    reg  [`MFP_N_PB-1  :0] IO_PB;
    wire [`MFP_N_LED-1 :0] IO_LED;
	wire [`MFP_N_7SD_DIG-1:0] OUT_7SD_ANODE;
	wire [`MFP_N_7SD_SEG-1:0] OUT_7SD_CATHODE;
	wire AUD_PWM, AUD_SD;
	
	// unused things?
	reg        UART_TXD_IN;
    wire       EJ_TDO; // the only output
    reg        SI_ColdReset_N, EJ_TRST_N_probe, EJ_TCK, EJ_TDI, EJ_TMS;
	wire [8:1] JB_inout;
	assign JB_inout = {SI_ColdReset_N, EJ_TRST_N_probe, 1'bX, 1'bX, EJ_TCK, EJ_TDO, EJ_TDI, EJ_TMS};
	// [8:1] JB
	// 1=EJ_TMS=input 0
	// 2=EJ_TDI=input 0
	// 3=EJ_TDO=output
	// 4=EJ_TCK=input 1 ?
	// 7=EJ_TRST_N_probe=input 0
	// 8=SI_ColdReset_N=input 1
	
    initial begin
		IO_Switch = 0;
		IO_PB = 0;
        SI_ClkIn = 0;
        EJ_TRST_N_probe = 0; EJ_TDI = 0; EJ_TMS = 0; EJ_TCK = 0;
        UART_TXD_IN = 1;
        SI_ColdReset_N = 1;

        forever
            #5 SI_ClkIn = ~SI_ClkIn;
    end
	
	
	// instantiate the module under test
	mfp_top_audio_tonegen board(
			.CLK100MHZ(SI_ClkIn),
			.CPU_RESETN(SI_Reset_N),
			.BTNU(IO_PB[BUTT_UP_IDX]), .BTND(IO_PB[BUTT_DOWN_IDX]), .BTNL(IO_PB[BUTT_LEFT_IDX]), .BTNC(IO_PB[BUTT_CENTER_IDX]), .BTNR(IO_PB[BUTT_RIGTH_IDX]),
			.SW(IO_Switch),
			.LED(IO_LED),
			.JB(JB_inout), 
			.UART_TXD_IN(UART_TXD_IN),
			.AN(OUT_7SD_ANODE), // anode output for 7seg (digit enable)
			.DP(OUT_7SD_CATHODE[7]), .CA(OUT_7SD_CATHODE[6]), .CB(OUT_7SD_CATHODE[5]), .CC(OUT_7SD_CATHODE[4]), 
			.CD(OUT_7SD_CATHODE[3]), .CE(OUT_7SD_CATHODE[2]), .CF(OUT_7SD_CATHODE[1]), .CG(OUT_7SD_CATHODE[0]), // all segment cathodes
			// sound pin and audio enable
			.AUD_PWM(AUD_PWM), 
			.AUD_SD(AUD_SD)
			);


    initial begin
		// first take care of the reset
        SI_Reset_N  <= 0;
        repeat (100) @(posedge SI_ClkIn);
        SI_Reset_N  <= 1;
        repeat (200) @(posedge SI_ClkIn);
		$stop;
		// now we can actually try sequences of inputs
		
		// squarewave mode first
		IO_PB[BUTT_CENTER_IDX] <= 1;
		repeat (50) @(posedge SI_ClkIn);
		IO_Switch <= 16'h000f;
		repeat (1000) @(posedge SI_ClkIn);
		IO_Switch <= 16'h00ff;
		repeat (2000) @(posedge SI_ClkIn);
		IO_Switch <= 16'h0fff;
		repeat (4000) @(posedge SI_ClkIn);
		$stop;
		
		// triwave mode next
		IO_PB[BUTT_CENTER_IDX] <= 0;
		IO_PB[BUTT_DOWN_IDX] <= 1;
		IO_Switch <= 16'h00ff;
		repeat (2000) @(posedge SI_ClkIn);
		
		
        $stop;
    end

    initial begin
        $dumpvars;
        $timeformat (-9, 1, "ns", 10);
    end
    
endmodule


