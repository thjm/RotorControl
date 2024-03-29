
/*
 * File   : LSM303DLH.c
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        v1r0
 * Description:    Implementation file for LSM303DLH specific routines.
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

#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>

/** @file LSM303DLH.c
  * Implementation file for LSM303DLH specific routines.
  * @author
  */

#include <i2cmaster.h>

#include "LSM303DLH.h"

/* -------------------------------------------------------------------------- */

int8_t LSM303DLHInitACC(uint8_t acc_addr) {

  int8_t err = I2C_NO_ERROR;

  err = LSM303DLHWrite( acc_addr, CTRL_REG1_A, 0x27 ); // normal mode, ODR 50Hz
  if ( !err )
    err = LSM303DLHWrite( acc_addr, CTRL_REG4_A, 0x80 ); // +- 2 gauss, little endian

  return err;
}

/* -------------------------------------------------------------------------- */

int8_t LSM303DLHInitMAG(uint8_t mag_addr) {

  int8_t err = I2C_NO_ERROR;

  err = LSM303DLHWrite( mag_addr, CRA_REG_M, 0x14 ); // ODR := 30 Hz
  if ( !err )
    LSM303DLHWrite( mag_addr, MR_REG_M, 0x00 ); // awake from sleep mode

  return err;
}

/* -------------------------------------------------------------------------- */

int8_t LSM303DLHReadACC(uint8_t acc_addr,LSM303DLHData* data) {

  if ( i2c_start( acc_addr | I2C_WRITE ) ) {
    /* failed to issue start condition, possibly no device found */
    i2c_stop();
    return I2C_ERR_NO_DEVICE;
  }

  /* issuing start condition ok, device accessible */
  i2c_write(0xa8);// OUT_X_L_A, MSB set to enable auto-increment

  if ( i2c_rep_start( acc_addr | I2C_READ ) ) {
    i2c_stop();
    return I2C_ERROR;
  }

  data->fSensorX = i2c_readAck();
  data->fSensorX |= (i2c_readAck() << 8);
  data->fSensorY = i2c_readAck();
  data->fSensorY |= (i2c_readAck() << 8);
  data->fSensorZ = i2c_readAck();
  data->fSensorZ |= (i2c_readNak() << 8);

  i2c_stop();

  return I2C_NO_ERROR;
}

/* -------------------------------------------------------------------------- */

int8_t LSM303DLHReadMAG(uint8_t mag_addr,LSM303DLHData* data) {

  if ( i2c_start( mag_addr | I2C_WRITE ) ) {
    /* failed to issue start condition, possibly no device found */
    i2c_stop();
    return I2C_ERR_NO_DEVICE;
  }

  /* issuing start condition ok, device accessible */
  i2c_write(0x03); // OUT_X_H_M

  if ( i2c_rep_start( mag_addr | I2C_READ ) ) {
    i2c_stop();
    return I2C_ERROR;
  }

  data->fSensorX = i2c_readAck() << 8;  // OUT_X_H_M
  data->fSensorX |= i2c_readAck();      // OUT_X_L_M
  data->fSensorY = i2c_readAck() << 8;  // OUT_Y_H_M
  data->fSensorY |= i2c_readAck();      // OUT_Y_L_M
  data->fSensorZ = i2c_readAck() << 8;  // OUT_Z_H_M
  data->fSensorZ |= i2c_readNak();      // OUT_Z_L_M

  i2c_stop();

  return I2C_NO_ERROR;
}

/* -------------------------------------------------------------------------- */

int8_t LSM303DLHWrite(uint8_t addr,uint8_t reg,uint8_t data) {

  if ( i2c_start( addr | I2C_WRITE ) ) {
    /* failed to issue start condition, possibly no device found */
    i2c_stop();
    return I2C_ERR_NO_DEVICE;
  }
  else {
    /* issuing start condition ok, device accessible */
    i2c_write(reg);
    i2c_write(data);
    i2c_stop();
  }

  return I2C_NO_ERROR;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
