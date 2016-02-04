/*********************************************
* vim: set sw=8 ts=8 si :
* Author: Guido Socher, Copyright: GPL 
* See http://linuxfocus.org/English/November2004/
* for details.
* Chip type           : ATMEGA8
* Clock frequency     : Internal clock 1 Mhz (factory default)
*
*********************************************/

#include <avr/io.h>
#include <util/delay.h>

//
// avrdude -p atmega8 -P /dev/parport1 -c stk200 -y -U flash:w:avrledtest.hex
//
// avrdude -p atmega8 -P usb -c usbasp -y -U flash:w:avrledtest.hex
//
// 1 MHz internal clock (factory settings):
// = avrdude -p atmega8 -P usb -c usbasp -y -U lfuse:w:0xE1:m -U hfuse:w:0xD9:m
// 2 MHz internal clock:
// = avrdude -p atmega8 -P usb -c usbasp -y -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m
// 4 MHz internal clock:
// = avrdude -p atmega8 -P usb -c usbasp -y -U lfuse:w:0xE3:m -U hfuse:w:0xD9:m
// 8 MHz internal clock:
// = avrdude -p atmega8 -P usb -c usbasp -y -U lfuse:w:0xE4:m -U hfuse:w:0xD9:m
//

// A = 0, B = 1, C = 2, D = 3
#define USE_PORT 3

// definitions of ports, bits, etc.
// original avrm8ledtest.c was with PC5
#if (USE_PORT == 0)
// works for ATmega32, ATmega644, ...
# warning Using Port A !
# define PORT       PORTA
# define PORT_DDR   DDRA
//# define PORT_PIN   PA0
//# define PORT_PIN   PA1
# define PORT_PIN   PA2
//# define PORT_PIN   PA3
//# define PORT_PIN   PA4
//# define PORT_PIN   PA5
//# define PORT_PIN   PA6
//# define PORT_PIN   PA7
#elif (USE_PORT == 1)
# warning Using Port B !
# define PORT       PORTB
# define PORT_DDR   DDRB
//# define PORT_PIN   PB0
# define PORT_PIN   PB1
//# define PORT_PIN   PB2
//# define PORT_PIN   PB3
//# define PORT_PIN   PB4
//# define PORT_PIN   PB5
//# define PORT_PIN   PB6
//# define PORT_PIN   PB7
#elif (USE_PORT == 2)
# warning Using Port C !
# define PORT       PORTC
# define PORT_DDR   DDRC
# define PORT_PIN   PC0
//# define PORT_PIN   PC1
//# define PORT_PIN   PC2
//# define PORT_PIN   PC3
//# define PORT_PIN   PC4
//# define PORT_PIN   PC5
//# define PORT_PIN   PC6
//# define PORT_PIN   PC7
#elif (USE_PORT == 3)
# warning Using Port D !
// works for ATmega8, AT90S2313, ...
# define PORT       PORTD
# define PORT_DDR   DDRD
//# define PORT_PIN   PD0
//# define PORT_PIN   PD1
//# define PORT_PIN   PD2
//# define PORT_PIN   PD3
//# define PORT_PIN   PD4
//# define PORT_PIN   PD5
//# define PORT_PIN   PD6
# define PORT_PIN   PD7
#else
# error "No PORT was defined!"
#endif


#ifndef F_CPU
 #warning F_CPU not defined, assuming 1 MHz
 #define F_CPU 1000000UL
#endif // F_CPU

// --------------------------------------------------------------------------

void delay_ms(unsigned short ms)
/* delay for a minimum of <ms> */
/* with a 1Mhz clock, the resolution is 1 ms */
 {
#if 1
# if 0
  // Note 1: This function is faulty, see avrm8ledtest-0.2.tar.gz for
  //	     updated code.
  // Note 2: It seems that avr-gcc 4.3.3 optimisation removes that function.
  //         -> the code from avrm8ledtest-0.2 might have the same problem...
  unsigned short outer1, outer2;
  outer1 = 50 * (F_CPU/1000000UL); 

  while ( outer1 ) {
    outer2 = 1000;
    while ( outer2 ) {
      while ( ms ) ms--;
      outer2--;
    }
    outer1--;
  }
# else
  // we use a calibrated macro. This is more
  // accurate and not so much compiler dependent
  // as self made code.
  while(ms){
    _delay_ms(0.96);
    ms--;
  }
# endif
#else
  while ( ms ) {
    ms--;
    _delay_ms( 1.0 );
  }
#endif
}

// --------------------------------------------------------------------------

/* new style, only */
int main(void) {

  /* INITIALIZE */
  /* enable PORT_PIN as output */
  PORT_DDR |= _BV(PORT_PIN);


  /* BLINK, BLINK ... */

  while (1) {
    /* led on, pin=0 */
    PORT &= ~_BV(PORT_PIN);
    delay_ms(500);
    /* set output to 5V, LED off */
    PORT |= _BV(PORT_PIN);
    delay_ms(500);
  }

  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
