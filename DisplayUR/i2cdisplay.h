
/*
 * File   : i2cdisplay.h
 *
 * Purpose: Declarations and definitions for the I2C based display on UR's board.
 *
 * $Id: i2cdisplay.h,v 1.2 2012/01/03 15:39:22 mathes Exp $
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

#endif // _i2cdisplay_h_
