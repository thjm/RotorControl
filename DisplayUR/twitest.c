
/*
 * File   : twitest.c
 *
 * Purpose: Program to send test data via I2C to UR's display board (twi_slave).
 *
 * $Id: twitest.c,v 1.1 2011/12/27 12:34:29 mathes Exp $
 *
 */
 
#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


/** @file twitest.c
  * Program to send test data via I2C to UR's display board (twi_slave).
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <i2cmaster.h>

#define USE_UART
#define UART_BAUD_RATE 9600
#define UART_TX_BUFFER_SIZE 16 
#define UART_RX_BUFFER_SIZE 16

#ifdef USE_UART
# include <uart.h>
#endif // USE_UART

/** local prototypes */

static void delay_sec(uint8_t n_sec);

// --------------------------------------------------------------------------

//
// avrdude -p atmega8 -P usb -c usbasp -y -U flash:w:twitest.hex
//
// the board is supposed to run with 12 MHz crystal oscillator
//

// --------------------------------------------------------------------------

/** PCF8574A address definition. */
#define I2C_DEV8574A   0x70  // PCF8574A: 0x72, .., 0x7e

#define TEST_PCF8574A    0
#define TEST_DISP        1

int main(void)
 {
  i2c_init();                         // init I2C interface

#ifdef USE_UART
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
  uart_puts_P("'twitest' ready!\r\n");
  
  sei();
#endif // USE_UART
  
  uint16_t data = 0;
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
    ret = i2c_start( 0x50 | I2C_WRITE);  // set device address and write mode
    
    if ( ret ) {
      /* failed to issue start condition, possibly no device found */
      i2c_stop();
 #ifdef USE_UART
      uart_puts_P("Error - no DISP device!\r\n");
 #endif // USE_UART

      delay_sec(1);

      continue;
    }
    else {
      i2c_write(0x00); // write remote buffer address
      
      i2c_write(data & 0xff);  // write data bytes to remote buffer
      i2c_write((data & 0xff00) >> 8);

      i2c_write(data & 0xff);
      i2c_write((data & 0xff00) >> 8);

      i2c_stop();
 #ifdef USE_UART
      uart_puts_P("Transmit OK\r\n");
 #endif // USE_UART
    }
#endif // TEST_DISP

    data++;
    
    if ( data == 999 ) data = 0;
    
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
