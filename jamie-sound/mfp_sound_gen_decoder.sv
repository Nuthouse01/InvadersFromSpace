// mfp_sound_gen_decoder.sv

// Used to get game information from memory and enable/initiate sound
// Currently only creating a music enable

`include "mfp_ahb_const.vh" //../rtl_up_v3/system/


module mfp_sound_gen_decoder(
        // inputs from the CPU memory write
        input                   CLK,
        input  [ 31: 0]         HADDR, HWDATA,
        input                   HWRITE,
        input  [  1: 0]         HTRANS,

        // outputs to the sprite table
        output reg                 SOUND_MUSIC_EN // goes to music enable on sound_generator.v
    );

  reg [ 8:0]       SOUND_ADDR;
  reg [31:0]       SOUND_DI;
  reg [10:0]  HADDR_d;
  reg         HWRITE_d;
  reg         HSEL_d;

  wire   HSEL;
  
  assign HSEL = (HADDR[28:20] ==? `H_SOUND_ADDR_Match);  // sprite table ram

  // delay HADDR, HWRITE, HSEL, and HTRANS to align with HWDATA for writing
  always @ (posedge CLK)
  begin
    HADDR_d  <= HADDR[10:2];
    HWRITE_d <= HWRITE;
    HSEL_d   <= HSEL;
  end

  assign SOUND_MUSIC_WR = HSEL_d & HWRITE_d;
  
  // write
  always @(posedge CLK) begin
    SOUND_ADDR <= SOUND_ADDR;
    SOUND_DI <= SOUND_DI;

    if (SOUND_MUSIC_WR) begin
         SOUND_ADDR <= HADDR_d;
         SOUND_DI <= HWDATA;
    end
    
	// If the data at this location is non-zero, music is enabled
    if(SOUND_ADDR == `H_SOUND_MUSICSTATE_IONUM)
    begin
        if(SOUND_DI > 0)
            SOUND_MUSIC_EN <= 1;
        else
            SOUND_MUSIC_EN <= 0;
    end
  end

endmodule
