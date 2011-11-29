
/*
 * File   : i2cdisplay.c
 *
 * Purpose: Program to display data received via I2C on UR's display board.
 *
 * $Id: i2cdisplay.c,v 1.1 2011/11/29 21:09:54 mathes Exp $
 *
 */
 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>

/** @file i2cdisplay.c
  * Program to display data received via I2C on UR's display board.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include "multiplex.h"

// --------------------------------------------------------------------------

//
// avrdude -p atmega8 -P /dev/parport1 -c stk200 -y -U flash:w:i2cdisplay.hex
//
// avrdude -p atmega8 -P usb -c usbasp -y -U flash:w:i2cdisplay.hex
//
// --------------------------------------------------------------------------

int main(void)
 {
  
  while ( 1 ) {
  }
  
  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
