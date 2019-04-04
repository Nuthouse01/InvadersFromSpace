// icon.v
// February 22, 2019
//
// Bradon Kanyid, Grant Vesely
//

module icon(
    input        clk,
    input [11:0] x_pix, y_pix,

    output [9:0] x, // used for sorting

    output [11:0] sprite_addr,
    input  [1:0]  ds,
    input  [31:0] data,
    
    output pixel_active,
    output [3:0] pixel_data,
  
    input  sprite_table_wr,
    input  [1:0]  sprite_table_addr,
    input  [31:0] sprite_table_di
);

    // sprite table data
    reg [9:0] x_pos;
    reg [9:0] y_pos;
    reg [4:0] index;    

    // register used to store pixel data for current line
    reg [31:0] data0;
    reg [31:0] data1;
    
    always @(posedge clk) begin
       if(ds[0]) data0 <= data;
       if(ds[1]) data1 <= data;
    end

    wire v_visible = (y_pix >= y_pos) && (y_pix < y_pos + 10'd16);
    wire visible = v_visible && (x_pix >= x_pos) && (x_pix < x_pos + 10'd16);

    assign x = v_visible ? x_pos : 10'd640;

    wire [3:0] col = x_pix - x_pos;
    wire [3:0] row = y_pix - y_pos;

    assign sprite_addr = {index, row, x_pix[2], 2'b00}; // index, then the row, then the even/odd word within the 2 words that make up a line

    assign pixel_data =
      (col ==  0) ? data0[31:28] :
      (col ==  1) ? data0[27:24] :
      (col ==  2) ? data0[23:20] :
      (col ==  3) ? data0[19:16] :
      (col ==  4) ? data0[15:12] :
      (col ==  5) ? data0[11: 8] :
      (col ==  6) ? data0[ 7: 4] :
      (col ==  7) ? data0[ 3: 0] :
      (col ==  8) ? data1[31:28] :
      (col ==  9) ? data1[27:24] :
      (col == 10) ? data1[23:20] :
      (col == 11) ? data1[19:16] :
      (col == 12) ? data1[15:12] :
      (col == 13) ? data1[11: 8] :
      (col == 14) ? data1[ 7: 4] :
      (col == 15) ? data1[ 3: 0] : 4'b0000;

    assign pixel_active = (pixel_data != 0) && visible;

    always @(posedge clk) begin
       if(sprite_table_wr) begin
          case(sprite_table_addr)
             0: x_pos <= sprite_table_di;
             1: y_pos <= sprite_table_di;
             2: index <= sprite_table_di;
          endcase
       end
    end

endmodule
