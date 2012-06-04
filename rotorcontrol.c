
/*
 * File   : rotorcontrol.c
 *
 * $Id: rotorcontrol.c,v 1.23 2012/06/04 17:34:11 mathes Exp $
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        $Revision: 1.23 $ $Date: 2012/06/04 17:34:11 $
 * Description:    Program which performs the rotator control.
 *
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version. 
        
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Falls nicht, schreiben Sie an die Free Software Foundation, 
   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

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
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include <i2cmaster.h>   // P.Fleury's lib

#include <uart.h>        // P.Fleury's lib

#include "global.h"
#include "i2cdisplay.h"

#define UART_BAUD_RATE 9600

// Fuses and programming:
//
// Factory settings:
//  Device signature = 0x1e9502
//  hfuse = 0x99
//  lfuse = 0xE1
//
// avrdude -p atmega32 -P usb -c usbasp -y -Y 0
//
// avrdude -p atmega32 -P /dev/parport1 -c stk200 -y -U flash:w:rotorcontrol.hex
//
// avrdude -p atmega32 -P usb -c usbasp -y -U flash:w:rotorcontrol.hex
//
// avrdude -p atmega32 -P usb -c usbasp -y -U lfuse:r:-:i -U hfuse:r:-:i 
// avrdude -p atmega32 -P usb -c usbasp -y -U hfuse:w:0xC9:m
//  - JTAG disabled
//  - CKOPT enabled
// avrdude -p atmega32 -P usb -c usbasp -y -U lfuse:w:0xEE:m
//  - external crystal (CKSEL3..1 = 111)
//
// //avrdude -p atmega32 -P usb -c usbasp
//
// avrdude -p atmega32 -P usb -c usbasp -y -U flash:w:avrledtest.hex
// avrdude -p atmega32 -P usb -c usbasp -y -U flash:w:displaytest.hex
//

// local prototypes

static void delay_sec(uint8_t);

// --------------------------------------------------------------------------

// configuration storage in EEPROM

uint16_t gLimitAngle EEMEM = LIMIT_ANGLE;

i_vector_t gMAG_min EEMEM = { -364, -535, -535 };
i_vector_t gMAG_max EEMEM = {  202,  -83,  -83 };

// --------------------------------------------------------------------------

// ISR for timer/counter 0 overflow: called every 10 ms
// - load counter with initial constant
// - call button check routine

ISR(TIMER0_OVF_vect) {

  TCNT0 = CNT0_PRESET;
  
  // call button check routine
  CheckKeys();

  // rotator command interface
  if ( gRotatorBusyCounter )
    gRotatorBusyCounter--;
  else
    RotatorExec();
  
  // execute 'Preset' program
  PresetExec();
  
  // will switchoff 'Preset' display after some time
  if ( gPresetDisplayCounter > 0 )
    gPresetDisplayCounter--;
}

// --------------------------------------------------------------------------

static void InitHardware(void) {

  delay_sec( 1 );
  
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
  mask = LED_LEFT | LED_RIGHT | LED_CALIBRATE | LED_OVERLOAD;

  LED_PORT &= ~mask;
  LED_DDR |= mask;
  
  RS485EnableRx();
  RS485_DDR |= RS485_TX_ENABLE;
  
  // Buttons port initialisation, turn pull-ups on
  mask = BUTTON_PRESET_CCW | BUTTON_CCW | BUTTON_STOP | BUTTON_CW |
  BUTTON_PRESET_CW;

  BUTTON_PORT |= mask;
  BUTTON_DDR &= ~mask;
  
  // initialize USART0 (receiving NMEA messages via RS485 from ACC/MAG sensor)
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

  // initialize USART1, for command/status exchange with HAM op (& debug)
  
  // enable interrupts globally
  sei();
  
  // init I2C interface
  i2c_init();
  
  I2CDisplayBlank();
  I2CDisplayOn();
}
 
// --------------------------------------------------------------------------

static const uint8_t cStartMessage[] PROGMEM = 
         { 0x77, 0x65, 0x6b, 0x12, 0x4f, 0x00 }; // "DC2" "IP "
//         { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20 };  // test pattern

static const uint8_t cPauseMessage[] PROGMEM = 
         { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08 }; // "---" "---" 

static void StartMessage(uint8_t n_sec) {
  
  I2CDisplayWrite_p( sizeof(cStartMessage), cStartMessage );
  I2CDisplayOn();
  
  delay_sec( n_sec );
  
  I2CDisplayWrite_p( sizeof(cPauseMessage), cPauseMessage );
  
  delay_sec( 1 );
  
  I2CDisplayBlank();
}

// --------------------------------------------------------------------------

int main(void) {

  // initialize the hardware ...
  InitHardware();
  
  // reset the message decoding engine
  CompassMessageInit();
  
  // display the start message, and leave it for # seconds on
  StartMessage(2);
  
  unsigned int uart_data;
  
  while ( 1 ) {

   // --- handle serial messages (from ACC/MAG sensor)

   if ( (uart_data = uart_getc()) != UART_NO_DATA ) {

     CommpassMessageReceive( uart_data );
   }

   // --- handle serial messages from RS232 interface
   
     // ...
     
   // --- update of heading display

    UpdateDisplay();
    
   // --- 5 button user interface to rotator control
  
    // --- checks for BUTTON CCW ---
    
    if ( (gKeyState & BUTTON_CCW) && !(gKeyState & BUTTON_STOP)
                                   && !IsRotatorBusy() ) {
      
      SetCommand( kTurnCCW );
    }
    
    // was BUTTON CCW released ?
    
    if ( (GetLastCommand() == kTurnCCW) && !(gKeyState & BUTTON_CCW) ) {

      SetCommand( kStop );
    }
    
    // --- checks for BUTTON CW ---
    
    if ( (gKeyState & BUTTON_CW) && !(gKeyState & BUTTON_STOP)
                                    && !IsRotatorBusy() ) {
      
      SetCommand( kTurnCW );
    }

    // was BUTTON CW released ?
    
    if ( (GetLastCommand() == kTurnCW) && !(gKeyState & BUTTON_CW) ) {
      
      SetCommand( kStop );
    }
    
    // --- checks for BUTTON STOP ---
    
    if ( gKeyState & BUTTON_STOP ) {
      
      SetCommand( kFastStop );
    }
    
    // --- checks for BUTTON PRESET CCW ---
    
    if ( gKeyState & BUTTON_PRESET_CCW ) {
      
      SetPresetCommand( kPresetCCW );
    }
    
    if ( (gPresetCommand == kPresetCCW) && !(gKeyState & BUTTON_PRESET_CCW) ) {
      
      SetPresetCommand( kPresetStop );
    }
    
    // --- checks BUTTON PRESET CW ---
    
    if ( gKeyState & BUTTON_PRESET_CW ) {
      
      SetPresetCommand( kPresetCW );
    }
    
    if ( (gPresetCommand == kPresetCW) && !(gKeyState & BUTTON_PRESET_CW) ) {
      
      SetPresetCommand( kPresetStop );
    }
    
  } // while ( 1 ) ...
  
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
