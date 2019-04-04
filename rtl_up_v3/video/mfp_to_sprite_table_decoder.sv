`include "../system/mfp_ahb_const.vh"

module mfp_to_sprite_table_decoder(
        // inputs from the CPU memory write
        input                   CLK,
        input  [ 31: 0]         HADDR, HWDATA,
        input                   HWRITE,
        input  [  1: 0]         HTRANS,

        // outputs to the sprite table
        output                  SPRITE_TABLE_WR,
        output reg [ 8:0]       SPRITE_TABLE_ADDR,
        output reg [31:0]       SPRITE_TABLE_DI
    );


  reg [10:0]  HADDR_d;
  reg         HWRITE_d;
  reg         HSEL_d;

  wire   HSEL;
  
  assign HSEL = (HADDR[28:20] ==? `H_SPRITE_TBL_ADDR_Match);  // sprite table ram

  // delay HADDR, HWRITE, HSEL, and HTRANS to align with HWDATA for writing
  always @ (posedge CLK)
  begin
    HADDR_d  <= HADDR[10:2];
    HWRITE_d <= HWRITE;
    HSEL_d   <= HSEL;
  end

  assign SPRITE_TABLE_WR = HSEL_d & HWRITE_d;
  
  // write
  always @(posedge CLK) begin
    SPRITE_TABLE_ADDR <= SPRITE_TABLE_ADDR;
    SPRITE_TABLE_DI <= SPRITE_TABLE_DI;

    if (SPRITE_TABLE_WR) begin
         SPRITE_TABLE_ADDR <= HADDR_d;
         SPRITE_TABLE_DI <= HWDATA;
    end
  end

endmodule
