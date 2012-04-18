
/*
 * File   : global.h
 *
 * Purpose: Contains all global definitions of the 'rotorcontrol' project
 *
 * $Id: global.h,v 1.7 2012/04/18 19:45:43 mathes Exp $
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

#define BUTTON_PORT             PORTA
#define BUTTON_PIN              PINA
#define BUTTON_DDR              DDRA

#define BUTTON_PRESET_LEFT      (1<<PA3)
#define BUTTON_LEFT             (1<<PA1)
#define BUTTON_STOP             (1<<PA0)
#define BUTTON_RIGHT            (1<<PA2)
#define BUTTON_PRESET_RIGHT     (1<<PA4)

#define LED_PORT                PORTC
#define LED_DDR                 DDRC

#define LED_CALIBRATE           (1<<PC2)  // green
#define LED_OVERLOAD            (1<<PC3)  // yellow
#define LED_LEFT                (1<<PC4)
#define LED_RIGHT               (1<<PC5)

#define RS485_TX_ENABLE         (1<<PC6)

#define RS485EnableRx()         { LED_PORT &= ~RS485_TX_ENABLE; }
#define RS485EnableTx()         { LED_PORT |= RS485_TX_ENABLE; }

#define RELAY_PORT              PORTD
#define RELAY_DDR               DDRD

#define RELAY_POWER             (1<<PD4)  // turn on rotor power
#define RELAY_CW                (1<<PD5)  // turn clock wise
#define RELAY_CCW               (1<<PD6)  // turn counter clock wise
#define RELAY_STOP              (1<<PD7)  // open the brake

#define BrakeOn()               { RELAY_PORT &= ~RELAY_STOP; }
#define BrakeOff()              { RELAY_PORT |= RELAY_STOP; }
#define RotatorOn()             { RELAY_PORT |= RELAY_POWER; }
#define RotatorCW()             { RELAY_PORT |= RELAY_CW; } 
#define RotatorCCW()            { RELAY_PORT |= RELAY_CCW; }
#define RotatorOff()            { RELAY_PORT &= ~(RELAY_CW | RELAY_CCW | RELAY_POWER); }

/* --- declaration of global vars: file get8key4.c --- */

extern volatile uint8_t gKeyState;
extern volatile uint8_t gKeyPress;

extern void CheckKeys(void);
extern uint8_t GetKeyPress(uint8_t key_mask);
extern uint8_t GetKeyShort(uint8_t key_mask);

#endif /* _global_h_ */
