
/*
 * File   : LSM303DLH.h
 *
 * $Id: LSM303DLH.h,v 1.4 2012/05/20 11:10:50 mathes Exp $
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        $Revision: 1.4 $ $Date: 2012/05/20 11:10:50 $
 * Description:    Header file for LSM303DLH.c.
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


#ifndef _LSM303DLH_h_
#define _LSM303DLH_h_

#include <stdint.h>

/** LSM303DLH I2C register definitions. */
#define I2C_DEV_LSM303DLH_ACC1	0x30
#define I2C_DEV_LSM303DLH_ACC2	0x31
#define I2C_DEV_LSM303DLH_MAG	0x3C

/** LSM303DLH error codes (I2C error codes) */
#define I2C_NO_ERROR		0
#define I2C_ERR_NO_DEVICE	1
#define I2C_ERROR	      127

/** Register definitions for LSM303DLH accelerometer and magnetormeter. */
#define CTRL_REG1_A	     0x20
#define CTRL_REG2_A	     0x21
#define CTRL_REG3_A	     0x22
#define CTRL_REG4_A	     0x23
#define CTRL_REG5_A	     0x24
#define HP_FILTER_RESET_A    0x25
#define REFERENCE_A          0x26
#define STATUS_REG_A	     0x27
#define OUT_X_L_A	     0x28
#define OUT_X_H_A	     0x29
#define OUT_Y_L_A	     0x2A
#define OUT_Y_H_A	     0x2B
#define OUT_Z_L_A	     0x2C
#define OUT_Z_H_A	     0x2D
/* addresses 0x2E .. 0x2F are reserved */

#define INT1_CFG_A	     0x30
#define INT1_SOURCE_A 	     0x31
#define INT1_THS_A           0x32
#define INT1_DURATION_A      0x33
#define INT2_CFG_A           0x34
#define INT2_SOURCE_A        0x35
#define INT2_THS_A           0x36
#define INT2_DURATION_A      0x37
/* addresses 0x38 .. 0x3F are reserved */

#define CRA_REG_M	     0x00
#define CRB_REG_M	     0x01
#define MR_REG_M	     0x02
#define OUT_X_H_M	     0x03
#define OUT_X_L_M	     0x04
#define OUT_Y_H_M	     0x05
#define OUT_Y_L_M	     0x06
#define OUT_Z_H_M	     0x07
#define OUT_Z_L_M	     0x08
#define SR_REG_M	     0x09
#define IRA_REG_M	     0x0A
#define IRB_REG_M	     0x0B
#define IRC_REG_M	     0x0C

/** Data structure whhich represents read sensor data. */
typedef struct _LSM303DLHData {

  int16_t fSensorX;
  int16_t fSensorY;
  int16_t fSensorZ;
  
} LSM303DLHData;

/** Initialize accelerometer sensor of LSM303DLH at I2C address acc_addr. */
extern int8_t LSM303DLHInitACC(uint8_t acc_addr);

/** Initialize magnetometer sensor of LSM303DLH at I2C address mag_addr. */
extern int8_t LSM303DLHInitMAG(uint8_t mag_addr);

/** Read the accelerometer sensor data. */
extern int8_t LSM303DLHReadACC(uint8_t acc_addr,LSM303DLHData* data);

/** Read the magnetometer sensor data. */
extern int8_t LSM303DLHReadMAG(uint8_t mag_addr,LSM303DLHData* data);

/** Write value 'data' to the specified LSM303DLH register 'reg'. */
extern int8_t LSM303DLHWrite(uint8_t addr,uint8_t reg,uint8_t data);

#endif /* _LSM303DLH_h_ */
