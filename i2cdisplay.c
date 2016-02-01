
/*
 * File   : i2cdisplay.c
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        Version v1r0
 * Description:    Functions for the I2C driven display unit (UR).
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

/** @file i2cdisplay.c
  * Functions for the I2C driven display unit (UR).
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <i2cmaster.h>   // P.Fleury's lib

#include "i2cdisplay.h"

// --------------------------------------------------------------------------

void I2CDisplayInit(void) {

}

// --------------------------------------------------------------------------

void I2CDisplayBlank(void) {

  uint8_t ret = i2c_start( I2C_DISPLAY | I2C_WRITE);
  
  if ( ret ) {
    i2c_stop();
    return;
  }
  
  i2c_write(0x00); 			// write remote buffer address
  i2c_write(I2C_DISP_RAWDATA);  	// write command
  for (uint8_t i=0; i<6; ++i)
    i2c_write( 0x00 );
  i2c_stop();
}

// --------------------------------------------------------------------------

void I2CDisplayOn(void) {

  uint8_t ret = i2c_start( I2C_DISPLAY | I2C_WRITE);
  
  if ( ret ) {
    i2c_stop();
    return;
  }
  
  i2c_write(0x00); 			// write remote buffer address
  i2c_write(I2C_DISP_ON);  		// write command
  i2c_stop();
}

// --------------------------------------------------------------------------

void I2CDisplayOff(void) {

  uint8_t ret = i2c_start( I2C_DISPLAY | I2C_WRITE);
  
  if ( ret ) {
    i2c_stop();
    return;
  }
  
  i2c_write(0x00); 			// write remote buffer address
  i2c_write(I2C_DISP_OFF);  		// write command
  i2c_stop();
}

// --------------------------------------------------------------------------

uint8_t I2CDisplayWrite_p(uint8_t l_msg,const uint8_t *p_msg) {

  uint8_t ret = i2c_start( I2C_DISPLAY | I2C_WRITE);
  
  if ( ret ) {
    i2c_stop();
    return 1;
  }
  
  i2c_write(0x00); 			// write remote buffer address
  i2c_write(I2C_DISP_RAWDATA);  	// write command
  
  for (uint8_t i=0; i<l_msg; ++i) {
    i2c_write( pgm_read_byte( &p_msg[i] ) );
  }
  
  i2c_stop();
  
  return 0;
}

// --------------------------------------------------------------------------

uint8_t I2CDisplayWrite(uint8_t l_msg,const uint8_t *p_msg) {

  uint8_t ret = i2c_start( I2C_DISPLAY | I2C_WRITE);
  
  if ( ret ) {
    i2c_stop();
    return 1;
  }
  
  i2c_write(0x00); 			// write remote buffer address
  i2c_write(I2C_DISP_RAWDATA);  	// write command
  
  for (uint8_t i=0; i<l_msg; ++i) {
    i2c_write( p_msg[i] );
  }
  
  i2c_stop();
  
  return 0;
}

// --------------------------------------------------------------------------

uint8_t I2CDisplayWriteL(uint8_t l_msg,const uint8_t *p_msg) {

  uint8_t ret = i2c_start( I2C_DISPLAY | I2C_WRITE);
  
  if ( ret ) {
    i2c_stop();
    return 1;
  }
  
  i2c_write(0x00); 			// write remote buffer address
  i2c_write(I2C_DISP_RAWDATA_LEFT);  	// write command
  
  for (uint8_t i=0; i<l_msg; ++i) {
    i2c_write( p_msg[i] );
  }
  
  i2c_stop();
  
  return 0;
}

// --------------------------------------------------------------------------

uint8_t I2CDisplayWriteR(uint8_t l_msg,const uint8_t *p_msg) {

  uint8_t ret = i2c_start( I2C_DISPLAY | I2C_WRITE);
  
  if ( ret ) {
    i2c_stop();
    return 1;
  }
  
  i2c_write(0x00); 			// write remote buffer address
  i2c_write(I2C_DISP_RAWDATA_RIGHT);  	// write command
  
  for (uint8_t i=0; i<l_msg; ++i) {
    i2c_write( p_msg[i] );
  }
  
  i2c_stop();
  
  return 0;
}

// --------------------------------------------------------------------------

uint8_t I2CDisplayWriteLData(int16_t data) {
  
  if ( i2c_start( I2C_DISPLAY | I2C_WRITE) ) {
    i2c_stop();
    return 1;
  }
  
  i2c_write(0x00); 			// write remote buffer address
  i2c_write(I2C_DISP_DATA_LEFT);  	// write command
  i2c_write(data & 0xff);		// write data bytes to remote buffer
  i2c_write((data & 0xff00) >> 8);
  i2c_stop();
  
  return 0;
}

// --------------------------------------------------------------------------

uint8_t I2CDisplayWriteRData(int16_t data) {
  
  if ( i2c_start( I2C_DISPLAY | I2C_WRITE) ) {
    i2c_stop();
    return 1;
  }
  
  i2c_write(0x00); 			// write remote buffer address
  i2c_write(I2C_DISP_DATA_RIGHT);  	// write command
  i2c_write(data & 0xff);		// write data bytes to remote buffer
  i2c_write((data & 0xff00) >> 8);
  i2c_stop();
  
  return 0;
}

// --------------------------------------------------------------------------

uint8_t I2CDisplayWriteData(int16_t l_data,int16_t r_data) {

  uint8_t ret;
  
  if ( (ret = I2CDisplayWriteLData( l_data )) ) {
    i2c_init();
  }
  
  _delay_ms(1.0);
  
  ret = I2CDisplayWriteRData( r_data );
  
  return ret;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
