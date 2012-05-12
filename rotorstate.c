
/*
 * File   : rotorstate.c
 *
 * Purpose: State machine for the rotator control program.
 *
 * $Id: rotorstate.c,v 1.1 2012/05/12 11:11:22 mathes Exp $
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

volatile uint8_t gRotatorState = kIdle;

// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
