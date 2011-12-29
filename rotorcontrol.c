
/*
 * File   : rotorcontrol.c
 *
 * Purpose: Program which performs the rotator control.
 *
 * $Id: rotorcontrol.c,v 1.2 2011/12/29 10:35:19 mathes Exp $
 *
 */
 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** @file rotorcontrol.c
  * Program which performs the rotator control.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "global.h"

// --------------------------------------------------------------------------

// ISR for timer/counter 0: called every 10 ms
// - load counter with initial constant
// - call button check routine

ISR(TIMER0_OVF_vect)
 {
  TCNT0 = CNT0_PRESET;
  
  // call button check routine
  CheckKeys();
}

// --------------------------------------------------------------------------

static void InitHardware(void)
 {
  // set input for buttons
  BUTTON_DDR &= ~(BUTTON_LEFT | BUTTON_PRESET_LEFT | BUTTON_STOP 
                              | BUTTON_PRESET_RIGHT | BUTTON_RIGHT);
  
  // timer 0 initialisation
  TCNT0 = CNT0_PRESET;
  TCCR0 = (1<<CS02)|(1<<CS00);  // CK/1024 -> 1 tick each .128 msec

  // enable timer overflow interrupt
  TIMSK |= (1<<TOIE0);

  // relay port initialisation
  RELAY_PORT &= ~(RELAY_CW | RELAY_CCW | RELAY_STOP);
  RELAY_DDR |= RELAY_CW | RELAY_CCW | RELAY_STOP;
  
  // enable interrupts globally
  sei();
}
 
// --------------------------------------------------------------------------

int main(void)
 {
  // init the hardware ...
  InitHardware();
  
  // --- 5 button user interface to rotator control

  while ( 1 ) {

    // --- check BUTTON LEFT:
    
    if ( gKeyState & BUTTON_LEFT ) {
      
      BrakeOff();
      RotatorCCW();
      
      while ( gKeyState & BUTTON_LEFT );
      
      RotatorOff();
      BrakeOn();
    }
    
    // --- check BUTTON RIGHT:
    
    if ( gKeyState & BUTTON_RIGHT ) {
      
      BrakeOff();
      RotatorCW();
      
      while ( gKeyState & BUTTON_RIGHT );
      
      RotatorOff();
      BrakeOn();
    }
    
    // --- check BUTTON STOP:
    
    if ( gKeyState & BUTTON_STOP ) {
      
      while ( gKeyState & BUTTON_STOP );
      
      /* ... */
    }
    
    // --- check BUTTON PRESET LEFT:
    
    if ( gKeyState & BUTTON_PRESET_LEFT ) {
      
      while ( gKeyState & BUTTON_PRESET_LEFT );
      
      /* ... */
    }
    
    // --- check BUTTON PRESET RIGHT:
    
    if ( gKeyState & BUTTON_PRESET_RIGHT ) {
      
      while ( gKeyState & BUTTON_PRESET_RIGHT );
      
      /* ... */
    }
    
  } // while ( 1 ) ...
  
  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
