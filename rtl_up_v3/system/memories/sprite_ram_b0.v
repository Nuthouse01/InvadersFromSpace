module sprite_ram
#(parameter DATA_WIDTH=8, parameter ADDR_WIDTH=6)
(
    input clk,
    
	input [(DATA_WIDTH-1):0] data1,
	input [(ADDR_WIDTH-1):0] read_addr1, write_addr1,
	input we1,
	output reg [(DATA_WIDTH-1):0] q1,

	input [(ADDR_WIDTH-1):0] read_addr2,
	output reg [(DATA_WIDTH-1):0] q2
);

	// Declare the RAM variable
	reg [DATA_WIDTH-1:0] ram[2**ADDR_WIDTH-1:0];

	always @ (posedge clk)
	begin
		// Write
		if (we1)
			ram[write_addr1] <= data1;

		q1 <= ram[read_addr1];

		q2 <= ram[read_addr2];
	end
endmodule