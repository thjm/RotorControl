
/*
 * File   : global.h
 *
 * Purpose: Contains all global definitions of the 'rotorcontrol' project
 *
 * $Id: global.h,v 1.1 2011/12/27 20:00:16 mathes Exp $
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

#define BUTTON_PORT             PORTB
#define BUTTON_PIN              PINB
#define BUTTON_DDR              DDRB

#define BUTTON_LEFT             (1<<PB0)
#define BUTTON_PRESET_LEFT      (1<<PB1)
#define BUTTON_STOP             (1<<PB2)
#define BUTTON_PRESET_RIGHT     (1<<PB3)
#define BUTTON_RIGHT            (1<<PB4)

/* --- declaration of global vars: file get8key4.c --- */

extern volatile uint8_t gKeyState;
extern volatile uint8_t gKeyPress;

extern void CheckKeys(void);
extern uint8_t GetKeyPress(uint8_t key_mask);
extern uint8_t GetKeyShort(uint8_t key_mask);

#endif /* _global_h_ */
