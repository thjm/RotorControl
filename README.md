# About

RotorController is for controlling a simple antenna rotator using directionaly
signals from a 3D magnet sensor (and from a 3D accelerometer).

The software is split onto several micro controllers from Atmel's AVR family.

- the software for the display board (hardware design based on ATmega8 from U.Radig) 
  is located in the directory ./DisplayUR
- the software for the ATmega8 at the remote magnetic/accelerometric sensor is 
  located in the directory ./LSM303
- the software foreseen to run on the main ATmega32 is in the ./ directory
- further we have the Eagle circuit drawing of the various boards (my boards are
  all done on prototype boards, thus the eagle design was never verified):
  - AntennaBoard.sch : board with the mag./acc. sensor attached 
    to the boom of the antenna
  - ButtonBoard.sch : board to hold push buttons and LEDs for the front panel
  - CPUBoard.sch : the board with the main CPU and connectors to the other 
    board with a micro controller on them
  - PowerSupply.sch : main power supply, +12V and +5V
  - RelayBoard1.sch : board which holds relays for the direction and the release
    of the rotor brake (if available)
  - RelayBoard2.sch : supplement relay board, switches extra 22V AC for the
    used rotor type

## Requirements

- you need a cross-build environment for AVR micro controllers.
- the UART library from Peter Fleury http://homepage.hispeed.ch/peterfleury/doxygen/avr-gcc-libraries/group__pfleury__uart.html
  unpacked to a local directory
- the TWI master library from Peter Fleury http://homepage.hispeed.ch/peterfleury/doxygen/avr-gcc-libraries/group__pfleury__ic2master.html
  unpacked to a local directory
- the environment variable FLEURYHOME must point to the root of both directories

## Usage

Compile the software in the 3 directories ./LSM303, ./DIsplayUR and ./ by 
typing make in ./ We have then:

- ./LSM303/lsm303read.hex : the software for the mag./acc. controller
- ./DisplayUR/i2cdisplay.hex : the software for the I2C controlled LED display
- ./rotorcontrol.hex : the software for the main processor


