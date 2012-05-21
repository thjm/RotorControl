
/*
 * File   : i2cdisplaydefs.h
 *
 * $Id: i2cdisplaydefs.h,v 1.2 2012/05/21 05:32:28 mathes Exp $
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        $Revision: 1.2 $ $Date: 2012/05/21 05:32:28 $
 * Description:    Declarations and definitions for the I2C based 
 *                 display on UR's board.
 *
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


/** @file i2cdisplaydefs.h
  * Declarations and definitions for the I2C based display on UR's board.
  * @author H.-J.Mathes <dc2ip@darc.de>
  */

#ifndef _i2cdisplaydefs_h_
#define _i2cdisplaydefs_h_

/** Address if the I2C slave */
#define I2C_DISPLAY      0x50

// I2C protocol definition
//
// byte 01 : command
#define I2C_DISP_OFF            0x00
#define I2C_DISP_ON             0x01
#define I2C_DISP_DATA           0x02
//
// byte 02 : data for left digit, lsb
// byte 03 : data for left digit, msb
// byte 04 : data for right digit, lsb
// byte 05 : data for right digit, msb
//
#define I2C_DISP_DATA_LEFT      0x03
//
// byte 02 : data for left digit, lsb
// byte 03 : data for left digit, msb
//
#define I2C_DISP_DATA_RIGHT     0x04
//
// byte 02 : data for right digit, lsb
// byte 03 : data for right digit, msb
//
#define I2C_DISP_RAWDATA        0x05
//
// byte 02 : raw data for left most digit, = digit 01
// byte 03 : raw data for digit 02
// byte 04 : raw data for digit 03
// byte 05 : raw data for digit 04
// byte 06 : raw data for digit 05
// byte 07 : raw data for right most digit, = digit 06
//
#define I2C_DISP_RAWDATA_LEFT   0x06
//
// byte 02 : raw data for left most digit, = digit 01
// byte 03 : raw data for digit 02
// byte 04 : raw data for digit 03
//
#define I2C_DISP_RAWDATA_RIGHT  0x07
//
// byte 02 : raw data for digit 04
// byte 03 : raw data for digit 05
// byte 04 : raw data for right most digit, = digit 06
//

#endif // _i2cdisplaydefs_h_
