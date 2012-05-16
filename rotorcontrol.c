
/*
 * File   : rotorcontrol.c
 *
 * Purpose: Program which performs the rotator control.
 *
 * $Id: rotorcontrol.c,v 1.12 2012/05/16 20:06:12 mathes Exp $
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
#include <util/delay.h>

#include <i2cmaster.h>   // P.Fleury's lib

#include "global.h"
#include "i2cdisplay.h"

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

volatile uint8_t gButtonPressCounter = 0;

// ISR for timer/counter 0: called every 10 ms
// - load counter with initial constant
// - call button check routine

ISR(TIMER0_OVF_vect) {

  TCNT0 = CNT0_PRESET;
  
  // call button check routine
  CheckKeys();

  if ( gRotatorCounter )
    gRotatorCounter--;
  else
    DoRotator();

  if ( gButtonPressCounter )
    gButtonPressCounter--;
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
  
  RS485_PORT &= ~RS485_TX_ENABLE;
  RS485_DDR |= RS485_TX_ENABLE;
  
  // Buttons port initialisation, turn pull-ups on
  mask = BUTTON_PRESET_LEFT | BUTTON_LEFT | BUTTON_STOP | BUTTON_RIGHT | BUTTON_PRESET_RIGHT;

  BUTTON_PORT |= mask;
  BUTTON_DDR &= ~mask;
  
  // initialize USART0 (receiving NMEA messages via RS485 from ACC/MAG sensor)
  
  // initialize USART1, for command/status exchange with HAM op (& debug)
  
  // enable interrupts globally
  sei();
  
  // init I2C interface
  i2c_init();
  
  I2CDisplayBlank();
  I2CDisplayOn();
}
 
// --------------------------------------------------------------------------

typedef enum {

  kSENTENCE_TYPE_UNKNOWN = 0,
  kSENTENCE_TYPE_ACOK,
  kSENTENCE_TYPE_ACERR,
  kSENTENCE_TYPE_ACRAW,
  
} ESentenceType;

static uint8_t gSentenceType = kSENTENCE_TYPE_UNKNOWN;

//static uint8_t gACC_x_raw[7];
//static uint8_t gACC_y_raw[7];
//static uint8_t gACC_z_raw[7];

static uint8_t gMAG_x_raw[5];
static uint8_t gMAG_y_raw[5];
static uint8_t gMAG_z_raw[5];

// inspired by G.Dion's (WhereAVR) MsgHandler() function
//
static uint8_t CompassMessageDecode(uint8_t newchar) {

  static uint8_t commas;			// Number of commas for far in sentence
  static uint8_t index;				// Individual array index

  if ( newchar == 0 ) {				// A NULL character resets decoding
  
    commas = 25;			    	// Set to an outrageous value
    gSentenceType = kSENTENCE_TYPE_UNKNOWN;	// Clear local parse variable
    return FALSE;
  }

  if ( newchar == '$' ) {			// Start of Sentence character, reset
  
    commas = 0; 			    	// No commas detected in sentence for far
    gSentenceType = kSENTENCE_TYPE_UNKNOWN;	// Clear local parse variable
    return FALSE;
  }

  if ( newchar == ',' ) {			// If there is a comma
  
    commas += 1;			    	// Increment the comma count
    index = 0;  			    	// And reset the field index
    return FALSE;
  }

  //if ( newchar == '*' ) {			// If there is a '*' -> checksum
  //  return TRUE;
  //}
  
  if ( commas == 0 ) {
  
    switch ( newchar ) {		       	// "AC" are simply skipped here
      
      case 'E': gSentenceType = kSENTENCE_TYPE_ACERR;
  		break;
      
      case 'O': gSentenceType = kSENTENCE_TYPE_ACOK;
  		break;
      
      case 'R': gSentenceType = kSENTENCE_TYPE_ACRAW;
  		break;
      
      default: break;
    }

    return FALSE;
  }

  if ( gSentenceType == kSENTENCE_TYPE_ACOK ) {         // $ACOK sentence decode initiated
  
  }

  if ( gSentenceType == kSENTENCE_TYPE_ACERR ) {        // $ACERR sentence decode initiated
   
  }

  if ( gSentenceType == kSENTENCE_TYPE_ACRAW ) {	// $ACRAW sentence decode initiated
   
    // example: "$ACRAW,768,-704,-16208,-278,-342,337*E4"
     
    switch ( commas ) {
      //case 1: gACC_x_raw[index++] = newchar; return FALSE;
      //case 2: gACC_y_raw[index++] = newchar; return FALSE;
      //case 3: gACC_z_raw[index++] = newchar; return FALSE;
      case 4: gMAG_x_raw[index++] = newchar; return FALSE;
      case 5: gMAG_y_raw[index++] = newchar; return FALSE;
      case 6: gMAG_z_raw[index++] = newchar; return FALSE;
    }
    
    return FALSE;
  }

}  // end of CompassMessageDecode()

// --------------------------------------------------------------------------

static void CompassMessageInit(void) {

//  for (uint8_t i=0; i<sizeof(gACC_x_raw); ++i) {
//    gACC_x_raw[i] = 0;
//    gACC_y_raw[i] = 0;
//    gACC_z_raw[i] = 0;
//  }

  for (uint8_t i=0; i<sizeof(gMAG_x_raw); ++i) {
    gMAG_x_raw[i] = 0;
    gMAG_y_raw[i] = 0;
    gMAG_z_raw[i] = 0;
  }
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

static uint8_t gLastCommand = kNone;

#define SetCommand(_cmd_) \
 { \
  gLastCommand = gRotatorCommand; \
  gRotatorCommand = _cmd_; \
}

int main(void) {

  // initialize the hardware ...
  InitHardware();
  
  // reset the message decoding engine
  //CompassMessageDecode( 0 );
  //CompassMessageInit();
  
  // display the start message, and leave it for 3 seconds on
  StartMessage(3);
  
  // --- 5 button user interface to rotator control
  
  while ( 1 ) {

    UpdateDisplay();
    
    // --- checks for BUTTON LEFT ---
    
    if ( (gKeyState & BUTTON_LEFT) && !IsRotatorBusy() ) {
      
      SetCommand( kTurnCCW );
    }
    
    if ( (gLastCommand == kTurnCCW) && !(gKeyState & BUTTON_LEFT) ) {

      SetCommand( kStop );
    }
    
    // --- checks for BUTTON RIGHT ---
    
    if ( (gKeyState & BUTTON_RIGHT) && !IsRotatorBusy() ) {
      
      SetCommand( kTurnCW );
    }

    if ( (gLastCommand == kTurnCW) && !(gKeyState & BUTTON_RIGHT) ) {
      
      SetCommand( kStop );
    }
    
    // --- checks for BUTTON STOP ---
    
    if ( gKeyState & BUTTON_STOP ) {
      
      //while ( gKeyState & BUTTON_STOP );
      
      SetCommand( kStop );
      //SetCommand( kFastStop );
    }
    
    // --- checks for BUTTON PRESET LEFT ---
    
    if ( gKeyState & BUTTON_PRESET_LEFT ) {
      
      DecreasePreset();
    }
    
    // --- checks BUTTON PRESET RIGHT ---
    
    if ( gKeyState & BUTTON_PRESET_RIGHT ) {
      
      IncreasePreset();
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
