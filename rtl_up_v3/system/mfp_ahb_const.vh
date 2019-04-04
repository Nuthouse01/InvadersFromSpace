// 
// mfp_ahb_const.vh
//
// Verilog include file with AHB definitions
// 

//---------------------------------------------------
// Physical bit-width of memory-mapped I/O interfaces
//---------------------------------------------------
`define MFP_N_LED             16  // number of basic LEDs
`define MFP_N_SW              16  // number of switches
`define MFP_N_PB              5+2   // number of buttons

`define MFP_N_7SD_DIG         8   // 7SD number of digits (anodes)
`define MFP_N_7SD_SEG         8   // 7SD number of segments per digit (cathodes)

`define BUTT_UP_IDX			4
`define BUTT_DOWN_IDX		3
`define BUTT_LEFT_IDX		2
`define BUTT_CENTER_IDX		1
`define BUTT_RIGHT_IDX		0


//---------------------------------------------------
// Memory-mapped I/O addresses for each register
// note: these aren't actually used in the code anywhere, except this file
// highest valid: 32'h1fb00000
// lowest valid:  32'h11000000
// NOTE: ahb modules located OUTSIDE the hierarchy must begin with H_OUTER_AHB_Match
//---------------------------------------------------
`define H_LED_ADDR        (32'h1f800000)
`define H_SW_ADDR       (32'h1f800004)
`define H_PB_ADDR       (32'h1f800008)

`define H_7SD_DIGEN_ADDR    (32'h1f700000)
`define H_7SD_LSEG_ADDR     (32'h1f700004)
`define H_7SD_HSEG_ADDR     (32'h1f700008)
`define H_7SD_DP_ADDR     (32'h1f70000C)

`define H_TIMER0_ADDR     (32'h1f600000)
`define H_TIMER1_ADDR     (32'h1f600004)
`define H_RAND_ADDR       (32'h1f600010)

// rojobot has been removed, these addresses are now unused
`define H_ROJO_BOTINFO_ADDR   (32'h1f500000)
`define H_ROJO_MTRCTRL_ADDR   (32'h1f500004)
`define H_ROJO_SYNCH_ADDR   (32'h1f500008)
`define H_ROJO_ACK_ADDR     (32'h1f50000C)
`define H_ROJO_CONFIG_ADDR    (32'h1f500010)

// xadc
`define H_XADC_RESULTA_ADDR   (32'h1f400000)
`define H_XADC_RESULTB_ADDR   (32'h1f400004)
// audio
`define H_SOUND_SOUNDFX_ADDR (32'h1f300000)
`define H_SOUND_TONEGEN_ADDR (32'h1f300004)
`define H_SOUND_STATUS_ADDR (32'h1f300008)

`define H_SPRITE_GFX_ADDR (32'h1f200000)
`define H_SPRITE_TBL_ADDR (32'h1f100000)


// Define the bits [5:2] of the address for each register, used within each module
// it makes more sense to me to define these in terms of their full reg addresses... resulting values are unchanged
// because of masking by 8'hF I can have up to 256 regs inside one module... should be plenty
`define H_LED_IONUM       (8'hFF & (`H_LED_ADDR>>2))    // 0
`define H_SW_IONUM        (8'hFF & (`H_SW_ADDR>>2))   // 1
`define H_PB_IONUM        (8'hFF & (`H_PB_ADDR>>2))   // 2

`define H_7SD_DIGEN_IONUM   (8'hFF & (`H_7SD_DIGEN_ADDR>>2))// 0
`define H_7SD_LSEG_IONUM    (8'hFF & (`H_7SD_LSEG_ADDR>>2)) // 1
`define H_7SD_HSEG_IONUM    (8'hFF & (`H_7SD_HSEG_ADDR>>2)) // 2
`define H_7SD_DP_IONUM      (8'hFF & (`H_7SD_DP_ADDR>>2)) // 3

`define H_TIMER0_IONUM      (8'hFF & (`H_TIMER0_ADDR>>2)) // 0
`define H_TIMER1_IONUM      (8'hFF & (`H_TIMER1_ADDR>>2)) // 1
`define H_RAND_IONUM      (8'hFF & (`H_RAND_ADDR>>2))   // 4

`define H_ROJO_BOTINFO_IONUM  (8'hFF & (`H_ROJO_BOTINFO_ADDR>>2)) // 0
`define H_ROJO_MTRCTRL_IONUM  (8'hFF & (`H_ROJO_MTRCTRL_ADDR>>2)) // 1
`define H_ROJO_SYNCH_IONUM    (8'hFF & (`H_ROJO_SYNCH_ADDR>>2)) // 2
`define H_ROJO_ACK_IONUM    (8'hFF & (`H_ROJO_ACK_ADDR>>2))   // 3
`define H_ROJO_CONFIG_IONUM   (8'hFF & (`H_ROJO_CONFIG_ADDR>>2))  // 4

`define H_XADC_RESULTA_IONUM  (8'hFF & (`H_XADC_RESULTA_ADDR>>2)) // 0
`define H_XADC_RESULTB_IONUM  (8'hFF & (`H_XADC_RESULTB_ADDR>>2)) // 1

`define H_SOUND_SOUNDFX_IONUM  (8'hFF & (`H_SOUND_SOUNDFX_ADDR>>2)) // 0
// write 1<<1 = player explode sound
// write 1<<2 = player shoot sound
// write 1<<3 = enemy death sound
// write 1<<4 = wavestart (maybe)
// write 1<<5 = gameover (maybe)
`define H_SOUND_TONEGEN_IONUM  (8'hFF & (`H_SOUND_TONEGEN_ADDR>>2)) // 1
// 1 = be playing music
// 0 = pause the music
`define H_SOUND_STATUS_IONUM  (8'hFF & (`H_SOUND_STATUS_ADDR>>2)) // 2
// wave number

//---------------------------------------------------
// Address matching constants
// these constants are the minimum unique portion of the address that is used to determine HSEL
//---------------------------------------------------
// todo: standardize these lengths & offsets... perhaps make them all the same length with ???? before/after?
// `define H_RAM_RESET_ADDR_Match (7'h7f)  // =...1_1111_11.. = 1f(c/d/e/f)
// `define H_RAM_ADDR_Match     (1'b0)   // =...0_...._....
// `define H_LED_ADDR_Match     (7'h7e)  // =...1_1111_10.. = 1f(8/9/a/b), but it should match only 8
// `define H_7SD_ADDR_Match     (9'h1f7) // =...1_1111_0111

`define H_RAM_RESET_ADDR_Match      (9'b1_1111_11??) // =0x1f(c/d/e/f)
`define H_RAM_ADDR_Match            (9'b0_????_????) // =0x0???????
`define H_LED_ADDR_Match            (9'b1_1111_1000) // =0x1f8?????
`define H_7SD_ADDR_Match            (9'b1_1111_0111) // =0x1f7?????
`define H_TIMER_ADDR_Match          (9'b1_1111_0110) // =0x1f6?????
`define H_ROJO_ADDR_Match           (9'b1_1111_0101) // =0x1f5?????
`define H_XADC_ADDR_Match           (9'b1_1111_0100) // =0x1f4?????
`define H_SOUND_ADDR_Match          (9'b1_1111_0011) // =0x1f3?????
`define H_SPRITE_GFX_ADDR_Match     (9'b1_1111_0010) // =0x1f2?????
`define H_SPRITE_TBL_ADDR_Match     (9'b1_1111_0001) // =0x1f1?????
`define H_OUTER_AHB_Match           (5'b1_0000)      // aligned with the others

//---------------------------------------------------
// RAM addresses...?
// !!  DO NOT CHANGE  !!
//---------------------------------------------------
`define H_RAM_RESET_ADDR        (32'h1fc?????)
`define H_RAM_ADDR              (32'h0???????)
`define H_RAM_SPRITE_ADDR       (32'h1f2?????)
`define H_RAM_SPRITE_TBL_ADDR   (32'h1f1?????)
`define H_RAM_RESET_ADDR_WIDTH  (8)
`define H_RAM_ADDR_WIDTH        (16)
`define H_RAM_SPRITE_ADDR_WIDTH (10)
`define H_RAM_SPRITE_TBL_ADDR_WIDTH (9)


//---------------------------------------------------
// AHB-Lite values used by MIPSfpga core
// !!  DO NOT CHANGE  !!
//---------------------------------------------------

`define HTRANS_IDLE    2'b00
`define HTRANS_NONSEQ  2'b10
`define HTRANS_SEQ     2'b11

`define HBURST_SINGLE  3'b000
`define HBURST_WRAP4   3'b010

`define HSIZE_1        3'b000
`define HSIZE_2        3'b001
`define HSIZE_4        3'b010
