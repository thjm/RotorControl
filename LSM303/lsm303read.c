
/*
 * File   : lsm303read.c
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        v1r0
 * Description:    Program to readout the LSM303DLH sensor and send its 
 *                 data via UART. 
 *
  
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version. 
        
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Falls nicht, schreiben Sie an die Free Software Foundation, 
   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*
 */
 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/** @file lsm303read.c
  * Program to readout the LSM303DLH sensor and send its data via UART.
  * The data is either sent in plain ASCII format or in NMEA formatted 
  * strings (favored mode of operation).
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <i2cmaster.h>

#define UART_BAUD_RATE 9600
#define UART_TX_BUFFER_SIZE 32 
#define UART_RX_BUFFER_SIZE 32

#include <uart.h>
#include <num2uart.h>

#include "global.h"
#include "LSM303DLH.h"

/** Flag which signals to main() that 0.1 sec are over and a new sensor
  * reading is required.
  *
  * At program start, we do this immediately...
  */
volatile uint8_t gSensorReadout = 1;

// --------------------------------------------------------------------------

//
// avrdude -p atmega8 -P /dev/parport1 -c stk200 -y -U flash:w:lsm303read.hex
//
// avrdude -p atmega8 -P usb -c usbasp -y -U flash:w:lsm303read.hex
//
// factory fuse settings:
//  Device signature = 0x1e9307
//  lfuse = 0xE1
//  hfuse = 0xD9
//
// avrdude -p atmega8 -P usb -c usbasp -y -U lfuse:r:-:i -U hfuse:r:-:i 
// avrdude -p atmega8 -P usb -c usbasp -y -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m
//  - 2 MHz internal clock:
//    -> lfuse = 0xE2
//
// settings for the bootloader ():
//  BOOTRST = 0 (programmed)
//    -> hfuse = 0xD8
//  BOOTSZ0 = 0
//  BOOTSZ1 = 0 -> 1K words boot block size (default)
//  -> application flash size = 6 KBytes, BOOTLOADERSTARTADR = 0x1800
//     bootloader flash size = 2 KBytes, BOOTLDRSIZE = 0x0800
//  + 4 or 8 MHz internal clock:
//    -> lfuse = 0xE3 or 0xE4
//  LB1 = 1, LB2 = 1
//

static const char cBlank[] PROGMEM = " ";
static const char cCRLF[] PROGMEM = "\r\n";

#ifdef NMEA_FORMAT
static char *strcat_p(char *dest,const char *progmem_src) {

  register char c;
  char *dest2 = &dest[strlen(dest)];
  
  while ( (c = pgm_read_byte(progmem_src++)) )
    *dest2++ = c;
  
  *dest2 = 0;
  
  return dest;
}

// --------------------------------------------------------------------------

static const char * int2string(int16_t data) {

  static char buffer[8];
  
  itoa( data, buffer, 10 );
  
  return buffer;
}

// --------------------------------------------------------------------------

const unsigned char HEX[] PROGMEM = {"0123456789ABCDEF"};

static const char * hex2string(uint8_t val) {

  static char buffer[3];
  
  buffer[0] = pgm_read_byte(&HEX[val & 0x0F]);
  buffer[1] = pgm_read_byte(&HEX[val >> 4]);
  buffer[2] = 0;
  
  return buffer;
}

// --------------------------------------------------------------------------

//
// format of NMEA 0183 message string for LSM303DLH sensor raw data:
//
//  $ACRAW,acx,acy,acz,mx,my,mz*CHECKSUM
//

static const char cACRAW[] PROGMEM = "$ACRAW";
//static const char cAsterisk[] PROGMEM = "*";
static const char cComma[] PROGMEM = ",";

static void UartSendLSM303DataNMEA(LSM303DLHData* acc_data,
                                   LSM303DLHData* mag_data) {

  char message[60] = { 0 };
  
  strcat_p( message, cACRAW );
  strcat_p( message, cComma );
  strcat( message, int2string( acc_data->fSensorX ) );
  strcat_p( message, cComma );
  strcat( message, int2string( acc_data->fSensorY ) );
  strcat_p( message, cComma );
  strcat( message, int2string( acc_data->fSensorZ ) );
  strcat_p( message, cComma );
  strcat( message, int2string( mag_data->fSensorX ) );
  strcat_p( message, cComma );
  strcat( message, int2string( mag_data->fSensorY ) );
  strcat_p( message, cComma );
  strcat( message, int2string( mag_data->fSensorZ ) );
  
  strcat( message, "*" );
  
  uint8_t checksum = 0;
  
  for ( int8_t i=1; message[i] != '*'; ++i )
    checksum ^= message[i];
    
  strcat( message, hex2string( checksum ) );
  
  uart_puts( message );
  uart_puts_p( cCRLF );
}
#endif // NMEA_FORMAT

// --------------------------------------------------------------------------

#ifndef NMEA_FORMAT
static void UartSendLSM303Data(LSM303DLHData* data) {

  if ( !data ) return;
  
  int2uart( data->fSensorX );
  uart_puts_p( cBlank );
  
  int2uart( data->fSensorY );
  uart_puts_p( cBlank );
  
  int2uart( data->fSensorZ );
  uart_puts_p( cBlank );

  uart_puts_p( cCRLF );
}
#endif // NMEA_FORMAT

// --------------------------------------------------------------------------

static int8_t LSM303DLHInit(void) {

  int8_t err = LSM303DLHInitACC( I2C_DEV_LSM303DLH_ACC1 );
  
  if ( !err ) err = LSM303DLHInitMAG( I2C_DEV_LSM303DLH_MAG );
  
  return err;
}

// --------------------------------------------------------------------------

static uint8_t gSensorReadoutCounter = SENSOR_READOUT_PERIOD;

// ISR for timer/counter 0 overflow: called every 100 ms
// - load counter with initial constant
// - set flag for the next sensor readout

// In case of 4 or 8 MHz internal clock this ISR is called every 10 ms.
// -> to achieve 100 ms, we have an additional loop then
#if (F_CPU > 2000000UL)
static uint8_t g10msCounter = 10;
#endif // F_CPU

ISR(TIMER0_OVF_vect) {

  TCNT0 = CNT0_PRESET;
  
#if (F_CPU > 2000000UL)
  if ( --g10msCounter != 0 ) return;
  
  g10msCounter = 10;
#endif // F_CPU
  
  if ( --gSensorReadoutCounter == 0 ) {
    gSensorReadout = 1;
    gSensorReadoutCounter = SENSOR_READOUT_PERIOD;
  }
}

// --------------------------------------------------------------------------

#ifdef NMEA_FORMAT
static const char cACERR[] PROGMEM = "$ACERR*00\r\n";
static const char cACOK[] PROGMEM = "$ACOK*00\r\n";
#else
static const char cACERR[] PROGMEM = "ERROR\r\n";
static const char cACOK[] PROGMEM = "\r\nREADY\r\n";
#endif // 

int main(void) {

  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
  
  // init I2C interface
  i2c_init();
  
  // init port(s) for RS485
  RS485EnableTx();
  RS485_DDR |= RS485_TX_ENABLE;

  // timer 0 initialisation
  TCNT0 = CNT0_PRESET;
  TCCR0 = (1<<CS02)|(1<<CS00);  // CK/1024 -> 1 tick each 1.024 msec

  // enable timer overflow interrupt
  TIMSK |= (1<<TOIE0);

  sei();
  
  uart_puts_p(cACOK);

  int8_t err = LSM303DLHInit();
  
  if ( err ) {
  
    uart_puts_p(cACERR);
  }

  LSM303DLHData acc_data, mag_data;
  
  while ( 1 ) {

#if 0
    // if nothing more to send, switch off transmitter to receive commands
    
    if ( uart.c::UART_TxHead == uart.c::UART_TxTail ) {
      RS485DisableTx();
    }
#endif

    if ( !gSensorReadout ) continue;
    
    RS485EnableTx();

    err = LSM303DLHReadACC( I2C_DEV_LSM303DLH_ACC1, &acc_data );
    
    if ( !err ) LSM303DLHReadMAG( I2C_DEV_LSM303DLH_MAG, &mag_data );
    
    if ( err ) {

      uart_puts_p(cACERR);
      
      LSM303DLHInit();   // try to init the sensor again
    }
    else {  // send ACC & MAG data via UART
#ifdef NMEA_FORMAT
      UartSendLSM303DataNMEA( &acc_data, &mag_data );
#else
      uart_puts_P("ADATA ");
      UartSendLSM303Data( &acc_data );
      uart_puts_P("MDATA ");
      UartSendLSM303Data( &mag_data );
#endif // NMEA_FORMAT
    }
    
    gSensorReadout = 0;
  }
  
  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
    
