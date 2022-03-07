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

## Directories

- Bootloader : contains the boot loader for the antenna sensor uC (not yet used?)

- Docu : various documentation files

- DisplayUR : parts takes from Ulrich Radig's (http://www.ulrichradig.de)
          PowerSupply project. The display unit presented there seems to be
          quite usable for a simple display here, just software needs to be 
          adopted.

- Eagle : contains drawings and schemes done with the Eagle layout program

- Linux : various test programs, to be run on Linux

- LSM303 : library and example for the Pololu LSM303DLH 6-axis sensor
          breakout board. The boards contains a 3-axis magnetometer and a
          3-axis accelerometer.
          See also:
  - http://www.lipoly.de/index.php?main_page=product_info&products_id=165628
  - data sheet LSM303DLH.pdf
  - LSM303DLH-compass-app-note.pdf
  - LSM303DLH-orangutan-example-cde.zip
  - UM10204.pdf (I2C-bus specification)
  - https://github.com/ryantm/LSM303DLH (Arduino library)

- Sketches : Arduino sketches collection

- Test : directory containing various test programs for the 'rotorcontrol'
         hardware environment

## Requirements

- you need a cross-build environment for AVR micro controllers.
- the UART library from Peter Fleury, unpacked to a local directory
- the TWI master library from Peter Fleury, unpacked to a local directory
- the environment variable FLEURYHOME must point to the root of both directories

## Usage

Compile the software in the 3 directories ./LSM303, ./DIsplayUR and ./ by 
typing make in ./ We have then:

- ./LSM303/lsm303read.hex : the software for the mag./acc. controller
- ./DisplayUR/i2cdisplay.hex : the software for the I2C controlled LED display
- ./rotorcontrol.hex : the software for the main processor

## Project Details

### Revision 1.1

- the display of UR's PS1 project will be used to display the current and the
  set value for the azimuth (directory DisplayUR)

- a LSM303DLH accelerometer and compass module will be used to determine the
  antenna direction. It will be coupled to the main electronics via a serial
  interface (coupled via RS-485)

- the main electronics module has the task to
  - receive direction information from the LSM303DLH
  - calculate/average the direction information and send it to the display 
    module
  - compare the current direction with the preset value and pass this
    information to the rotator controller (software module)
  - switch the rotator motor in an appropriate way to achieve the desired
    direction; handle (optional) end-switches and a clutch/brake
  - get users input via push buttons (attached to the main electronics or to
    the display module?)
  - ...

### Revision 2.x

Managed somehow to destroy the antenna sensor during comissioning. As sensor is no
longer available, purchased successor LSM303DLHC (GY-511).

## References

- http://homepage.hispeed.ch/peterfleury/doxygen/avr-gcc-libraries/group__pfleury__uart.html, UART library
- http://homepage.hispeed.ch/peterfleury/doxygen/avr-gcc-libraries/group__pfleury__ic2master.html, TWI library
- https://github.com/ryantm/LSM303DLH, Arduini library for LSM303 like sensors

