/*
 * Seven-segment display decoder
 *
 * January 25, 2017
 *
 * On August 9th, 2017
 * Modified by Srivatsa Yogendra to include the decimal points and to 
 * display special characters on the seven segment display.
 *
 * Feb 1, 2019
 * Brian Henson & Jamie Williams: added better comments.
 * Also renamed to "mfp_ahb_7sd_decoder"
 *
 */

module mfp_ahb_7sd_decoder(input      [5:0] data,
                           output reg [7:0] seg);
// data: 6 bits, first 5 count 0-31 and correspond to programmed outputs, bit 6 is decimal point (active low)
// seg: 8 bits, 7 for the 7 segments + 1 for decimal point
// this module is nothing but combinational logic
// NOTE: we could potentially remove this block from the signal path and have manual control over the segments, if we need to turn the segments on in some pattern not represented here... would require more assembly code tho
// NOTE: we may want to tweak the patterns used to display some of the numbers/letters, this is where its done

localparam seg_a = 7'b0111111;
localparam seg_b = 7'b1011111;
localparam seg_c = 7'b1101111;
localparam seg_d = 7'b1110111;
localparam seg_e = 7'b1111011;
localparam seg_f = 7'b1111101;
localparam seg_g = 7'b1111110;
localparam upH = 7'b1001000;	// capital H
localparam upL = 7'b1110001;	// capital L
localparam upR = 7'b0001000;	// capital R
localparam lol = 7'b1111001;	// lowercase l
localparam lor = 7'b1111010;	// lowercase r

localparam  blank = 7'b1111111;

// combinational logic to translate input signal to output
always @(data)
begin
	case (data[4:0])
		// 0-15 = hex digits: 0-9, a-f
		5'd00: seg = {data[5],7'h01};
		5'd01: seg = {data[5],7'h4f};
		5'd02: seg = {data[5],7'h12};
		5'd03: seg = {data[5],7'h06};
		5'd04: seg = {data[5],7'h4c};
		5'd05: seg = {data[5],7'h24};
		5'd06: seg = {data[5],7'h20};
		5'd07: seg = {data[5],7'h0f};
		5'd08: seg = {data[5],7'h00};
		5'd09: seg = {data[5],7'h0c};
		5'd10: seg = {data[5],7'h08};
		5'd11: seg = {data[5],7'h60};
		5'd12: seg = {data[5],7'h72};
		5'd13: seg = {data[5],7'h42};
		5'd14: seg = {data[5],7'h30};
		5'd15: seg = {data[5],7'h38};
		// 16-22 = individual segments (see diagram)
		5'd16: seg = {data[5],seg_a};
        5'd17: seg = {data[5],seg_b};
        5'd18: seg = {data[5],seg_c};
        5'd19: seg = {data[5],seg_d};
        5'd20: seg = {data[5],seg_e};
        5'd21: seg = {data[5],seg_f};
        5'd22: seg = {data[5],seg_g};
		// 23-27 = these 5 characters, not yet sure what for
        5'd23: seg = {data[5],upH};
        5'd24: seg = {data[5],upL};
        5'd25: seg = {data[5],upR};
        5'd26: seg = {data[5],lol};
        5'd27: seg = {data[5],lor};
		// 28-31 = blank
        default: seg = {data[5],blank};
	endcase
		
end
	
endmodule
