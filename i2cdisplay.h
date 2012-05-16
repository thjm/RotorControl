
/*
 * File   : i2cdisplay.h
 *
 * Purpose: Declarations and definitions for the client interface to
 *          I2C based display on UR's board.
 *
 * $Id: i2cdisplay.h,v 1.1 2012/05/16 16:06:49 mathes Exp $
 *
 */


/** @file i2cdisplay.h
  * Declarations and definitions for the client interface to
  *              I2C based display on UR's board.
  * @author H.-J.Mathes <dc2ip@darc.de>
  */

#ifndef _i2cdisplay_h_
#define _i2cdisplay_h_

#include <stdint.h>

#include <DisplayUR/i2cdisplaydefs.h>

/** Empty the display buffer of the I2C display */
extern void I2CDisplayBlank(void);

/** Switch I2C display multiplexing on. */
extern void I2CDisplayOn(void);
/** Switch I2C display multiplexing off. */
extern void I2CDisplayOff(void);

/** Write string to segment buffer from PROGMEM. */
extern uint8_t I2CDisplayWrite_p(uint8_t l_msg,const uint8_t *p_msg);
/** Write string to segment buffer. */
extern uint8_t I2CDisplayWrite(uint8_t l_msg,const uint8_t *p_msg);

/** Write data to left group of digits of the I2C display. */
extern uint8_t I2CDisplayWriteLData(int16_t data);
/** Write data to right group of digits of the I2C display. */
extern uint8_t I2CDisplayWriteRData(int16_t data);
/** Write data to the I2C display (both groups). */
extern uint8_t I2CDisplayWriteData(int16_t l_data,int16_t r_data);

#endif /* _i2cdisplay_h_ */
