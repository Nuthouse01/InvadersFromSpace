
# this is just a guess, this might or might not be the right way to do these constraints
# 50mhz clock
# this jitter value should match the resulting jitter from the clock wizard summary tab on the 50mhz output

#create_clock -period 20.000 [get_ports SI_ClkIn]
#set_input_jitter [get_clocks -of_objects [get_ports SI_ClkIn]] 150.541ps
create_clock -period 39.725 [get_ports SI_ClkIn]
set_input_jitter [get_clocks -of_objects [get_ports SI_ClkIn]] 319.783ps
