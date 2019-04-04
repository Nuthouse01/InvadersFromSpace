// mfp_ahb_sprite_ram.v
//
// Small RAM holding sprite/video ram

`timescale 100ps/1ps

`include "mfp_ahb_const.vh"

module mfp_ahb_sprite_ram
(
    input               HCLK,
    input               HRESETn,
    input      [ 31: 0] HADDR,
    input      [  2: 0] HBURST,
    input               HMASTLOCK,
    input      [  3: 0] HPROT,
    input      [  2: 0] HSIZE,
    input      [  1: 0] HTRANS,
    input      [ 31: 0] HWDATA,
    input               HWRITE,
    output     [ 31: 0] HRDATA,
	input               HSEL,
    
    // secondary interface for sprite manager
    input      [ 11: 0] SPRITE_ADDR,
    output     [ 31: 0] SPRITE_DATA
);

  reg  [31:0] HADDR_d;
  reg         HWRITE_d;
  reg         HSEL_d;
  reg  [1:0]  HTRANS_d;
  wire        we;            // write enable
  reg  [3:0]  we_mask;       // sub-word write mask

  // delay HADDR, HWRITE, HSEL, and HTRANS to align with HWDATA for writing
  always @ (posedge HCLK) 
  begin
    HADDR_d  <= HADDR;
	HWRITE_d <= HWRITE;
	HSEL_d   <= HSEL;
	HTRANS_d <= HTRANS;
  end
  
  // overall write enable signal
  assign we = (HTRANS_d != `HTRANS_IDLE) & HSEL_d & HWRITE_d;

  // determine byte/half-word/word write enables
  always @ (*)
  begin
      if (~we)
          we_mask = 4'b0000;										// not writing
      else if (HBURST == `HBURST_SINGLE && HSIZE == `HSIZE_1)   // byte write
          we_mask = 4'b0001 << HADDR [1:0];
      else if (HBURST == `HBURST_SINGLE && HSIZE == `HSIZE_2)   // half-word write
          we_mask = HADDR [1] ? 4'b1100 : 4'b0011;
      else														// word write
          we_mask = 4'b1111;
  end

  // 4 byte-wide RAMs
  sprite_ram #(8, `H_RAM_SPRITE_ADDR_WIDTH) ram_sprite0
  (
    .clk(HCLK),
    
    .data1(HWDATA[7:0]),
    .read_addr1(HADDR[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .write_addr1(HADDR_d[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .we1(we_mask[0]),
    .q1(HRDATA[7:0]),

    .read_addr2(SPRITE_ADDR[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .q2(SPRITE_DATA[7:0])
  );

  sprite_ram #(8, `H_RAM_SPRITE_ADDR_WIDTH) ram_sprite1
  (
    .clk(HCLK),

    .data1(HWDATA[15:8]),
    .read_addr1(HADDR[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .write_addr1(HADDR_d[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .we1(we_mask[1]),
    .q1(HRDATA[15:8]),

    .read_addr2(SPRITE_ADDR[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .q2(SPRITE_DATA[15:8])
  );

  sprite_ram #(8, `H_RAM_SPRITE_ADDR_WIDTH) ram_sprite2
  (
    .clk(HCLK),

    .data1(HWDATA[23:16]),
    .read_addr1(HADDR[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .write_addr1(HADDR_d[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .we1(we_mask[2]),
    .q1(HRDATA[23:16]),

    .read_addr2(SPRITE_ADDR[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .q2(SPRITE_DATA[23:16])
  );

  sprite_ram #(8, `H_RAM_SPRITE_ADDR_WIDTH) ram_sprite3
  (
    .clk(HCLK),
    
    .data1(HWDATA[31:24]),
    .read_addr1(HADDR[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .write_addr1(HADDR_d[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .we1(we_mask[3]),
    .q1(HRDATA[31:24]),

    .read_addr2(SPRITE_ADDR[(`H_RAM_SPRITE_ADDR_WIDTH+1):2]),
    .q2(SPRITE_DATA[31:24])
  );

endmodule