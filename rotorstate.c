
/*
 * File   : rotorstate.c
 *
 * Purpose: State machine for the rotator control program.
 *
 * $Id: rotorstate.c,v 1.2 2012/05/13 11:20:49 mathes Exp $
 *
 */
 
#include <stdint.h>
#include <stdlib.h>

/** @file rotorstate.c
  * State machine for the rotator control program.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <avr/io.h>

#include "global.h"

volatile uint8_t gRotatorCommand = kNone;
volatile uint8_t gRotatorState = kIdle;
volatile uint8_t gRotatorCounter = 0;

// --------------------------------------------------------------------------

void DoRotator(void)
 {
  switch ( gRotatorState ) {
  
    case kIdle: return;
    
    case kReleaseBreak:
         break;
    
    case kLockBreak:
         break;
    
    case kRotorRampup:
         break;
    
    case kRotorRampdown:
         break;
    
    case kTurningCCW:
         break;
    
    case kTurningCW:
         break;
  }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
