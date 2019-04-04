


// TODO: fill this out with the xadc IP instantiation and whatever logic is needed to get it constantly running

module top_xadc_stuff(
  input       clk,
  input       resetn,
  input               vauxp2, vauxn2,
  input               vauxp3, vauxn3,
  input               vp_in, vn_in,
  output  [23:0]      xadc_results  // {resultB, resultA} concatenated into one signal
);

  wire enable;
  wire ready;
  wire [15:0] data;
  reg [6:0] Address_in;

  reg [11:0] channels[0:1];

  //xadc instantiation connect the eoc_out .den_in to get continuous conversion
  xadc_wiz_0  XLXI_7 (.daddr_in(Address_in), //addresses can be found in the artix 7 XADC user guide DRP register space
                    .dclk_in(clk),
                    .den_in(enable),
                    .di_in(0),
                    .dwe_in(0),
                    .busy_out(),
                    .vauxp2(vauxp2),
                    .vauxn2(vauxn2),
                    .vauxp3(vauxp3),
                    .vauxn3(vauxn3),
                    .vn_in(vn_in),
                    .vp_in(vp_in),
                    .alarm_out(),
                    .do_out(data),
                    .reset_in(0),
                    .eoc_out(enable),
                    .eos_out(),
                    .channel_out(),
                    .drdy_out(ready));                    

  always @( posedge(clk))
  begin
    if(ready == 1'b1)
    begin
      if (Address_in == 8'h12) begin
          Address_in <= 8'h13;
          channels[0] <= (data >> 4);
      end else begin
          Address_in <= 8'h12;
          channels[1] <= (data >> 4);
      end
    end
  end

  assign xadc_results[23:12] = channels[0];
  assign xadc_results[11: 0] = channels[1];

endmodule


