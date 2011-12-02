
/*
 * File   : i2cdisplay.c
 *
 * Purpose: Program to display data received via I2C on UR's display board.
 *
 * $Id: i2cdisplay.c,v 1.2 2011/12/02 17:27:06 mathes Exp $
 *
 */
 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/** @file i2cdisplay.c
  * Program to display data received via I2C on UR's display board.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

/** TWI buffer size, will overwrite the settings in the header file. */
//#define TWI_SLAVE_BUFFERSIZE 10
#include <twislave.h>

/** Address of the implemented TWI slave */
#define SLAVE_ADRESS 0x50

#include "multiplex.h"

/** local prototypes */

static void delay_sec(uint8_t n_sec);

// --------------------------------------------------------------------------

//
// avrdude -p atmega8 -P /dev/parport1 -c stk200 -y -U flash:w:i2cdisplay.hex
//
// avrdude -p atmega8 -P usb -c usbasp -y -U flash:w:i2cdisplay.hex
//
// --------------------------------------------------------------------------

int main(void)
 {
  // init the hardware ...
  DDRD |= (1<<PD0)|(1<<PD1)|(1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6);
  DDRB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5);
  
  MultiplexInit();
  
  // initialize the TWI slave
  TWI_SlaveInit(SLAVE_ADRESS /*, enable_irq */);
  
  // enable interrupts globally
  sei();
  
  int val1 = 0;
  
  while ( 1 ) {
    
    MultiplexSet( val1, val1 );
    
    val1++;
    
    if ( val1 == 1000 ) val1 = 0;
    
    delay_sec(1);
  }
  
  return 0;
}

// --------------------------------------------------------------------------

static void delay_sec(uint8_t n_sec)
 {
  while ( n_sec ) {
    // inline in util/delay.h
    for ( int m=0; m<100; m++ )
      _delay_ms( 10.0 );
    n_sec--;
  }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
