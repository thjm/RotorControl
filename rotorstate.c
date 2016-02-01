
/*
 * File   : rotorstate.c
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        Version v1r0
 * Description:    State machine for the rotator control program.
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

/** @file rotorstate.c
  * State machine for the rotator control program.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <i2cmaster.h>   // P.Fleury's lib

#include "global.h"

#include <uart.h>

#include "i2cdisplay.h"
#include "num2uart.h"

volatile uint8_t gRotatorBusy = 0;
volatile uint8_t gRotatorCommand = kNone;
volatile uint8_t gRotatorState = kIdle;
volatile uint8_t gRotatorBusyCounter = 0;

#define SetBusy(_busy_) { gRotatorBusy = _busy_; }

volatile static int16_t gCurrentHeading = 0;
volatile static int16_t gPresetHeading = 0;

#define PRESET_COUNTER_MAX           32
#define PRESET_COUNTER_MIN           4

volatile uint8_t gPresetCommand = kPresetNone;
static uint8_t gPresetCounterStart = PRESET_COUNTER_MAX;
volatile uint8_t gPresetCounter = 0;

/* local prototypes */
static uint8_t GetDirection(uint16_t cur_heading,uint16_t nom_heading);

// --------------------------------------------------------------------------

/** The state machine of the rotor control engine ...
 *
 * This is the transition diagram after the command kRotateCW has been issued:
 *
 * kRotateCW -> kIdle
 *                |
 *                | ==> PowerOn()
 *               \/
 *              kReleaseBrake
 *                |
 *       kStop -> | -------- >>> -------+
 *                | ==> ReleaseBrake()  |
 *                |                     |
 *               \/                     |
 *              kRotorRampup            |
 *                |                     |
 *       kStop -> | -------- >>> -------|--+
 *                | ==> RotateCW()      |  |
 *                |                     |  |
 *               \/                     |  |
 *              kTurningCW              |  |
 *                |                     |  |
 *                * <-------------------+  |
 *       kStop -> |                        |
 *                | ==> RotorOff()         |
 *                |                        |
 *               \/                        |
 *              kLockBrake                 |
 *                |                        |
 *                * <----------------------+
 *                |
 *                | ==> LockBrake()
 *                |
 *               \/
 *              kRotorRampdown
 *                |
 *                | ==> PowerOff()
 *                |
 *               \/
 *              kIdle
 *
 * It will be handled in a similar way for the command kRotateCCW.
 *
 * As the trasitions will take some time, it is not unlikely, that the kStop 
 * event is issued before the state kTurningCW is reached. In that case the 
 * rotor relays etc. must be switched off in a proper order.
 */
void RotatorExec(void) {

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
         
         // both PRESET LEDs off
         LED_PORT &= ~(LED_LEFT | LED_RIGHT);
	 
	 SetBusy(0);
      
         // clear preset data
         gPresetHeading = gCurrentHeading;
	 gPresetCounter = 0;
	 gPresetCommand = kPresetNone;
         break;

    default:
         break;
  }
}

// --------------------------------------------------------------------------

static uint8_t gLastCommand = kNone;

void SetCommand(uint8_t cmd) {

  gLastCommand = gRotatorCommand;
  gRotatorCommand = cmd;
}

// --------------------------------------------------------------------------

uint8_t GetLastCommand(void) {

  return gLastCommand;
}

// --------------------------------------------------------------------------

volatile uint16_t gPresetDisplayCounter = 0;

static uint16_t gCurrentHeadingOld = 999;
static uint16_t gPresetHeadingOld = 999;

// called by main()
void UpdateDisplay(void) {

  static uint8_t ret = 0;
  
  if ( gCurrentHeadingOld != gCurrentHeading ) {
  
    gCurrentHeadingOld = gCurrentHeading;
    
    if ( ret ) {
      i2c_init();
      
      _delay_ms(10.0);
    }
    
    ret = I2CDisplayWriteLData( gCurrentHeading );
    
    _delay_ms(1.0);
  }
  
  if ( gPresetHeadingOld != gPresetHeading ) {
    
    gPresetHeadingOld = gPresetHeading;
    
    if ( ret ) {
      i2c_init();
      
      _delay_ms(10.0);
    }

    ret = I2CDisplayWriteRData( gPresetHeading );
    
    _delay_ms(1.0);

    // 'PRESET' display should vanish after 5 sec if both are equal
    if ( gCurrentHeading == gPresetHeading ) {
    
      // atomic set uint16_t type variable
      cli();
       gPresetDisplayCounter = 500;  
      sei();
    }
  }
  
  // 'PRESET' display should vanish after 5 sec if both are equal
  if ( gCurrentHeading == gPresetHeading ) {
    
    if ( gPresetDisplayCounter == 0 )
      //I2CDisplayWriteR( 3, (uint8_t*)"\000\000\000" );  // blank display
      I2CDisplayWriteR( 3, (uint8_t*)"\010\010\010" );    // "---"
  }
}

// --------------------------------------------------------------------------

// is called for every 10ms
void PresetExec(void) {

  // nothing to do here...
  if ( gPresetCommand == kPresetNone ) return;
  
  static uint16_t preset_duration = 0;
  
  // 'stop' requested, set everything to start values
  
  if ( gPresetCommand == kPresetStop ) {
  
    gPresetCounter = 0;
    preset_duration = 0;
    gPresetCounterStart = PRESET_COUNTER_MAX;

    gPresetCommand = kPresetExec;
    
    return;
  }
  
  if ( preset_duration > 200 ) {
    
    preset_duration = 0;
    
    if ( gPresetCounterStart > PRESET_COUNTER_MIN )
      gPresetCounterStart /= 2;
  }
  
  preset_duration++;
  
  if ( gPresetCounter-- != 0 ) return;
  
  switch ( gPresetCommand ) {
  
    case kPresetCW:
         gPresetHeading++;
         if ( gPresetHeading > MAX_ANGLE )
           gPresetHeading = MIN_ANGLE;
	 break;
  
    case kPresetCCW:
         gPresetHeading--;
         if ( gPresetHeading < MIN_ANGLE )
           gPresetHeading = MAX_ANGLE;
         break;
  }
  
  gPresetCounter = gPresetCounterStart;
  
#if 0
  PresetExecSlow();
#endif
}

// --------------------------------------------------------------------------

void PresetExecSlow(void) {

  if ( gPresetCommand == kNone ) return;
  
  if (    gPresetCommand != kPresetCW && gPresetCommand != kPresetCCW
       && gPresetCommand != kPresetExec ) return;

  if ( abs(gCurrentHeading - gPresetHeading) < 5 ) return; // do nothing

  uint8_t cmd;

//  if ( abs(gCurrentHeading - gPresetHeading) < 5 ) 
//    cmd = kNone;
//  else
    cmd = GetDirection( gCurrentHeading, gPresetHeading );
  
  switch ( cmd ) {

    case kTurnCW:
  	 LED_PORT |= LED_RIGHT;
  	 SetCommand( kTurnCW );
  	 break;

    case kTurnCCW:
  	 LED_PORT |= LED_LEFT;
  	 SetCommand( kTurnCCW );
  	 break;

    case kStop:
    case kNone:
    default:
  	 SetCommand( kStop );
         gPresetCommand = kPresetNone;
         
  	 // both PRESET LEDs off
  	 LED_PORT &= ~(LED_LEFT | LED_RIGHT);
  }
}

// --------------------------------------------------------------------------

void SetPresetCommand(uint8_t cmd) {

  gPresetCommand = cmd;
}

// --------------------------------------------------------------------------

void SetCurrentHeading(int heading) {

  gCurrentHeading = heading;
  
  if ( gPresetCommand == kPresetNone ) {
    gPresetHeading = heading;
    gPresetHeadingOld = heading;
  }
}

// --------------------------------------------------------------------------

/** This function calculates the direction into which the rotator has to be
  * turned from the 'actual direction' to reach the 'nominal direction'.
  *
  * It takes the position of the mechanical limitation into account 
  * (LIMIT_ANGLE).
  *
  * This is how it works:
  * In a configuration where the limit is at 0 degrees (north), we have to
  * @li rotate CCW if nominal direction < actual direction
  * @li rotate CW if nominal direction > actual direction
  * @li do nothing else.
  * Thus, if we first rotate our system back into this simple configuration, we
  * easily can decide what to do. The rotation angle is 360 degrees (MAX_ANGLE)
  * minus the angular position of the bracket, it must always be a positive 
  * value.
  *
  * Additionally, the angles obtained must be normalized, i.e. must be below 
  * 360 degrees.
  *
  * @return either kTurnCW or kTurnCCW or kNone if no action is required.
  *
  * Note: This may not work for configurations where the total angular range is
  * larger than 360 degrees.
  */
static uint8_t GetDirection(uint16_t cur_heading,uint16_t nom_heading) {

  uint16_t rotation_angle = MAX_ANGLE - LIMIT_ANGLE;
  
  uint16_t cur_heading_rotated = cur_heading + rotation_angle;
  if ( cur_heading_rotated > MAX_ANGLE ) cur_heading_rotated -= MAX_ANGLE;
  
  uint16_t nom_heading_rotated = nom_heading + rotation_angle;
  if ( nom_heading_rotated > MAX_ANGLE ) nom_heading_rotated -= MAX_ANGLE;
  
  if ( nom_heading_rotated < cur_heading_rotated )
    return kTurnCCW;
  else if ( nom_heading_rotated > cur_heading_rotated )
    return kTurnCW;
  else
    return kNone;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
