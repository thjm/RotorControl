
/*
 * File   : displaytest.c
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: none
 * Version:        v1r0
 * Description:    Program to send test data via I2C to UR's display board 
 *                 (as TWI slave).
 *
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version. 
	        
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
   If not, write to the Free Software Foundation, 
   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

 */
 
#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


/** @file displaytest.c
  * Program to send test data via I2C to UR's display board (twi_slave).
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <i2cmaster.h>

//#define USE_UART
#define UART_BAUD_RATE 9600
#define UART_TX_BUFFER_SIZE 16 
#define UART_RX_BUFFER_SIZE 16

#ifdef USE_UART
# include <uart.h>
#endif // USE_UART

#include "i2cdisplay.h"

/** local prototypes */

static void delay_sec(uint8_t n_sec);
static void delay_msec10(uint8_t msec10);
static uint8_t I2C_DisplayWrite(void);
#ifdef USE_UART
static void int2uart(int val);
#endif // USE_UART

// --------------------------------------------------------------------------

//
// avrdude -p atmega32 -P usb -c usbasp -y -U flash:w:displaytest.hex
//
// the board is supposed to run with 12 MHz crystal oscillator
//

// --------------------------------------------------------------------------

/** PCF8574A address definition. */
#define I2C_DEV8574A   0x70  // PCF8574A: 0x72, .., 0x7e

#define TEST_PCF8574A    0
#define TEST_DISP        1

int main(void) {

  i2c_init();                         // init I2C interface

#ifdef USE_UART
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
  uart_puts_P("'twitest' ready!\r\n");
  
  sei();
#endif // USE_UART
  
  uint8_t ret = 0;
  
  while ( 1 ) {
  
#ifdef USE_UART
      uart_puts_P("Loop...\r\n");
#endif // USE_UART

    if ( ret ) {
      i2c_init();                         // init I2C interface
#ifdef USE_UART
      uart_puts_P("I2C init!\r\n");
#endif // USE_UART
    }
    
#if (TEST_PCF8574A == 1)
    // test with PCF8574A
    ret = i2c_start(I2C_DEV8574A | I2C_WRITE);
    
    if ( ret ) {
      /* failed to issue start condition, possibly no device found */
      i2c_stop();
#ifdef USE_UART
      uart_puts_P("Error - no PCF8574A device!\r\n");
#endif // USE_UART

      delay_sec(1);

      continue;
    }
    else {
      /* issuing start condition ok, device accessible */
      i2c_write(0xfe);			   // write data to IO extender
      i2c_stop(); 			   // set stop conditon = release bus
 #ifdef USE_UART
      uart_puts_P("PCF8574A OK!\r\n");
 #endif // USE_UART
    }
#endif  // TEST_PCF8574A

#if (TEST_DISP == 1 )
    ret = I2C_DisplayWrite();
    
    if ( ret ) {
      delay_sec(1);
      continue;
    }
#endif // TEST_DISP
    
    delay_msec10(20);
  }
  
  return 0;
}

// --------------------------------------------------------------------------

static void delay_sec(uint8_t n_sec) {

  while ( n_sec ) {
    // inline in util/delay.h
    for ( int m=0; m<100; m++ )
      _delay_ms( 10.0 );
    n_sec--;
  }
}

// --------------------------------------------------------------------------

static void delay_msec10(uint8_t msec10) {

  while ( msec10 ) {
    // inline in util/delay.h
    _delay_ms( 10.0 );
    msec10--;
  }
}

// --------------------------------------------------------------------------

#if (TEST_DISP == 1 )
static uint8_t I2C_DisplayWrite(void) {

  static uint16_t data = 0;
  uint8_t ret = 0;
  
  ret = i2c_start( I2C_DISPLAY | I2C_WRITE);  // set device address and write mode
  
  if ( ret ) {
    /* failed to issue start condition, possibly no device found */
    i2c_stop();
 #ifdef USE_UART
    uart_puts_P("Error - no DISP device!\r\n");
 #endif // USE_UART
    return ret;
  }
  
 #ifdef USE_UART
  uart_puts_P("Data: ");
  int2uart( data );
  uart_puts_P("\r\n");
 #endif // USE_UART
      
  if ( (data % 300) < 100 ) {
  
    i2c_write(0x00); // write remote buffer address (always)
    i2c_write(I2C_DISP_DATA);  // write command
      
    i2c_write(data & 0xff);  // write data bytes to remote buffer
    i2c_write((data & 0xff00) >> 8);

    i2c_write(data & 0xff);
    i2c_write((data & 0xff00) >> 8);

    i2c_stop();
  }
  else if ( (data % 300) < 200 ) {
  
    i2c_write(0x00); // write remote buffer address (always)
    i2c_write(I2C_DISP_DATA_LEFT);  // write command
      
    i2c_write(data & 0xff);  // write data bytes to remote buffer
    i2c_write((data & 0xff00) >> 8);

    i2c_stop();
    
    _delay_ms(1.0);  // needs some delay before next start

    ret = i2c_start( I2C_DISPLAY | I2C_WRITE);  // set device address and write mode
    
    if ( ret ) {
      /* failed to issue start condition, possibly no device found */
      i2c_stop();
 #ifdef USE_UART
      uart_puts_P("Error - no DISP device!\r\n");
 #endif // USE_UART
      return ret;
    }
  
    i2c_write(0x00);
    i2c_write(I2C_DISP_RAWDATA_RIGHT);  // write command
    
    i2c_write( 0x08 );   		// "---"
    i2c_write( 0x08 );
    i2c_write( 0x08 );
    
    i2c_stop();
  }
  else if ( (data % 300) < 300 ) {

    i2c_write(0x00); // write remote buffer address (always)
    i2c_write(I2C_DISP_RAWDATA_LEFT);  	// write command
    
    i2c_write( 0x08 ); 			// "---"
    i2c_write( 0x08 );
    i2c_write( 0x08 );
    
    i2c_stop();

    _delay_ms(1.0);

    ret = i2c_start( I2C_DISPLAY | I2C_WRITE);  // set device address and write mode
    
    if ( ret ) {
      /* failed to issue start condition, possibly no device found */
      i2c_stop();
 #ifdef USE_UART
      uart_puts_P("Error - no DISP device!\r\n");
 #endif // USE_UART
      return ret;
    }

    i2c_write(0x00);
    i2c_write(I2C_DISP_DATA_RIGHT);  // write command

    i2c_write(data & 0xff);  // write data bytes to remote buffer
    i2c_write((data & 0xff00) >> 8);

    i2c_stop();
  }
  
  _delay_ms(1.0);

 #ifdef USE_UART
  uart_puts_P("Transmit OK\r\n");
 #endif // USE_UART

  data++;
  
  if ( data == 999 ) data = 0;

  return ret;
}

#endif // TEST_DISP

// --------------------------------------------------------------------------

#ifdef USE_UART
static const unsigned int TEST[] PROGMEM = { 10, 100, 1000, 10000 };

void int2uart(int val) {

  unsigned char d, i;
  unsigned char zero;
  unsigned int uval = val;

  if( val < 0 ){
    uval = -val;
    uart_putc( '-' );
  }

  zero = 1;
  i = 4;
  do{
    i--;
    for ( d = '0'; uval >= pgm_read_word(&TEST[i]); 
                   uval -= pgm_read_word(&TEST[i]) ) {
      d++;
      zero = 0;
    }

    if( zero == 0 ) uart_putc( d );

  } while( i );

  uart_putc( (unsigned char)uval + '0' );
}

#endif // USE_UART

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
