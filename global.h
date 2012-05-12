
/*
 * File   : global.h
 *
 * Purpose: Contains all global definitions of the 'rotorcontrol' project
 *
 * $Id: global.h,v 1.9 2012/05/12 05:43:05 mathes Exp $
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

// 12 MHz crystal ==> CLK/1024 = 11.71875 kHz
// T_0 = 0.0853 msec ==> * 117 = 9.984 msec = T_1
#define CNT0_PRESET             (0xff - 117)

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
#define RELAY_CW                RELAY3    // turn clock wise
#define RELAY_CCW               RELAY2    // turn counter clock wise
#define RELAY_STOP              RELAY1    // open the brake

#define BrakeOn()               { RELAY_PORT &= ~RELAY_STOP; }
#define BrakeOff()              { RELAY_PORT |= RELAY_STOP; }
#define RotatorOn()             { RELAY_PORT |= RELAY_POWER; }
#define RotatorCW()             { RELAY_PORT |= RELAY_CW; } 
#define RotatorCCW()            { RELAY_PORT |= RELAY_CCW; }
#define RotatorOff()            { RELAY_PORT &= ~(RELAY_CW | RELAY_CCW | RELAY_POWER); }

/* --- from file get8key4.c --- */

extern volatile uint8_t gKeyState;
extern volatile uint8_t gKeyPress;

extern void CheckKeys(void);
extern uint8_t GetKeyPress(uint8_t key_mask);
extern uint8_t GetKeyShort(uint8_t key_mask);

#endif /* _global_h_ */
