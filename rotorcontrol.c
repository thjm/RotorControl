
/*
 * File   : rotorcontrol.c
 *
 * Purpose: Program which performs the rotator control.
 *
 * $Id: rotorcontrol.c,v 1.18 2012/05/18 18:57:18 mathes Exp $
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

#define UART_TX_BUFFER_SIZE 32 
#define UART_RX_BUFFER_SIZE 32

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

typedef struct i_vector {

  int16_t  fX;
  int16_t  fY;
  int16_t  fZ;

} i_vector_t;

// configuration storage in EEPROM

uint16_t gLimitAngle EEMEM = LIMIT_ANGLE;

i_vector_t gMAG_min EEMEM = { -364, -535, -535 };
i_vector_t gMAG_max EEMEM = { 202, -83, -83 };

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
  
  //if ( gPresetCommand != kPresetNone ) 
  PresetExec();
  
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
  
  RS485_PORT &= ~RS485_TX_ENABLE;
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

typedef enum {

  kSENTENCE_TYPE_UNKNOWN = 0,
  kSENTENCE_TYPE_ACOK,
  kSENTENCE_TYPE_ACERR,
  kSENTENCE_TYPE_ACRAW,
  
} ESentenceType;

static uint8_t gSentenceType = kSENTENCE_TYPE_UNKNOWN;

//static char gACC_x_raw[7];
//static char gACC_y_raw[7];
//static char gACC_z_raw[7];

static char gMAG_x_raw[5];
static char gMAG_y_raw[5];
static char gMAG_z_raw[5];

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
 
  if ( newchar == '\n' ) {			// If there is a linefeed character
    gSentenceType = kSENTENCE_TYPE_UNKNOWN;	// Clear local parse variable
    return TRUE;
  }

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

  return FALSE;

}  // end of CompassMessageDecode()

// --------------------------------------------------------------------------

static void CompassMessageConvert(i_vector_t* mag) {

  if ( !mag ) return;

  mag->fX = atoi( gMAG_x_raw );
  mag->fY = atoi( gMAG_y_raw );
  mag->fZ = atoi( gMAG_z_raw );
}

// --------------------------------------------------------------------------

static void CompassMessageInit(void) {

  CompassMessageDecode( 0 );

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

#define SetPresetCommand(_cmd_) \
 { \
  gPresetCommand = _cmd_; \
}

int main(void) {

  // initialize the hardware ...
  InitHardware();
  
  // reset the message decoding engine
  CompassMessageInit();
  
  // display the start message, and leave it for # seconds on
  StartMessage(2);
  
  unsigned int uart_data;
  uint8_t msg_complete;
  
  while ( 1 ) {

   // --- handle serial messages (from ACC/MAG sensor)

   if ( (uart_data = uart_getc()) != UART_NO_DATA ) {

     // HandleUartData( uart_data );

     if ( (uart_data >> 8) == 0) {
       msg_complete = CompassMessageDecode( uart_data & 0xff );
       if ( msg_complete ) {
       }
     }
     else { // UART receive error
     }
   }

   // --- update of direction display

    UpdateDisplay();
    
   // --- 5 button user interface to rotator control
  
    // --- checks for BUTTON CCW ---
    
    if ( (gKeyState & BUTTON_CCW) && !(gKeyState & BUTTON_STOP)
                                   && !IsRotatorBusy() ) {
      
      SetCommand( kTurnCCW );
    }
    
    // was BUTTON CCW released ?
    
    if ( (gLastCommand == kTurnCCW) && !(gKeyState & BUTTON_CCW) ) {

      SetCommand( kStop );
    }
    
    // --- checks for BUTTON CW ---
    
    if ( (gKeyState & BUTTON_CW) && !(gKeyState & BUTTON_STOP)
                                    && !IsRotatorBusy() ) {
      
      SetCommand( kTurnCW );
    }

    // was BUTTON CW released ?
    
    if ( (gLastCommand == kTurnCW) && !(gKeyState & BUTTON_CW) ) {
      
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
