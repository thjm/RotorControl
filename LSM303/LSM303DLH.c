
/*
 * File   : LSM303DLH.c
 *
 * Purpose: Implementation file for LSM303DLH specific routines.
 *
 * $Id: LSM303DLH.c,v 1.3 2011/10/24 20:33:00 mathes Exp $
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

int8_t LSM303DLHInitACC(uint8_t acc_addr)
 {
  int8_t err = I2C_NO_ERROR;
  
  err = LSM303DLHWrite( acc_addr, CTRL_REG1_A, 0x27 ); // normal mode, ODR 50Hz
  if ( !err )
    err = LSM303DLHWrite( acc_addr, CTRL_REG4_A, 0x80 ); // +- 2 gauss, little endian
  
  return err;
}

/* -------------------------------------------------------------------------- */

int8_t LSM303DLHInitMAG(uint8_t mag_addr)
 {
  int8_t err = I2C_NO_ERROR;
  
  err = LSM303DLHWrite( mag_addr, CRA_REG_M, 0x14 ); // ODR := 30 Hz
  if ( !err )
    LSM303DLHWrite( mag_addr, MR_REG_M, 0x00 ); // awake from sleep mode
  
  return err;
}

/* -------------------------------------------------------------------------- */

int8_t LSM303DLHReadACC(uint8_t acc_addr,LSM303DLHData* data)
 {
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

int8_t LSM303DLHReadMAG(uint8_t mag_addr,LSM303DLHData* data)
 {
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

int8_t LSM303DLHWrite(uint8_t addr,uint8_t reg,uint8_t data)
 {
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
