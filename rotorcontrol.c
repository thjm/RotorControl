
/*
 * File   : rotorcontrol.c
 *
 * Purpose: Program which performs the rotator control.
 *
 * $Id: rotorcontrol.c,v 1.4 2012/04/18 19:45:43 mathes Exp $
 *
 */
 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** @file rotorcontrol.c
  * main() of the rotator control program.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "global.h"

// --- variable(s) for the rotator control

volatile uint8_t gRotatorCommand;
volatile uint8_t gRotatorState;

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

  uint8_t mask;
  
  // relay port initialisation, all relays off
  mask = RELAY_POWER | RELAY_CW | RELAY_CCW | RELAY_STOP;
  RELAY_PORT &= ~mask;
  RELAY_DDR |= mask;
  
  // LED port initialisation, all LEDs off, RS485 RX enable
  mask = LED_LEFT | LED_RIGHT | LED_CALIBRATE | LED_OVERLOAD | RS485_TX_ENABLE;
  LED_PORT &= ~mask;
  LED_DDR |= mask;
  
  // Buttons port initialisation, turn pull-ups on
  mask = BUTTON_PRESET_LEFT | BUTTON_LEFT | BUTTON_STOP | BUTTON_RIGHT | BUTTON_PRESET_RIGHT;
  BUTTON_PORT |= mask;
  BUTTON_DDR &= ~mask;
  
  // initialize USART0 (receiving NMEA messages via RS485 from ACC/MAG sensor)
  
  // initialize USART1, for command/status exchange with HAM op (& debug)
  
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
