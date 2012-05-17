
/*
 * File   : global.h
 *
 * Purpose: Contains all global definitions of the 'rotorcontrol' project
 *
 * $Id: global.h,v 1.17 2012/05/17 12:31:51 mathes Exp $
 */


#ifndef _global_h_
#define _global_h_

/** @file global.h
  * Global header file for 'rotorcontrol' project.
  * @author H.-J. Mathes
  */


#include <stdint.h>
#include <avr/io.h>

#ifndef TRUE
 #define TRUE 	(1==1)
#endif /* TRUE */
#ifndef FALSE
 #define FALSE  (1==0)
#endif /* FALSE */
 
/* --- my program constants --- */

// 12 MHz crystal ==> CLK/1024 = 11.71875 kHz
// T_0 = 0.0853 msec ==> * 117 = 9.984 msec = T_1
#define CNT0_PRESET             (0xff - 117)

#define BUTTON_PORT             PORTA
#define BUTTON_PIN              PINA
#define BUTTON_DDR              DDRA

#define BUTTON_PRESET_LEFT      (1<<PA4)
#define BUTTON_LEFT             (1<<PA2)
#define BUTTON_STOP             (1<<PA0)
#define BUTTON_RIGHT            (1<<PA1)
#define BUTTON_PRESET_RIGHT     (1<<PA3)

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

/* ---  -- */

extern volatile uint8_t gButtonPressCounter;

/* --- declaration(s) for file get8key4.c --- */

extern volatile uint8_t gKeyState;
extern volatile uint8_t gKeyPress;

extern void CheckKeys(void);
extern uint8_t GetKeyPress(uint8_t key_mask);
extern uint8_t GetKeyShort(uint8_t key_mask);

/* --- declaration(s) for file rotorstate.c --- */

/**  */
extern void DoRotator(void);
/**  */
extern void UpdateDisplay(void);

/**  */
extern void IncreasePreset(void);
extern void DecreasePreset(void);

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

/** Minimum and maximum angles. */
#define MIN_ANGLE        0
#define MAX_ANGLE      359

#endif /* _global_h_ */
