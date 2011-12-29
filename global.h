
/*
 * File   : global.h
 *
 * Purpose: Contains all global definitions of the 'rotorcontrol' project
 *
 * $Id: global.h,v 1.3 2011/12/29 10:40:35 mathes Exp $
 */


#ifndef _global_h_
#define _global_h_

/** @file global.h
  * Global header file for 'rotorcontrol' project.
  * @author H.-J. Mathes
  */


#include <stdint.h>
#include <avr/io.h>

/* --- my program constants --- */

// 8 MHz internal osc ==> CLK/1024 = 7.8125 kHz
// T_0 = 0.128 msec ==> * 78 = 9.984 msec = T_1
#define CNT0_PRESET             (0xff - 78)

#define BUTTON_PORT             PORTB
#define BUTTON_PIN              PINB
#define BUTTON_DDR              DDRB

#define BUTTON_LEFT             (1<<PB0)
#define BUTTON_PRESET_LEFT      (1<<PB1)
#define BUTTON_STOP             (1<<PB2)
#define BUTTON_PRESET_RIGHT     (1<<PB3)
#define BUTTON_RIGHT            (1<<PB4)

#define RELAY_PORT              PORTA
#define RELAY_DDR               DDRA

#define RELAY_CW                (1<<PA0)
#define RELAY_CCW               (1<<PA1)
#define RELAY_STOP              (1<<PA2)

#define BrakeOn()               { RELAY_PORT &= ~RELAY_STOP; }
#define BrakeOff()              { RELAY_PORT |= RELAY_STOP; }
#define RotatorCW()             { RELAY_PORT |= RELAY_CW; } 
#define RotatorCCW()            { RELAY_PORT |= RELAY_CCW; }
#define RotatorOff()            { RELAY_PORT &= ~(RELAY_CW | RELAY_CCW); }

/* --- declaration of global vars: file get8key4.c --- */

extern volatile uint8_t gKeyState;
extern volatile uint8_t gKeyPress;

extern void CheckKeys(void);
extern uint8_t GetKeyPress(uint8_t key_mask);
extern uint8_t GetKeyShort(uint8_t key_mask);

#endif /* _global_h_ */
