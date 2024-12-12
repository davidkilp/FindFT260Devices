# FindFT260Devices - Find FT260 Devices for HID Facepod Gate

## Summary
Windows command line program that searches for FT260 devices with I2C addresses matching
Internal (0x60, 0x63) and External (0x62) LED Strip controllers as used on the HID Facepod
and the HID Baryonyx board (external)

The program will list all USB HID (Human Interface Devices NOT HID Global) for specific matching
VID/PID's for the FT260 controller and will then probe the I2C addresses as necessary.

The final two lines of output will tell you if LED strip controllers were found or not on 
both Internal and External boards.

## Usage

Open a Windows cmd.exe or Powershell application and run the corresponding FindFT260Devices.exe program.

The output will look similar to this if the internal and external LED strips are connected:

	C:\Temp\FindFT260Devices>FindFT260Devices.exe
	Finding FT260 USB-to-I2C bridge devices
	Number of HID devices found: 11

	Checking device #0
	Device path:\\?\hid#vid_0403&pid_6030#6&1a51999b&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}
	Found an FT260 Controller at path: \\?\hid#vid_0403&pid_6030#6&1a51999b&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}
	Opened path: \\?\hid#vid_0403&pid_6030#6&1a51999b&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030} OK
	We found an FT260 Controller for Internal LED Strip at: 0x60

	Checking device #1
	Device path:\\?\hid#vid_0403&pid_6030#6&2d0b4a40&1&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}
	Found an FT260 Controller at path: \\?\hid#vid_0403&pid_6030#6&2d0b4a40&1&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}
	Opened path: \\?\hid#vid_0403&pid_6030#6&2d0b4a40&1&0000#{4d1e55b2-f16f-11cf-88cb-001111000030} OK
	readSingleByte fails with status: (19)FT260_I2C_READ_FAIL
	NO FT260 Controller with LED Strip at: 0x60
	Opened path: \\?\hid#vid_0403&pid_6030#6&2d0b4a40&1&0000#{4d1e55b2-f16f-11cf-88cb-001111000030} OK
	We found an FT260 Controller for External LED Strip at: 0x62

	Checking device #2
	Device path:\\?\hid#vid_08bb&pid_27c6&mi_02#7&d660dc6&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}

	Checking device #3
	Device path:\\?\hid#vid_0dc6&pid_3412&mi_00#8&39386564&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\kbd

	Checking device #4
	Device path:\\?\hid#vid_0dc6&pid_3412&mi_01#8&156127a2&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}

	Checking device #5
	Device path:\\?\hid#vid_222a&pid_0001&col01#6&1c87b147&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}

	Checking device #6
	Device path:\\?\hid#vid_222a&pid_0001&col01#6&2f4161ec&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}

	Checking device #7
	Device path:\\?\hid#vid_222a&pid_0001&col02#6&1c87b147&0&0001#{4d1e55b2-f16f-11cf-88cb-001111000030}

	Checking device #8
	Device path:\\?\hid#vid_222a&pid_0001&col02#6&2f4161ec&0&0001#{4d1e55b2-f16f-11cf-88cb-001111000030}

	Checking device #9
	Device path:\\?\hid#vid_222a&pid_0001&col03#6&1c87b147&0&0002#{4d1e55b2-f16f-11cf-88cb-001111000030}

	Checking device #10
	Device path:\\?\hid#vid_222a&pid_0001&col03#6&2f4161ec&0&0002#{4d1e55b2-f16f-11cf-88cb-001111000030}


	---------------------------------------
	Found Internal FT260 controller at: \\?\hid#vid_0403&pid_6030#6&1a51999b&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}
	Found External FT260 controller at: \\?\hid#vid_0403&pid_6030#6&2d0b4a40&1&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}
