
/*
 * File   : uarttest.c
 *
 * $Id: uarttest.c,v 1.3 2012/05/21 07:07:37 mathes Exp $
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        $Revision: 1.3 $ $Date: 2012/05/21 07:07:37 $
 * Description:    Test the USART of ATmega32 by implementing an echo server.
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

//
// Note: This test program is using UART0 which must be either wired to the
//       (standard) RS-485 adapter or to the MAX 232 which goes to a 9-pin male
//       connector (in DCE configuration).
//
// avrdude -p atmega32 -P usb -c usbasp -y -U flash:w:uarttest.hex
//

int main(void)
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
