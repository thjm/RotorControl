
/*
 * File   : global.h
 *
 * Purpose: Contains all global definitions of the 'LSM303DLH' project(s)
 *
 * $Id: global.h,v 1.1 2011/10/26 12:52:57 mathes Exp $
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

#endif /* _global_h_ */
