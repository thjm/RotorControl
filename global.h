
/*
 * File   : global.h
 *
 * $Id: global.h,v 1.26 2014/03/23 07:30:35 mathes Exp $
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        $Revision: 1.26 $ $Date: 2014/03/23 07:30:35 $
 * Description:    Contains all global definitions of the 'rotorcontrol'
 *                 project.
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


#ifndef _global_h_
#define _global_h_

/** @file global.h
  * Global header file for 'rotorcontrol' project.
  * @author H.-J. Mathes
  */


#include <stdint.h>
#include <avr/io.h>
#include <avr/eeprom.h>

#ifndef TRUE
 #define TRUE 	(1==1)
#endif /* TRUE */
#ifndef FALSE
 #define FALSE  (1==0)
#endif /* FALSE */

#include "vector.h"

/* --- for the UART library of P.Fleury --- */

//#define UART_TX_BUFFER_SIZE	128
//#define UART_RX_BUFFER_SIZE	128

// ==> uart.h must be included afterwards !!!

/* --- my program constants --- */

// 12 MHz crystal ==> CLK/1024 = 11.71875 kHz
// T_0 = 0.0853 msec ==> * 117 = 9.984 msec = T_1
#define CNT0_PRESET             (0xff - 117)

#define BUTTON_PORT             PORTA
#define BUTTON_PIN              PINA
#define BUTTON_DDR              DDRA

#define BUTTON_PRESET_CCW       (1<<PA4)
#define BUTTON_CCW              (1<<PA2)
#define BUTTON_STOP             (1<<PA0)
#define BUTTON_CW               (1<<PA1)
#define BUTTON_PRESET_CW        (1<<PA3)

#define LED_PORT                PORTC
#define LED_DDR                 DDRC

#define LED_OVERLOAD            (1<<PC2)  // red
#define LED_CALIBRATE           (1<<PC3)  // yellow
#define LED_LEFT                (1<<PC4)
#define LED_RIGHT               (1<<PC5)

#define RS485_PORT              LED_PORT
#define RS485_DDR               LED_DDR

#define RS485_TX_ENABLE         (1<<PC6)

#define RS485EnableRx()         { RS485_PORT &= ~RS485_TX_ENABLE; }
#define RS485EnableTx()         { RS485_PORT |= RS485_TX_ENABLE; }

#define RELAY_PORT              PORTD
#define RELAY_DDR               DDRD

#define RELAY1                  (1<<PD7)  // generic names, as in schematic
#define RELAY2                  (1<<PD6)
#define RELAY3                  (1<<PD5)
#define RELAY4                  (1<<PD4)

#define RELAY_POWER             RELAY4    // turn on rotor power
#define RELAY_STOP              RELAY3    // open the brake
#define RELAY_CCW               RELAY2    // turn counter clock wise
#define RELAY_CW                RELAY1    // turn clock wise

#define BrakeLock()             { RELAY_PORT &= ~RELAY_STOP; }
#define BrakeRelease()          { RELAY_PORT |= RELAY_STOP; }
#define PowerOn()               { RELAY_PORT |= RELAY_POWER; }
#define PowerOff()              { RELAY_PORT &= ~RELAY_POWER; }
#define RotatorCW()             { RELAY_PORT |= RELAY_CW; }
#define RotatorCCW()            { RELAY_PORT |= RELAY_CCW; }
#define RotatorOff()            { RELAY_PORT &= ~(RELAY_CW | RELAY_CCW); }

/** vector like data type with int16_t components. */
typedef struct i_vector {

  int16_t  x;
  int16_t  y;
  int16_t  z;

} i_vector_t;

/* --- configuration data in EEPROM --- */

//extern i_vector_t EEMEM gEE_MAG_min;
//extern i_vector_t EEMEM gEE_MAG_max;
extern vector_t EEMEM gEE_MAG_min;
extern vector_t EEMEM gEE_MAG_max;

/* --- declaration(s) for file get8key4.c --- */

extern volatile uint8_t gKeyState;
extern volatile uint8_t gKeyPress;

extern void CheckKeys(void);
extern uint8_t GetKeyPress(uint8_t key_mask);
extern uint8_t GetKeyShort(uint8_t key_mask);

/* --- declaration(s) for file rotorstate.c --- */

/** Set the current heading (for the display).
  *
  * If we are not in 'preset' mode, the gPresetHeading is also modified.
  */
extern void SetCurrentHeading(int);

/**  */
extern void SetPresetHeading(int);

/**  */
extern void RotatorExec(void);

extern volatile uint8_t gRotatorBusy;
extern volatile uint8_t gRotatorCommand;
extern volatile uint8_t gRotatorState;
extern volatile uint8_t gRotatorBusyCounter;

#define IsRotatorBusy() (gRotatorBusy != 0)

/** States the rotor control might be in. */

typedef enum {

  kIdle,
  kReleaseBrake,
  kLockBrake,
  kRotorRampup,
  kRotorRampdown,
  kTurningCCW,
  kTurningCW,

} ERotorState;

/** The commands to the rotor control. */
typedef enum {

  kNone,
  kStop,
  kTurnCCW,
  kTurnCW,
  kFastStop,

} ERotorCommand;

/**  */
extern void SetCommand(uint8_t cmd);
/**  */
extern uint8_t GetLastCommand(void);

/**  */
extern void SetPresetCommand(uint8_t cmd);

/**  */
extern void PresetExec(void);
extern void PresetExecSlow(void);
/**  */
extern void UpdateDisplay(void);

/** Minimum and maximum angles. */
#define MIN_ANGLE        0
#define MAX_ANGLE      359

/** A mechanical stop where we cannot rotate further. */
#define LIMIT_ANGLE    270

typedef enum {

  kPresetNone = 0,
  kPresetCW,
  kPresetCCW,
  kPresetStop,
  kPresetExec,

} EPresetCommand;

/** Command to the 'preset' logic. */
extern volatile uint8_t gPresetCommand;
extern volatile uint8_t gPresetCounter;

extern volatile uint16_t gPresetDisplayCounter;

/* --- declaration(s) for file compass.c --- */

extern void CompassInit(void);
extern void CompassMessageInit(void);
extern void CompassMessageReceive(unsigned int uart_data);

#endif /* _global_h_ */
