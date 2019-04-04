// mfp_nexys4_ddr.v
// January 1, 2017
//
// Instantiate the mipsfpga system and rename signals to
// match the GPIO, LEDs and switches on Digilent's (Xilinx)
// Nexys4 DDR board

// Outputs:
// 16 LEDs (IO_LED)
// 7-Seg display
// VGA display 
// Inputs:
// 16 Slide switches (IO_Switch),
// 5 Pushbuttons (IO_PB): {BTNU, BTND, BTNL, BTNC, BTNR}
// 2 Paddle buttons
// 2 Paddle analog positions (XADC)
//

`include "mfp_ahb_const.vh"

module mfp_nexys4_ddr(
                        input                   CLK100MHZ,
                        input                   CPU_RESETN,
                        input                   BTNU, BTND, BTNL, BTNC, BTNR,
                        input  [`MFP_N_SW-1 :0] SW,
                        output [`MFP_N_LED-1:0] LED,
                        inout  [ 8          :1] JB,
                        input                   UART_TXD_IN,
                        output [7:0]            AN, // anode output for 7seg (digit enable)
                        output                  CA, CB, CC, CD, CE, CF, CG, // all segment cathodes
                        output                  DP, // decimal point cathode
                        output [3:0]            VGA_R, VGA_G, VGA_B,
                        output                  VGA_HS, VGA_VS,
                        // sound pin and audio enable
                        output					AUD_PWM, AUD_SD,
                        // paddle buttons
                        input                   PADDLE_A_BTN, PADDLE_B_BTN,
                        // Analog inputs for XADC
                        input                   vauxp2, vauxn2,
                        input                   vauxp3, vauxn3,
                        input                   vp_in, vn_in
                        );

    // Press btnCpuReset to reset the processor.

    /*
    DONE:
    clock wizard
    made MIPSfpga cpu core an out-of-context run (not sure if that will be preserved by the "recreate_project.tcl" script)
    remove rojobot signals passed up thru hierarchy
    remove rojobot modules: ahb, synchro
    update AHB tree/signals so we can read ahb things outside the top level
    ahb XADC module
    ahb sprite table storage module
    vga module
    sprite controller
    expand debounce module to accept additional paddle buttons
    expand signals passed thru hierarchy so addl buttons are still held by the GPIO ahb module
    placeholder ahb sound module
    sprite RAM 
    */

    /*
    TODO:
    sound ROM (or maybe some other method of storage?)
    change pin constraints to let out the sound pins
    */

    wire clk25;
    wire tck_in, tck;
    wire [ 4:0] pushbutton_debounced;
    wire        resetbutton_debounced;
    wire [15:0] switches_debounced;
    wire [1:0]  controller_buttons_debounced;

    wire [31:0] HADDR, HWDATA, HRDATA;
    wire [1:0]  HTRANS;
    wire        HWRITE;

    wire [23:0] xadc_results;

    clk_wiz_0 clk_wiz_0(.clk_in_100mhz(CLK100MHZ), .clk_out_25(clk25));

    IBUF IBUF1(.O(tck_in),.I(JB[4]));
    BUFG BUFG1(.O(tck), .I(tck_in));

    debounce #(.NUM_BUTTONS(8), .NUM_SWITCHES(16)) debounce(
                    .clk(clk25),
                    .pbtn_in({~PADDLE_A_BTN, ~PADDLE_B_BTN, BTNU, BTND, BTNL, BTNC, BTNR, CPU_RESETN}),
                    .pbtn_db({controller_buttons_debounced, pushbutton_debounced, resetbutton_debounced}),
                    .switch_in(SW),
                    .switch_db(switches_debounced)
                    );

    wire [11:0] sprite_addr;
    wire [31:0] sprite_data;

    mfp_sys mfp_sys(
                    .SI_Reset_N(resetbutton_debounced),
                    .SI_ClkIn(clk25),
                    .HADDR(HADDR),
                    .HRDATA(HRDATA), // CHANGED: was output, now input
                    .HWDATA(HWDATA),
                    .HWRITE(HWRITE),
                    .HSIZE(), // not used
                    .HTRANS(HTRANS), // NEW
                    .EJ_TRST_N_probe(JB[7]),
                    .EJ_TDI(JB[2]),
                    .EJ_TDO(JB[3]),
                    .EJ_TMS(JB[1]),
                    .EJ_TCK(tck),
                    .SI_ColdReset_N(JB[8]),
                    .EJ_DINT(1'b0),
                    .IO_Switch(switches_debounced),
                    .IO_PB({controller_buttons_debounced, pushbutton_debounced}),
                    .IO_LED(LED),
                    .UART_RX(UART_TXD_IN),
                    .OUT_7SD_ANODE(AN),
                    .OUT_7SD_CATHODE({DP, CA, CB, CC, CD, CE, CF, CG}),
                    .IN_analog_result ( xadc_results ), // {resultB, resultA} concatenated into one signal
                    .OUT_audio_pwm_and_enable({AUD_PWM, AUD_SD}), // [0]=AUD_PWM, [1]=AUD_SD
                    .IN_audio_clock(CLK100MHZ),
                    // sprite ram access via sprite manager
                    .SPRITE_ADDR(sprite_addr),
                    .SPRITE_DATA(sprite_data)
                    );

    // Handles the sampling of the 2 paddle controllers' positional data
    // inputs: 
    //   vauxp2 and vauxp3 from XADC PMOD connector on board
    // outputs:
    //   packed 24-bit value made up of 12-bit position 1 and 12-bit position 2

    top_xadc_stuff top_xadc_stuff(
                .clk(clk25),
                .resetn(resetbutton_debounced),
                .vauxp2(vauxp2), .vauxn2(vauxn2),
                .vauxp3(vauxp3), .vauxn3(vauxn3),
                .vp_in(vp_in), .vn_in(vn_in),
                .xadc_results(xadc_results)
                );

    wire        video_on;
    wire [11:0] pix_row, pix_col;
    wire hsync, vsync;

    dtg dtg(
      // inputs
      .clock(clk25),
      .rst(~resetbutton_debounced),
      // outputs
      .horiz_sync(hsync),
      .vert_sync(vsync),
      .video_on(video_on),
      .pixel_row(pix_row),
      .pixel_column(pix_col));

    assign VGA_HS = hsync;
    assign VGA_VS = vsync;

    wire [3:0] sprite_pixel_data;
    wire sprite_pixel_active;

    localparam SORT_TIME = 12'd64;
    localparam SORT_TIME_16 = SORT_TIME/16;
    localparam POST_SORT = 12'd640 + SORT_TIME;

  // start generating indexes through the sorted sprite table, right after sorting is done
  // if not in HBLANK, post-sort, then just index sprite 16 (never shown)
  wire [3:0] sprite_index = (pix_col > POST_SORT ? pix_col[6:3] - 8 : 16);

  // not really necessary, but give a few cycles between address request and reading the data
  // into the requesting sprite line
  wire [1:0] sprite_dvalid = {
     (pix_col[2:0] == 3'h7) && (pix_col > POST_SORT),   // this signal goes high when the 2nd word is ready
     (pix_col[2:0] == 3'h3) && (pix_col > POST_SORT)};  // this signal goes high when the 1st word is ready

  wire sprite_table_wr;
  wire [ 8: 0] sprite_table_addr;
  wire [31: 0] sprite_table_di;

  // this decoder snoops the AHB signals between the CPU and the sprite table ram, and 
  // generates the appropriate signals to write the registers within each icon instance
  // as necessary. 
  mfp_to_sprite_table_decoder sprite_table_decoder(
        // inputs from the CPU memory write
        .CLK(clk25),
        .HADDR(HADDR),
        .HWDATA(HWDATA),
        .HWRITE(HWRITE),
        .HTRANS(HTRANS),

        // outputs to the sprite table
        .SPRITE_TABLE_WR(sprite_table_wr),
        .SPRITE_TABLE_ADDR(sprite_table_addr),
        .SPRITE_TABLE_DI(sprite_table_di)
    );

  // extremely simple background generator. 
  // Take the 4-bit value of the y position of the current line being shown, 
  // and use that as a grayscale RGB value
  wire [11:0] background_color;
  
  assign background_color = { pix_row[8:5], pix_row[8:5], pix_row[8:5] };
  
  // This beast manages all the icons, and does the sprite multiplexing as well.
  // Inputs:
  //   pixel clock, pixel x/y positoin
  //   signals to fetch the two words that make up a horizontal line of a sprite,
  //     priority indexed by sprite_index. These are refreshed after sprite sorting,
  //     prior to the line being shown.
  //   signals to write the sprite table values (x, y, palette index)
  // Outputs:
  //   Multiplexed pixel value
  //   Multiplexed pixel active signal (easier way to determine if sprite or background is shown)

  sprite_man sprite_man(
    // simple inputs to know which pixel we're generating on the screen
    .clk(clk25),
    .pix_row(pix_row),
    .pix_col(pix_col),

    // these are the outputs that go to the colorizer
    .pixel_active(sprite_pixel_active),
    .pixel_data(sprite_pixel_data),

    // interface to refresh sprite pixel data for the sprites that will be shown during the next line
    // each icon instance only keeps the two words that make up the 16 pixels to show for that line
    .sprite_index(sprite_index),
    .sprite_addr(sprite_addr),
    .sprite_graphics_data_valid(sprite_dvalid),
    .sprite_graphics_di(sprite_data),

    // hooked up to the CPU indirectly via mfp_to_sprite_table_decoder
    .sprite_table_wr(sprite_table_wr),
    .sprite_table_addr(sprite_table_addr),
    .sprite_table_di(sprite_table_di)
  );

  // simple interface to generate the proper RGB value for the VGA pins
  // contains a 16-color fixed palette. Would have liked to expose this to MIPS software. Oh well...
  colorizer colorizer(
    .clk(clk25),
    .video_on(video_on),
    .world_pixel(background_color),
    .sprite_pixel(sprite_pixel_data),
    .sprite_pixel_active(sprite_pixel_active),

    .r(VGA_R),
    .g(VGA_G),
    .b(VGA_B)
  );

endmodule
