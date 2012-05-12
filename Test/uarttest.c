
/*
 * File   : uarttest.c
 *
 * Purpose: Test the USART of ATmega32 by implementing an echo server.
 *
 * $Id: uarttest.c,v 1.1 2012/05/12 22:11:45 mathes Exp $
 *
 */

#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/** @file uarttest.c
  * Echo server to test ATmega32 USART.
  * @author H.-J.Mathes <dc2ip@darc.de>
  */
  
#include <uart.h>
// baud rate for our USART (ATmega32)
#define UART_BAUD_RATE 9600

// this should be set in the Makefile
#ifndef F_CPU
# warning F_CPU not set, setting it to 1 MHz (factory default)
# define F_CPU 1000000UL  // 1 MHz
#endif // F_CPU
// --------------------------------------------------------------------------
 
int main( void )
 {
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

  sei();  // P.Fleurys lib is using interrupts

  // --- output some string(s)
  
  uart_puts( "USART echo server...\r\n" );
  
  // loop forever ...
  for (;;) {
    unsigned int ch;

    ch = uart_getc();
    if ( ch & UART_NO_DATA ) {
      // uart_puts_p( PSTR("No data !\n\r") );
      // no data received -> continue
    }
    else {
      // check error flags first
      if ( ch & UART_FRAME_ERROR )
//        uart_puts_p( PSTR("UART Frame Error: ") );
        uart_puts_P( "UART Frame Error: " );
      if ( ch & UART_OVERRUN_ERROR )
        uart_puts_P( "UART Overrun Error: " );
      if ( ch & UART_BUFFER_OVERFLOW )
        uart_puts_P( "Buffer overflow Error: ");
        
      uart_putc( (uint8_t)ch );
      
      if ( (uint8_t)ch == 0x0d ) uart_putc( 0x0a );  // add LF
    }
  } // for (;;) ...
  
  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
