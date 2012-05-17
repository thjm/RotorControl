
/*
 * File   : rotorstate.c
 *
 * Purpose: State machine for the rotator control program.
 *
 * $Id: rotorstate.c,v 1.5 2012/05/17 12:31:51 mathes Exp $
 *
 */
 
#include <stdint.h>
#include <stdlib.h>

/** @file rotorstate.c
  * State machine for the rotator control program.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <avr/io.h>
#include <util/delay.h>

#include <i2cmaster.h>   // P.Fleury's lib

#include "global.h"
#include "i2cdisplay.h"

volatile uint8_t gRotatorBusy = 0;
volatile uint8_t gRotatorCommand = kNone;
volatile uint8_t gRotatorState = kIdle;
volatile uint8_t gRotatorBusyCounter = 0;

#define SetBusy(_busy_) { gRotatorBusy = _busy_; }

static int16_t gCurrentDirection = 0;
static int16_t gPresetDirection = 0;

// --------------------------------------------------------------------------

void DoRotator(void)
 {
  switch ( gRotatorCommand ) {
  
    case kStop:
         
	 SetBusy(1);
	 
	 switch ( gRotatorState ) {
	 
	   case kReleaseBrake: // ???
	   case kTurningCW:
	   case kTurningCCW:
	        RotatorOff();
		gRotatorBusyCounter = 5;
		gRotatorState = kLockBrake;
	        break;

	   case kRotorRampup: // ???
	   case kLockBrake:
	        BrakeLock();
		gRotatorBusyCounter = 5;
		gRotatorState = kRotorRampdown;
	        break;

	   case kRotorRampdown:
		PowerOff();
	        gRotatorState = kIdle;
		gRotatorBusyCounter = 10;
	        break;

	   case kIdle:
		gRotatorCommand = kNone;
		SetBusy(0);
	        break;
	 }

         break;

    case kTurnCW:
         
	 SetBusy(1);
	 
	 switch ( gRotatorState ) {
	 
	   case kIdle: 
	        PowerOn();
		gRotatorState = kReleaseBrake;
		gRotatorBusyCounter = 10;
		break;
	 
	   case kReleaseBrake:
	        BrakeRelease();
		gRotatorState = kRotorRampup;
		gRotatorBusyCounter = 5;
		break;
	 
	   case kRotorRampup: 
		gRotatorState = kTurningCW;
         	RotatorCW();
		gRotatorBusyCounter = 5;
		break;
	   
	   case kTurningCW:
		gRotatorState = kTurningCW;
	        gRotatorCommand = kNone;
		break;
	 }
	
         break;

    case kTurnCCW:
         
	 SetBusy(1);
	 
	 switch ( gRotatorState ) {
	 
	   case kIdle: 
	        PowerOn();
		gRotatorState = kReleaseBrake;
		gRotatorBusyCounter = 10;
		break;
	 
	   case kReleaseBrake:
	        BrakeRelease();
		gRotatorState = kRotorRampup;
		gRotatorBusyCounter = 5;
		break;
	 
	   case kRotorRampup: 
		gRotatorState = kTurningCCW;
         	RotatorCCW();
		gRotatorBusyCounter = 5;
		break;
	   
	   case kTurningCCW:
		gRotatorState = kTurningCCW;
	        gRotatorCommand = kNone;
		break;
	 }

         break;

    case kFastStop:
         RotatorOff();
         BrakeLock();
	 PowerOff();
	 
	 // reset state machine and command variable
	 gRotatorState = kIdle;
	 gRotatorCommand = kNone;
         
	 SetBusy(0);
      
         // clear preset data
         gPresetDirection = gCurrentDirection;
         break;

    default:
         break;
  }
}

// --------------------------------------------------------------------------

void UpdateDisplay(void) {

  static uint16_t old_cur_dir = 999;
  
  static uint8_t ret = 0;
  
  if ( old_cur_dir != gCurrentDirection ) {
  
    old_cur_dir = gCurrentDirection;
    
    if ( ret ) {
      i2c_init();
      
      _delay_ms(10.0);
    }
    
    ret = I2CDisplayWriteLData( gCurrentDirection );
    
    _delay_ms(1.0);
  }
  
  static uint16_t old_preset_dir = 999;
  
  if ( old_preset_dir != gPresetDirection ) {
    
    old_preset_dir = gPresetDirection;
    
    if ( ret ) {
      i2c_init();
      
      _delay_ms(10.0);
    }

    ret =I2CDisplayWriteRData( gPresetDirection );
    
    _delay_ms(1.0);
  }
}

// --------------------------------------------------------------------------

void IncreasePreset(void) {

  if ( gButtonPressCounter == 0 ) {

    gPresetDirection++;
    if ( gPresetDirection > MAX_ANGLE )
      gPresetDirection = MIN_ANGLE;

    gButtonPressCounter = 30;
  }
}

// --------------------------------------------------------------------------

void DecreasePreset(void) {

  if ( gButtonPressCounter == 0 ) {

    gPresetDirection--;
    if ( gPresetDirection < MIN_ANGLE )
      gPresetDirection = MAX_ANGLE;

    gButtonPressCounter = 30;
  }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
