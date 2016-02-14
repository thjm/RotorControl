
/*
 * File   : blrtest.c
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: none
 * Version:        v1r0
 * Description:    Program to test buttons, LEDs and relay wiring.
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

ISR(TIMER0_OVF_vect) {

  TCNT0 = CNT0_PRESET;
  
  // call button check routine
  CheckKeys();
}

// --------------------------------------------------------------------------

int main(void) {

  // --- HARDWARE INITIALISATION
  
  // timer 0 initialisation
  TCNT0 = CNT0_PRESET;
  TCCR0 = (1<<CS02)|(1<<CS00);  // CK/1024 -> 1 tick each .128 msec

  // enable timer overflow interrupt
  TIMSK |= (1<<TOIE0);

  uint8_t mask;
  
  // Buttons port initialisation, turn pull-ups on
  mask = BUTTON_CCW | BUTTON_PRESET_CCW | BUTTON_STOP 
                    | BUTTON_PRESET_CW | BUTTON_CW;
  
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

    if ( gKeyState & BUTTON_PRESET_CCW ) { // check BUTTON 'PRESET CCW'
      
      LED_PORT |= LED_LEFT;
      RELAY_PORT |= RELAY3;
      
      while ( gKeyState & BUTTON_PRESET_CCW );
    
      RELAY_PORT &= ~RELAY3;
      LED_PORT &= ~LED_LEFT;
    }

    if ( gKeyState & BUTTON_CCW ) {   // check BUTTON 'CCW'
      
      LED_PORT |= LED_CALIBRATE;
      RELAY_PORT |= RELAY1;
      
      while ( gKeyState & BUTTON_CCW );
      
      RELAY_PORT &= ~RELAY1;
      LED_PORT &= ~LED_CALIBRATE;
    }

    if ( gKeyState & BUTTON_STOP ) {   // check BUTTON 'STOP'
      
      LED_PORT |= (LED_CALIBRATE | LED_OVERLOAD);
    
      while ( gKeyState & BUTTON_STOP );
    
      LED_PORT &= ~(LED_CALIBRATE | LED_OVERLOAD);
    }

    if ( gKeyState & BUTTON_CW ) {  // check BUTTON 'CW'
      
      LED_PORT |= LED_OVERLOAD;
      RELAY_PORT |= RELAY2;
      
      while ( gKeyState & BUTTON_CW );
      
      RELAY_PORT &= ~RELAY2;
      LED_PORT &= ~LED_OVERLOAD;
    }

    if ( gKeyState & BUTTON_PRESET_CW ) { // check BUTTON 'PRESET CW'
      
      LED_PORT |= LED_RIGHT;
      RELAY_PORT |= RELAY4;
      
      while ( gKeyState & BUTTON_PRESET_CW );
    
      RELAY_PORT &= ~RELAY4;
      LED_PORT &= ~LED_RIGHT;
    }

  } // while ( 1 ) ...
  
  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
