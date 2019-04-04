module sprite_sort #(
	parameter SPRITES = 128,
    parameter IDX_BITS = 7,
    parameter X_BITS = 10
)(
  // system signals
   input       clk,
   input       load,
             
   // sort
   input  [(X_BITS*SPRITES)-1:0] x,
   output [(IDX_BITS*SPRITES)-1:0] idx
);

wire [X_BITS-1:0] in [SPRITES-1:0];
	
reg  [X_BITS-1:0]     values [SPRITES-1:0];
reg  [IDX_BITS-1:0]   index  [SPRITES-1:0];
wire [SPRITES/2-1:0]  swap0;
wire [X_BITS-1:0]     int_val [SPRITES-1:0];
wire [IDX_BITS-1:0]   int_idx [SPRITES-1:0];
wire [SPRITES/2-2:0]  swap1;
wire [X_BITS:0]       sort_val [SPRITES-1:0];
wire [IDX_BITS:0]     sort_idx [SPRITES-1:0];

// map 1d input array onto 2d work array
// and 2d result array into 1d output array
generate
genvar i;
for(i=0;i<SPRITES;i=i+1) begin : input_map
	assign in[i] = x[(X_BITS*i)+(X_BITS-1):X_BITS*i];
	assign idx[(IDX_BITS*i)+(IDX_BITS-1):(IDX_BITS)*i] = index[i];
end
endgenerate

generate
   // 1st stage
   for(i=0;i<SPRITES/2;i=i+1) begin : stage1
      assign swap0[i] = values[2*i+0] > values[2*i+1];
      assign int_val[2*i+0] = swap0[i]?values[2*i+1]:values[2*i+0];
      assign int_val[2*i+1] = swap0[i]?values[2*i+0]:values[2*i+1];
      assign int_idx[2*i+0] = swap0[i]?index[2*i+1]:index[2*i+0];
      assign int_idx[2*i+1] = swap0[i]?index[2*i+0]:index[2*i+1];
   end

   // 2nd stage
   assign sort_val[0] = int_val[0];
   assign sort_idx[0] = int_idx[0];
   assign sort_val[SPRITES-1] = int_val[SPRITES-1];
   assign sort_idx[SPRITES-1] = int_idx[SPRITES-1];
   for(i=0;i<SPRITES/2-1;i=i+1) begin : stage4
      assign swap1[i] = int_val[2*i+1] > int_val[2*i+2];
      assign sort_val[2*i+1] = swap1[i]?int_val[2*i+2]:int_val[2*i+1];
      assign sort_val[2*i+2] = swap1[i]?int_val[2*i+1]:int_val[2*i+2];
      assign sort_idx[2*i+1] = swap1[i]?int_idx[2*i+2]:int_idx[2*i+1];
      assign sort_idx[2*i+2] = swap1[i]?int_idx[2*i+1]:int_idx[2*i+2];
   end

   for(i=0;i<SPRITES;i=i+1) begin : advance
		always @(posedge clk) begin
			if(load) begin
				values[i] <= in[i];
				index[i] <= i[IDX_BITS-1:0];
			end else begin
				values[i] <= sort_val[i];
				index[i] <= sort_idx[i];
			end
		end
	end

endgenerate

endmodule
