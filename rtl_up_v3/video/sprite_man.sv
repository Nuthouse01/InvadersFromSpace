module sprite_man(
      input         clk,
      input  [11:0] pix_row, pix_col,

      output pixel_active,
      output [3:0] pixel_data,

      input [3:0] sprite_index,
      output [11:0] sprite_addr,
      input [1:0] sprite_graphics_data_valid,
      input [31:0] sprite_graphics_di,

      input  sprite_table_wr,
      input  [8:0]  sprite_table_addr,
      input  [31:0] sprite_table_di
    );

    localparam SPRITES = 128;
    localparam SPRITE_IDX_BITS = 7;
    localparam SPRITE_X_MAX = 640;
    localparam SPRITE_X_BITS = 10;

    // sprites have left-to-right priority
    // and the leftmost 16 are being shown. 
    // They need to be sorted
    wire [(SPRITES*SPRITE_X_BITS)-1:0] sprite_x;
    wire [(SPRITES*SPRITE_IDX_BITS)-1:0] sprite_idx;

    // Sprite sorter is parameterized to support different number of sprites/widths
    sprite_sort #(.SPRITES(SPRITES), .IDX_BITS(SPRITE_IDX_BITS), .X_BITS(SPRITE_X_BITS)) sprite_sort (
      .clk   ( clk        ),
      .load  ( pix_col == 12'd640 ), // start a load directly after the line is drawn: 640 is first off-screen pixel. 0-639 are shown indexes
      .x     ( sprite_x   ),         // 1-d overlayed array made up of the generated icons x positions
      .idx   ( sprite_idx )          // 1-d overlayed array that is broken up in the generated icons into individual outputs
    );

    wire [SPRITES-1:0] sprite_pixel_active;
    wire [3:0] sprite_pixel_data [SPRITES-1:0];

    wire [11:0] sprites_addr [SPRITES-1:0];

    // address where the sprite wants to read data from
    // sprite_idx_array is the sorted index array.
    //  i.e. requesting sprite_idx_array[0] gives the left-most index
    //  sprites_addr is mapped onto in the generate statement for all the outputs from the icons
    wire [SPRITE_IDX_BITS-1:0] sprite_idx_array [SPRITES-1:0];
    wire [SPRITE_IDX_BITS-1:0] prio_index = sprite_idx_array[sprite_index];
    assign sprite_addr = sprites_addr[prio_index];

    // this generates the 128 sprite instances that are managed
    // there are some tricks to overlay the individual results 
    // into an array that is indexable outside the generate statement
    genvar index;
    generate
    for (index=0; index < SPRITES; index=index+1)  begin: icons  
        assign sprite_idx_array[index] = sprite_idx[SPRITE_IDX_BITS*index+(SPRITE_IDX_BITS-1):SPRITE_IDX_BITS*index];

        icon icon(
          .clk(clk),
          .x_pix(pix_col),
          .y_pix(pix_row),

          // this is the output x value that is used by the sprite sorter. These are put into a large
          // 1-d array, which makes it easy to pass into the sprite_sort module.
          .x(sprite_x[(SPRITE_X_BITS*index)+(SPRITE_X_BITS-1):(SPRITE_X_BITS*index)] ),

          // this is to update the two 32-bit words that make up a horizontal line of a sprite
          // these are refreshed during every HBLANK, if they are going to be shown in the next line
          .sprite_addr(sprites_addr[index]),
          .ds( (prio_index == index) ? sprite_graphics_data_valid : 2'b00),
          .data( sprite_graphics_di ),
    
          // this is the individual pixel output for an icon
          // these are used by the simple multiplexers below
          // the generate statement
          .pixel_active (sprite_pixel_active[index]),
          .pixel_data   (sprite_pixel_data[index]),

          // this is passed in from the top-level, and slightly decoded
          // to address the appropriate icon, to update it's sprite table entries
          .sprite_table_wr(sprite_table_wr && (sprite_table_addr[8:2] == index) ),
          .sprite_table_addr(sprite_table_addr[1:0]),
          .sprite_table_di(sprite_table_di)
        );
      end  
    endgenerate

    // easier way to reference the 16 sprites that could be shown on a line.
    wire [(SPRITE_IDX_BITS-1):0] spr0 = sprite_idx_array[0];
    wire [(SPRITE_IDX_BITS-1):0] spr1 = sprite_idx_array[1];
    wire [(SPRITE_IDX_BITS-1):0] spr2 = sprite_idx_array[2];
    wire [(SPRITE_IDX_BITS-1):0] spr3 = sprite_idx_array[3];
    wire [(SPRITE_IDX_BITS-1):0] spr4 = sprite_idx_array[4];
    wire [(SPRITE_IDX_BITS-1):0] spr5 = sprite_idx_array[5];
    wire [(SPRITE_IDX_BITS-1):0] spr6 = sprite_idx_array[6];
    wire [(SPRITE_IDX_BITS-1):0] spr7 = sprite_idx_array[7];
    wire [(SPRITE_IDX_BITS-1):0] spr8 = sprite_idx_array[8];
    wire [(SPRITE_IDX_BITS-1):0] spr9 = sprite_idx_array[9];
    wire [(SPRITE_IDX_BITS-1):0] sprA = sprite_idx_array[10];
    wire [(SPRITE_IDX_BITS-1):0] sprB = sprite_idx_array[11];
    wire [(SPRITE_IDX_BITS-1):0] sprC = sprite_idx_array[12];
    wire [(SPRITE_IDX_BITS-1):0] sprD = sprite_idx_array[13];
    wire [(SPRITE_IDX_BITS-1):0] sprE = sprite_idx_array[14];
    wire [(SPRITE_IDX_BITS-1):0] sprF = sprite_idx_array[15];

    // if any of these is active then the current pixel is being driven by
    // the sprite engine
    assign pixel_active =
      sprite_pixel_active[spr0] ||
      sprite_pixel_active[spr1] ||
      sprite_pixel_active[spr2] ||
      sprite_pixel_active[spr3] ||
      sprite_pixel_active[spr4] ||
      sprite_pixel_active[spr5] ||
      sprite_pixel_active[spr6] ||
      sprite_pixel_active[spr7] ||
      sprite_pixel_active[spr8] ||
      sprite_pixel_active[spr9] ||
      sprite_pixel_active[sprA] ||
      sprite_pixel_active[sprB] ||
      sprite_pixel_active[sprC] ||
      sprite_pixel_active[sprD] ||
      sprite_pixel_active[sprE] ||
      sprite_pixel_active[sprF];

    // get the pixel information of the leftmost sprite
    assign pixel_data =
      sprite_pixel_active[spr0]?sprite_pixel_data[spr0]:
      sprite_pixel_active[spr1]?sprite_pixel_data[spr1]:
      sprite_pixel_active[spr2]?sprite_pixel_data[spr2]:
      sprite_pixel_active[spr3]?sprite_pixel_data[spr3]:
      sprite_pixel_active[spr4]?sprite_pixel_data[spr4]:
      sprite_pixel_active[spr5]?sprite_pixel_data[spr5]:
      sprite_pixel_active[spr6]?sprite_pixel_data[spr6]:
      sprite_pixel_active[spr7]?sprite_pixel_data[spr7]:
      sprite_pixel_active[spr8]?sprite_pixel_data[spr8]:
      sprite_pixel_active[spr9]?sprite_pixel_data[spr9]:
      sprite_pixel_active[sprA]?sprite_pixel_data[sprA]:
      sprite_pixel_active[sprB]?sprite_pixel_data[sprB]:
      sprite_pixel_active[sprC]?sprite_pixel_data[sprC]:
      sprite_pixel_active[sprD]?sprite_pixel_data[sprD]:
      sprite_pixel_active[sprE]?sprite_pixel_data[sprE]:
      sprite_pixel_active[sprF]?sprite_pixel_data[sprF]:
      2'b00;

endmodule
