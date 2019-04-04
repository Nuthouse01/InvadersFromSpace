// testbench.v
// 31 May 2014
//
// Drive the mipsfpga_sys module for simulation testing

`timescale 100ps/1ps

`include "mfp_ahb_const.vh"

module testbench_EVERYTHING;

    reg                    SI_Reset_N, SI_ClkIn;
    reg                    EJ_TRST_N_probe, EJ_TDI; 
    wire                   EJ_TDO;
    reg                    SI_ColdReset_N;
    reg                    EJ_TMS, EJ_TCK, EJ_DINT;
    reg  [`MFP_N_SW-1  :0] IO_Switch;
    reg [          4  :0] IO_PB;
    wire [`MFP_N_LED-1 :0] IO_LED;
    reg                    UART_RX;
					
	
	wire [ 4:0] pushbutton_debounced;
	wire        resetbutton_debounced;
	wire [15:0] switches_debounced;
	
	wire 		IO_UPDATE, IO_ACK, IO_SET;						// for synchronizer ff
	wire [7:0]	LocX_reg, LocY_reg, Sensors_reg, BotInfo_reg;	// from rojo to mfp
	wire [7:0]	motorctrl, botconfig;					// from mfp to rojo
	wire [13:0]	worldmap_addr0, worldmap_addr1;
	wire [1:0]	worldmap_data0, worldmap_data1;

	
	
	debounce #(.SIMULATE(1)) debounce(
                    .clk(SI_ClkIn),
                    .pbtn_in({SI_Reset_N, IO_PB}),
                    .switch_in(IO_Switch),
                    .pbtn_db({resetbutton_debounced, pushbutton_debounced}),
                    .switch_db(switches_debounced));

					
    mfp_sys sys (.SI_Reset_N(resetbutton_debounced),
                 .SI_ClkIn(SI_ClkIn),
                 .EJ_TRST_N_probe(EJ_TRST_N_probe), 
                 .EJ_TDI(EJ_TDI), 
                 .EJ_TDO(EJ_TDO), 
                 .EJ_TMS(EJ_TMS), 
                 .EJ_TCK(EJ_TCK), 
                 .SI_ColdReset_N(SI_ColdReset_N), 
                 .EJ_DINT(EJ_DINT),
                 .IO_Switch(switches_debounced), 
                 .IO_PB(pushbutton_debounced), 
                 .IO_LED(IO_LED), 
                 .UART_RX(UART_RX),
					.IN_synchro			( IO_UPDATE ),
					.OUT_synchro_ack	( IO_ACK ),
					.IN_botinfo			( {LocX_reg,LocY_reg,Sensors_reg,BotInfo_reg} ), //(signals bundled/combined at top-level)
					.OUT_motorctrl		( motorctrl ),
					.OUT_botconfig		( botconfig )
    );

		rojobot_update_synchro synchro(
				.clk50(SI_ClkIn),
				.resetn(resetbutton_debounced),
				.IO_ACK(IO_ACK),
				.IO_SET(IO_SET),
				.IO_UPDATE(IO_UPDATE)
				);
	
	rojobot31_0 rojobot (
				.clk_in(SI_ClkIn),                   // input wire clk_in
				.reset(~resetbutton_debounced),   // input wire reset
				.upd_sysregs(IO_SET),             // output wire upd_sysregs
				
				.MotCtl_in(motorctrl),            // input wire [7 : 0] MotCtl_in
				.Bot_Config_reg(botconfig),       // input wire [7 : 0] Bot_Config_reg
				
				.LocX_reg(LocX_reg),              // output wire [7 : 0] LocX_reg
				.LocY_reg(LocY_reg),              // output wire [7 : 0] LocY_reg
				.Sensors_reg(Sensors_reg),        // output wire [7 : 0] Sensors_reg
				.BotInfo_reg(BotInfo_reg),        // output wire [7 : 0] BotInfo_reg
				
				.worldmap_addr(worldmap_addr0),   // output wire [13 : 0] worldmap_addr
				.worldmap_data(worldmap_data0)    // input wire [1 : 0] worldmap_data
				);

	assign worldmap_data0 = 14'b0;
	assign worldmap_data1 = 14'b0;
	
    initial
    begin
        SI_ClkIn = 0;
        EJ_TRST_N_probe = 0; EJ_TDI = 0; EJ_TMS = 0; EJ_TCK = 0; EJ_DINT = 0;
        UART_RX = 1;
        SI_ColdReset_N = 1;

        forever
            #50 SI_ClkIn = ~ SI_ClkIn;
    end

    initial
    begin
        IO_Switch = `MFP_N_SW'habcd; # 100;
        SI_Reset_N  <= 0;
        repeat (100)  @(posedge SI_ClkIn);
        SI_Reset_N  <= 1;
        repeat (10)  @(posedge SI_ClkIn);
        IO_Switch = `MFP_N_SW'habcd;
        repeat (100)  @(posedge SI_ClkIn);
        IO_PB = 5'b00111;
        IO_Switch = `MFP_N_SW'h1234;
        repeat (100)  @(posedge SI_ClkIn);
        IO_Switch = `MFP_N_SW'h0011;
        repeat (100)  @(posedge SI_ClkIn);
        IO_Switch = `MFP_N_SW'hffff;
        repeat (100)  @(posedge SI_ClkIn);
        IO_Switch = `MFP_N_SW'haaaa;
		repeat (100)  @(posedge SI_ClkIn);

        //$stop;
    end

    initial
    begin
        $dumpvars;
        $timeformat (-9, 1, "ns", 10);
    end
    
endmodule


