
/*
 * File   : i2cdisplay.h
 *
 * $Id: i2cdisplay.h,v 1.3 2012/05/21 05:30:15 mathes Exp $
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        $Revision: 1.3 $ $Date: 2012/05/21 05:30:15 $
 * Description:    Declarations and definitions for the client interface to
 *                 I2C based display on UR's board.
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
/** Write string to segment buffer, left group. */
extern uint8_t I2CDisplayWriteL(uint8_t l_msg,const uint8_t *p_msg);
/** Write string to segment buffer, right group. */
extern uint8_t I2CDisplayWriteR(uint8_t l_msg,const uint8_t *p_msg);

/** Write data to left group of digits of the I2C display. */
extern uint8_t I2CDisplayWriteLData(int16_t data);
/** Write data to right group of digits of the I2C display. */
extern uint8_t I2CDisplayWriteRData(int16_t data);
/** Write data to the I2C display (both groups). */
extern uint8_t I2CDisplayWriteData(int16_t l_data,int16_t r_data);

#endif /* _i2cdisplay_h_ */
