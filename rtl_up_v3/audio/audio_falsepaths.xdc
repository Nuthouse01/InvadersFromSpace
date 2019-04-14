## Specify that the paths from the period generation (on 25mhz) to the pulsewidth generators 
## (on 5mhz) isn't subject to timing concerns
## This is because the period changes very, very infrequently and it's easier to set it as a 
## "false path" than try to figure out how multicycle paths work

set_false_path -from [get_pins -hierarchical -filter { NAME =~  "*mfp_ahb_audio/perchannel[?].mgr/playperiod_reg*/C" && DIRECTION == "IN" }] -to [get_pins -hierarchical -filter { NAME =~  "*_pulsewidth_?/pulsewidth_reg*/D" && DIRECTION == "IN" }]
