
/*
 * File   : blrtest.c
 *
 * Purpose: Program to test buttons, LEDs and relay wiring.
 *
 * $Id: blrtest.c,v 1.4 2012/05/13 19:43:41 mathes Exp $
 *
 */
 
#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "global.h"


/** @file blrtest.c
  * Program to test buttons, LEDs and relay wiring.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

// --------------------------------------------------------------------------

//
// avrdude -p atmega32 -P usb -c usbasp -y -U flash:w:blrtest.hex
//
// the board is supposed to run with 12 MHz crystal oscillator
//

// --------------------------------------------------------------------------

ISR(TIMER0_OVF_vect)
 {
  TCNT0 = CNT0_PRESET;
  
  // call button check routine
  CheckKeys();
}

// --------------------------------------------------------------------------

int main(void)
 {
  // --- HARDWARE INITIALISATION
  
  // timer 0 initialisation
  TCNT0 = CNT0_PRESET;
  TCCR0 = (1<<CS02)|(1<<CS00);  // CK/1024 -> 1 tick each .128 msec

  // enable timer overflow interrupt
  TIMSK |= (1<<TOIE0);

  uint8_t mask;
  
  // Buttons port initialisation, turn pull-ups on
  mask = BUTTON_LEFT | BUTTON_PRESET_LEFT | BUTTON_STOP 
                     | BUTTON_PRESET_RIGHT | BUTTON_RIGHT;
  
  BUTTON_PORT |= mask;
  BUTTON_DDR &= ~mask;

  // relay port initialisation, all relays off
  mask = RELAY1 | RELAY2 | RELAY3 | RELAY4;

  RELAY_PORT &= ~mask;
  RELAY_DDR |= mask;
  
  // LED port initialisation, all LEDs off
  mask = LED_LEFT | LED_RIGHT | LED_CALIBRATE | LED_OVERLOAD;

  LED_PORT &= ~mask;
  LED_DDR |= mask;
  
  // enable interrupts globally
  sei();

  // --- MAIN LOOP, QUERY BUTTONS AND TRIGGER ACTIONS

  while ( 1 ) {


    if ( gKeyState & BUTTON_PRESET_LEFT ) { // check BUTTON 'PRESET LEFT'
      
      LED_PORT |= LED_LEFT;
      RELAY_PORT |= RELAY3;
      
      while ( gKeyState & BUTTON_PRESET_LEFT );
    
      RELAY_PORT &= ~RELAY3;
      LED_PORT &= ~LED_LEFT;
    }

    if ( gKeyState & BUTTON_LEFT ) {   // check BUTTON 'LEFT'
      
      LED_PORT |= LED_CALIBRATE;
      RELAY_PORT |= RELAY1;
      
      while ( gKeyState & BUTTON_LEFT );
      
      RELAY_PORT &= ~RELAY1;
      LED_PORT &= ~LED_CALIBRATE;
    }

    if ( gKeyState & BUTTON_STOP ) {   // check BUTTON 'STOP'
      
      LED_PORT |= (LED_CALIBRATE | LED_OVERLOAD);
    
      while ( gKeyState & BUTTON_STOP );
    
      LED_PORT &= ~(LED_CALIBRATE | LED_OVERLOAD);
    }

    if ( gKeyState & BUTTON_RIGHT ) {  // check BUTTON 'RIGHT'
      
      LED_PORT |= LED_OVERLOAD;
      RELAY_PORT |= RELAY2;
      
      while ( gKeyState & BUTTON_RIGHT );
      
      RELAY_PORT &= ~RELAY2;
      LED_PORT &= ~LED_OVERLOAD;
    }

    if ( gKeyState & BUTTON_PRESET_RIGHT ) { // check BUTTON 'PRESET RIGHT'
      
      LED_PORT |= LED_RIGHT;
      RELAY_PORT |= RELAY4;
      
      while ( gKeyState & BUTTON_PRESET_RIGHT );
    
      RELAY_PORT &= ~RELAY4;
      LED_PORT &= ~LED_RIGHT;
    }

  } // while ( 1 ) ...
  
  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
