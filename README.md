# HiFive1-HAL
This is a demonstration on how to implement a simple C++ program on the FE310-G000 RISC-V based microcontroller on the HiFive1 evaluation kit from SiFive.

The objective is to learn the risc-v and its peripherals from scratch. The drivers are implemented on bare metal.

In this demo the following features of the FE310-G000 chip are used:

- The PLL is used to generate a core clock with 256 MHz fequency
- A pwm peripheral is used to generate a 1 ms tick 
- An spi AD converter is connected to the spi1 interface. spi1 is then used to capture
  the A/D converted voltage
- The UART is used to print a start message, and then the measured voltage in mV 
- PLIC and the core Interrupt circuitry is excersiced to generate uart interrupts.

A SPI based AD converter, AD7476A from Analog Devices was connected to the SPI 1 interface of the 
HiFive1 board. The set-up of spi1 is done to much the requirements of this circuit.

The assembler start-up files and the linker script are copied from the freedom-metal SDK from SiFive. The other drivers are developped in this project, and works directly on top of the chip silicon. The objective is to build a thin- and easily understandable driver layer that can be used to build C++ applications on.

# Set-Up

This repsitory does only contain the application- and driver source code. To run it on the HiFive board it is necessary to compile it into a binary that can be downloaded to the HiFive1 target.

The FE310-G000 is based on a 32-bit RISC-V core, and to compile it, the GCC compiler for the target must be installed.

To set up the project in FreedomStudio, follow the steps below:
- In a workspace of your choise, select File/New/Project to show the "New Project" wizard
- In the dialog select "C/C++" project and "MakeFile project with existing code". Click Next to enter the "Import Existing Code" dialogue
- Brows to the location where you have cloned the project, i.e. [path]/HiFive1-HAL. Then the project name will be automatically set to "HiFive1-HAL". Make sure that the language boxes C and C++ are checked and select the "SiFive RISC-V GNU GCC Newlib" Tool-chain as Toolchain for indexer settings and press Finish.

Once everything is set up you will need to modify the Makefile to reflect where the GCC toolchain is located on your system.
The variable GCC_PATH located towards the top of the Makefile needs to be changed to [your path to GCC riscv32]/bin

# Build and run the executable

When all is set up it should be possible to build the binary by navigating into the HiFive1-HAL folder and type make in a terminal.

The FreedomStudio IDE can be used to download the binary to the target. Start by creating a debug target. 
- Go to menu "Run/Debug Configurations..." to open the Debug Configurations Dialogue.
- Select "SiFive GDB OpenOCD Debugging" in the list of configuration types and press the "New" button (This assumes that you are indeed using OpenOCD for debugging your HiFive1 board.) This opens a dialogue with a number of tabs 
- In the "Main tab, check that the project is "HiFive1-UART" and the binary is "build/spiAdc.elf"
- In tab "Target/DTS" the first field, "Device Tre File" should be set to the file found by browsing to "hw/design.dts". The other fields will be set automatically when the file is selected.
- In the "Debugger" tab everything is usually automatically set. Make sure that the field "Config Options" contains "-f hw/openocd.cfg"
- In tab "Startup" nothing will normally need to be changed.
- In tab "Config" nothing will normally need to be changed.
- In tab "Source" noting will normally need to be changed.
- In tab "Common" nothing will normally need to be changed.

When all is set up you should be able to press the "Apply" and then the "Debug" button. Good Luck!
