set remotetimeout 10
target remote :3333
monitor reset halt

define loadflash
	monitor reset halt
	monitor stm32f1x mass_erase 0
	monitor flash write_image main.hex
	disconnect
	target remote :3333
	file main.elf
	monitor reset halt
end

define lf
	loadflash
end

define mlf
	make
	loadflash
	continue
end
