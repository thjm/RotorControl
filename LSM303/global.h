
/*
 * File   : global.h
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        v1r0
 * Description:    Contains all global definitions of the 'LSM303DLH'
 *                 project(s).
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


#ifndef _global_h_
#define _global_h_

#include <stdint.h>

#include <avr/io.h>

/** @file global.h
  * Contains all global definitions of the 'LSM303DLH' project(s).
  * @author H.-J. Mathes
  */

// we are using the ATmega8 test board of U.Radig
//
// - buttons S1...S5 are connected to PD0 .. PD4
#define BUTTON_PORT       PORTD
#define BUTTON_PIN        PIND
#define BUTTON_DDR        DDRD

#define BUTTON_S1         (1<<PD0)
#define BUTTON_S2         (1<<PD1)
#define BUTTON_S3         (1<<PD2)
#define BUTTON_S4         (1<<PD3)
#define BUTTON_S5         (1<<PD4)

// - button S6 is connected to RESET pin of AVR

/** LED stuff ... */

#define LED_PORT          PORTB
#define LED_PORT_DDR      DDRB

#define LED_YELLOW        (1<<PB2)
#define LED_GREEN         (1<<PB3)
#define LED_RED           (1<<PB4)

#ifdef LED_DEBUG
# define InitLEDPort()     { DDRB |= LED_RED | LED_YELLOW | LED_GREEN; }

# define RedLEDOn()        { PORTB |= LED_RED; }
# define RedLEDOff()       { PORTB &= ~LED_RED; }
# define YellowLEDOn()     { PORTB |= LED_YELLOW; }
# define YellowLEDOff()    { PORTB &= ~LED_YELLOW; }
# define GreenLEDOn()      { PORTB |= LED_GREEN; }
# define GreenLEDOff()     { PORTB &= ~LED_GREEN; }
#else
# define InitLEDPort()     { }

# define RedLEDOn()        { }
# define RedLEDOff()       { }
# define YellowLEDOn()     { }
# define YellowLEDOff()    { }
# define GreenLEDOn()      { }
# define GreenLEDOff()     { }
#endif // LED_DEBUG

// definitions for our production board
//
// RxD & TxD are connected to the standard ports
// SDA & SCL are connected to the standard ports
//
// per default the transmitter of the MAX485 must be enabled during operation
// BUT NOT during boot loader operation !
//

#if 1
  // my first choice
# define RS485_PORT		PORTB
# define RS485_DDR		DDRB
# define RS485_TX_ENABLE	(1<<PB0)

#else
  // use this for the chip45boot2 boot loader (with RS485 support)
  // http://shop.chip45.com/AVR-Mikrocontroller-Software/AVR-ATmega-Xmega-Bootloader/Frei/chip45boot2
  //
  // uses XCK# pin of uController (XCK0 = PD4 for Atmega8)
# define RS485_PORT		PORTD
# define RS485_DDR		DDRD
# define RS485_TX_ENABLE	(1<<PD4)
#endif

#define RS485EnableRx()         { RS485_PORT &= ~RS485_TX_ENABLE; }
#define RS485EnableTx()         { RS485_PORT |= RS485_TX_ENABLE; }

/** Readout interval for the sensors, in multiples of 100 ms */
#define SENSOR_READOUT_PERIOD    2

#if (F_CPU==1000000UL)
// 1 MHz internal oscillator ==> CLK/1024 = 0.9765625 kHz
// T_0 = 1.024 msec ==> * 97 = 99.328 msec = T_1
 #define CNT0_PRESET		(0xff - 97)
#elif (F_CPU==2000000UL)
// 2 MHz internal oscillator ==> CLK/1024 = 1.953125 kHz
// T_0 = 0.512 msec ==> * 195 = 99.84 msec = T_1
 #define CNT0_PRESET             (0xff - 195)
#elif (F_CPU==4000000UL)
// 4 MHz internal oscillator ==> CLK/1024 = 3.90625 kHz
// T_0 = 0.256 msec ==> * 39 = 9.984 msec = T_1
 #define CNT0_PRESET		(0xff - 39)
#elif (F_CPU==8000000UL)
// 8 MHz internal oscillator ==> CLK/1024 = 7.8125 kHz
// T_0 = 0.128 msec ==> * 78 = 9.984 msec = T_1
#define CNT0_PRESET		(0xff - 78)
#else
 #error "No CNT=_PRESET value defined for selected F_CPU!"
#endif // F_CPU

#endif /* _global_h_ */
