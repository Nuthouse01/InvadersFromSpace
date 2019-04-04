// colorizer.v
// February 22, 2019
//
// Bradon Kanyid, Grant Vesely
//
// Inputs:
//   clock
//   video_on: determines if the current display x,y position is in the visible region.
//   world_pixel: current "pixel" from the world_map (lowest priority)
//   icon_pixel : current "pixel" from the icon (highest priority, has transparency options)
//
// Outputs:
//   R: 4-bit red value
//   G: 4-bit green value
//   B: 4-bit blue value
module colorizer(
    input        clk,
    input        video_on,
    input [11:0] world_pixel, 
    input [3:0]  sprite_pixel,
    input sprite_pixel_active,
    
    output reg [3:0] r, g, b);

    reg [11:0] palette_map[0:15];
    
    initial $readmemh("palette.txt", palette_map);  
    
    always @(posedge clk) begin
        {r, g, b} <=  video_on ? 
            (sprite_pixel_active ? 
                palette_map[sprite_pixel] :
                world_pixel) : 
            {4'b0000, 4'b0000, 4'b0000};    
    end
endmodule
