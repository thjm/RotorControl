
/*
 * File   : lsm303test.c
 *
 * $Id: lsm303test.c,v 1.12 2014/03/24 10:06:32 mathes Exp $
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        $Revision: 1.12 $ $Date: 2014/03/24 10:06:32 $
 * Description:    Simple test program for AVR TWI interface with LSM303DLH. 
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
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


/** @file lsm303test.c
  * Simple test program for AVR TWI interface with LSM303DLH.
  * This test program uses the I2Cmaster and the UART libraries of P.Fleury.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#define USE_UART
#define UART_BAUD_RATE 9600
#define UART_TX_BUFFER_SIZE 32 
#define UART_RX_BUFFER_SIZE 32

#include <i2cmaster.h>
#ifdef USE_UART
 #include <uart.h>
 #ifdef UART_DEBUG
  #include <num2uart.h>
 #endif // UART_DEBUG
#endif // UART

#include "global.h"

#include "LSM303DLH.h"

#define LSM303DLH_USE_ACC
#define LSM303DLH_USE_MAG

/** local prototypes */

static void delay_sec(uint8_t n_sec);

// --------------------------------------------------------------------------

//
// avrdude -p atmega8 -P /dev/parport1 -c stk200 -y -U flash:w:lsm303test.hex
//
// avrdude -p atmega8 -P usb -c usbasp -y -U flash:w:lsm303test.hex
//

static const char cBlank[] PROGMEM = " ";
static const char cCRLF[] PROGMEM = "\r\n";
static const char cEqualSign[] PROGMEM = "= ";

static void PrintSensorData(const char *what,int data) 
 {
#ifdef UART_DEBUG
  uart_puts( what ); 
  uart_puts_p( cEqualSign );
  int2uart( data );
  uart_puts_p( cBlank );
#endif // UART_DEBUG
}

// --------------------------------------------------------------------------

#ifdef LSM303DLH_USE_ACC
static void LSM303DLHTestACC(void)
 {
#ifdef UART_DEBUG
  uart_puts_P("\r\n");
#endif // UART_DEBUG

  LSM303DLHData acc_data;

  // --- read accelerometer values
  
  uint8_t ret = LSM303DLHReadACC( I2C_DEV_LSM303DLH_ACC1, &acc_data );

  if ( ret ) { 

    YellowLEDOff();
    RedLEDOn(); 			     // show error

#ifdef UART_DEBUG
    if ( ret == I2C_ERR_NO_DEVICE ) uart_puts_P("ACC: I2C_ERR_NO_DEV\r\n");
#endif // UART_DEBUG

#ifdef UART_DEBUG
    if ( ret == I2C_ERROR ) uart_puts_P("ACC: I2C_ERROR\r\n");
#endif // UART_DEBUG

    return;
  }

  PrintSensorData( "AX", acc_data.fSensorX );
  PrintSensorData( "AY", acc_data.fSensorY );
  PrintSensorData( "AZ", acc_data.fSensorZ );
  
  uart_puts_p( cCRLF );
  
  delay_sec(1);
}
#endif // LSM303DLH_USE_ACC

// --------------------------------------------------------------------------

#ifdef LSM303DLH_USE_MAG
static void LSM303DLHTestMAG(void)
 {
#ifdef UART_DEBUG
  uart_puts_P("\r\n");
#endif // UART_DEBUG

  LSM303DLHData mag_data;
  
  // --- read magnetometer values
  
  uint8_t ret = LSM303DLHReadMAG( I2C_DEV_LSM303DLH_MAG, &mag_data );
  
  if ( ret ) { 

    YellowLEDOff();
    RedLEDOn(); 			     // show error

#ifdef UART_DEBUG
    if ( ret == I2C_ERR_NO_DEVICE ) uart_puts_P("MAG: I2C_ERR_NO_DEV\r\n");
#endif // UART_DEBUG

#ifdef UART_DEBUG
    if ( ret == I2C_ERROR ) uart_puts_P("MAG: I2C_ERROR\r\n");
#endif // UART_DEBUG

    return;
  }

  PrintSensorData( "MX", mag_data.fSensorX );
  PrintSensorData( "MY", mag_data.fSensorY );
  PrintSensorData( "MZ", mag_data.fSensorZ );

  uart_puts_p( cCRLF );

  delay_sec(1);
}
#endif // LSM303DLH_USE_MAG

// --------------------------------------------------------------------------

int main(void)
 {
  InitLEDPort();
  
  YellowLEDOn();
  delay_sec(1);
  YellowLEDOff();
  delay_sec(1);
  YellowLEDOn();

  delay_sec(1);
    
  i2c_init();                                // init I2C interface
#ifdef UART_DEBUG
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
  
  sei();

  uart_puts_P("\r\n'lsm303test' ready!\r\n");
#endif // UART_DEBUG

#ifdef LSM303DLH_USE_ACC
  LSM303DLHInitACC( I2C_DEV_LSM303DLH_ACC1 );
#endif // LSM303DLH_USE_ACC

#ifdef LSM303DLH_USE_MAG
  LSM303DLHInitMAG( I2C_DEV_LSM303DLH_MAG );
#endif // LSM303DLH_USE_MAG

  while ( 1 ) {

#ifdef UART_DEBUG
    uart_puts_P("\r\nRunning test... ");
#endif // UART_DEBUG

#if (defined LSM303DLH_USE_ACC) || (defined LSM303DLH_USE_MAG)

# ifdef LSM303DLH_USE_ACC
    LSM303DLHTestACC();
# endif // LSM303DLH_USE_ACC
# ifdef LSM303DLH_USE_MAG
    LSM303DLHTestMAG();
# endif // LSM303DLH_USE_MAG

#else

# ifdef UART_DEBUG
    uart_puts_p( cCRLF );
# endif // UART_DEBUG

    delay_sec(2);
#endif // LSM303DLH_USE_ACC || LSM303DLH_USE_MAG
  }
      
  return 0;
}

// --------------------------------------------------------------------------

static void delay_sec(uint8_t n_sec)
 {
  while ( n_sec ) {
    // inline in util/delay.h
    for ( int m=0; m<100; m++ )
      _delay_ms( 10.0 );
    n_sec--;
  }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
