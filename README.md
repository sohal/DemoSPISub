# Bootloader project for Keil µVision 5

There are two projects in this folder.

Use Boot for bootloader that is integrated with Torque sensor and Pilot hardware

Use Bootloader for bootloader that is working as per Ervin's setup.

## Compiler settings of Bootloader
## Assembler settings of Bootloader
## Linker settings of Bootloader
	- The bootloader works on the following basic settings:
		Bootloader occupies from the base of flash 0x08000000 to 0x08000FFF.
		Bootloader interrupt vectors are located in flash.
		Application should reside at 0x08001000 starting with it's vectors.
		Application vectors are relocated to 0x20000000, the start of RAM.
		Both Application and Bootloader should keep a hole of 0xC0 bytes to be occupied by Application vectors in RAM.
## Usage
	- The bootloader shall wait for ~4 seconds for the host to download new Application.

	- The bootloader shall check after ~4 seconds for a valid Application. 
		If a valid Application is found, it boots the Application after relocating vectors in RAM and setting appropriate boot mode.
		If not, it keeps searching for the host to download a new Application. This continues untill a valid Application exists in Flash.
	
	- Communication parameters: 57600-8-1
