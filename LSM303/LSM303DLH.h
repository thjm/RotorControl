
/*
 * File   : LSM303DLH.h
 *
 * Purpose: Header file for LSM303DLH.c
 *
 * $Id: LSM303DLH.h,v 1.2 2011/10/23 16:04:31 mathes Exp $
 *
 */


#ifndef _LSM303DLH_h_
#define _LSM303DLH_h_

#include <stdint.h>

/** Initialize accelerometer sensor of LSM303DLH at I2C address acc_addr. */
extern uint8_t LSM303DLHInitACC(uint8_t acc_addr);

/** Initialize magnetometer sensor of LSM303DLH at I2C address mag_addr. */
extern uint8_t LSM303DLHInitMAG(uint8_t mag_addr);

#endif /* _LSM303DLH_h_ */
