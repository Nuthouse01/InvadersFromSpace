

// control everything related to audio system parameterization from here

// how many bits of resolution to use for pwm values: 2=4 steps, 3=8 steps, 4=16 steps, etc
`define BITRES				4
// how wide are the periods and durations in the SFX mem?
`define PERWIDTH			20
`define DURWIDTH			8
// readwidth = total width of one entry in the block mem
`define READWIDTH			(`PERWIDTH + `DURWIDTH)

// ID corresponds to top bits, DEPTH means bottom bits of sfxmem address
`define SFXIDBITS			5
`define SFXDEPTHBITS		2
// addrwidth = total width of the block mem address
`define ADDRWIDTH			(`SFXIDBITS + `SFXDEPTHBITS)


// this define isnt used anywhere, just used to indicate the freq last used to make the sound memory files
`define AUDCLK_FREQ_HZ		5_000_000


// durclk runs off of HCLK with a divider, not audclk
`define HCLK_FREQ_HZ		25_000_000
`define DURCLK_FREQ_HZ		100
`define DURCLK_TOP_CNT		((`HCLK_FREQ_HZ / `DURCLK_FREQ_HZ) - 1)
//`define DURCLK_TOP_CNT	6

// not sure what width is "tightest" but going too big doesn't matter so overshoot
`define DURCLKWIDTH			32
// what pwm should it hold when "muted"?
`define PWM_MUTED			(2**(`BITRES-1))-1
//`define PWM_MUTED			0



`define AUDIDX_PLAYEREXPLODE	0
`define AUDIDX_ENEMYEXPLODE		3
`define AUDIDX_ENEMYSHOOT		6
`define AUDIDX_SONGHIGH			9
`define AUDIDX_SONGLOW			14
`define AUDIDX_WAVESTART		23
`define AUDIDX_GAMEOVER			25
