
/*
 * File   : i2cdisplay.h
 *
 * Purpose: Declarations and definitions for the I2C based display on UR's board.
 *
 * $Id: i2cdisplay.h,v 1.1 2011/12/27 19:34:42 mathes Exp $
 *
 */


/** @file i2cdisplay.h
  * Declarations and definitions for the I2C based display on UR's board.
  * @author H.-J.Mathes <dc2ip@darc.de>
  */

#ifndef _i2cdisplay_h_
#define _i2cdisplay_h_

/** Address if the I2C slave */
#define I2C_DISPLAY      0x50

// I2C protocol definition
//
// byte 01 : command
#define I2C_DISP_OFF            0x00
#define I2C_DISP_ON             0x01
#define I2C_DISP_DATA           0x02
#define I2C_DISP_DATA_LEFT      0x03
#define I2C_DISP_DATA_RIGHT     0x04
#define I2C_DISP_RAWDATA        0x04
#define I2C_DISP_RAWDATA_LEFT   0x05
#define I2C_DISP_RAWDATA_RIGHT  0x06
//
// byte 02 : data for left digit, lsb
// byte 03 : data for left digit, msb
// byte 04 : data for right digit, lsb
// byte 05 : data for right digit, msb
//

#endif // _i2cdisplay_h_
