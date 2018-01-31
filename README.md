# The Bootloader is project for Keil Microvision v5

## Compiler settings of Bootloader
## Assembler settings of Bootloader
## Linker settings of Bootloader
	- The bootloader works on the following basic settings:
		Bootloader occupies from the base of flash 0x08000000 to 0x08000FFF.
		Bootloader interrupt vectors are located in flash.
		Application should reside at 0x08001000 starting with it's vectors.
		Application vectors are relocated to 0x20000000, the start of RAM.
		Both Application and Bootloader should keep a hole of 0xC0 bytes to be occupied by Application vectors in RAM.

	- The bootloader shall wait for ~4 seconds for the host to download new Application.

	- The bootloader shall check after ~4 seconds for a valid Application. 
		If a valid Application is found, it boots the Application after relocating vectors in RAM and setting appropriate boot mode.
		If not, it keeps searching for the host to download a new Application. This continues untill a valid Application exists in Flash.
