
set_false_path -from [get_pins -hierarchical -filter { NAME =~  "*mfp_ahb_audio/perchannel[?].mgr/playperiod_reg*/C" && DIRECTION == "IN" }] -to [get_pins -hierarchical -filter { NAME =~  "*_pulsewidth_?/pulsewidth_reg*/D" && DIRECTION == "IN" }]
