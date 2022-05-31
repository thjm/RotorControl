
/*
 * File   : i2cdisplay.c
 *
 * Purpose: Program to display data received via I2C on UR's display board.
 *
 * $Id: i2cdisplay.c,v 1.8 2012/05/16 15:00:48 mathes Exp $
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>


/** @file i2cdisplay.c
  * Program to display data received via I2C on UR's display board.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <twislave.h>

#include "multiplex.h"
#include "i2cdisplaydefs.h"

// --------------------------------------------------------------------------

//
// avrdude -p atmega8 -P /dev/parport1 -c stk200 -y -U flash:w:i2cdisplay.hex
//
// avrdude -p atmega8 -P usb -c usbasp -y -U flash:w:i2cdisplay.hex
//
// the board is supposed to run with 8 MHz internal oscillator
//
// 8 MHz: avrdude -p atmega8 -P usb -c usbasp -U lfuse:w:0xe4:m
// 4 MHz: avrdude -p atmega8 -P usb -c usbasp -U lfuse:w:0xe3:m
// 1 MHz: avrdude -p atmega8 -P usb -c usbasp -U lfuse:w:0xe1:m
//

// --------------------------------------------------------------------------

int main(void)
 {
  // init the hardware ...
  DDRD |= (1<<PD0)|(1<<PD1)|(1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6);
  DDRB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5);

  MultiplexInit();

  // initialize the TWI slave
  TWI_SlaveInit(I2C_DISPLAY, 0 /*, enable_irq */);

  // enable interrupts globally
  sei();

  uint16_t * data_left;
  uint16_t * data_right;


  while ( 1 ) {

    if ( !gTWI_SlaveStopReceived ) continue;

    switch ( gTWI_SlaveRxBuffer[0] ) { // command

      case I2C_DISP_OFF:
           MultiplexOff();
           break;

      case I2C_DISP_ON:
           MultiplexOn();
           break;

      case I2C_DISP_DATA_LEFT:
           data_left = (uint16_t *)&gTWI_SlaveRxBuffer[1];
	   MultiplexSetL( *data_left );
           break;

      case I2C_DISP_DATA_RIGHT:
	   data_right = (uint16_t *)&gTWI_SlaveRxBuffer[1];
	   MultiplexSetR( *data_right );
           break;

      case I2C_DISP_DATA:
           data_left = (uint16_t *)&gTWI_SlaveRxBuffer[1];
	   data_right = (uint16_t *)&gTWI_SlaveRxBuffer[3];
           MultiplexSet( *data_left, *data_right );
           break;

      case I2C_DISP_RAWDATA:
           MultiplexSetLRaw( (uint8_t *)&gTWI_SlaveRxBuffer[1] );
           MultiplexSetRRaw( (uint8_t *)&gTWI_SlaveRxBuffer[4] );
           break;

      case I2C_DISP_RAWDATA_LEFT:
           MultiplexSetLRaw( (uint8_t *)&gTWI_SlaveRxBuffer[1] );
           break;

      case I2C_DISP_RAWDATA_RIGHT:
           MultiplexSetRRaw( (uint8_t *)&gTWI_SlaveRxBuffer[1] );
           break;
    }

  } // while ( 1 ) ...

  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
