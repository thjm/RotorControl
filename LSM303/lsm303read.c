
/*
 * File   : lsm303read.c
 *
 * Purpose: Program to readout the LSM303DLH sensor and send its data via UART.
 *
 * $Id: lsm303read.c,v 1.1 2011/10/26 12:52:57 mathes Exp $
 *
 */
 
#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/** @file lsm303read.c
  * Program to readout the LSM303DLH sensor and send its data via UART.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include <i2cmaster.h>

#define UART_BAUD_RATE 9600
#define UART_TX_BUFFER_SIZE 32 
#define UART_RX_BUFFER_SIZE 32

#include <uart.h>
#include <num2uart.h>

#include "LSM303DLH.h"

// --------------------------------------------------------------------------

static const char cBlank[] PROGMEM = " ";
static const char cCRLF[] PROGMEM = "\r\n";

static void UartSendLSM303Data(LSM303DLHData* data)
 {
  if ( !data ) return;
  
  int2uart( data->fSensorX );
  uart_puts_p( cBlank );
  
  int2uart( data->fSensorY );
  uart_puts_p( cBlank );
  
  int2uart( data->fSensorZ );
  uart_puts_p( cBlank );

  uart_puts_p( cCRLF );
}

// --------------------------------------------------------------------------

//
// avrdude -p atmega8 -P /dev/parport1 -c stk200 -y -U flash:w:lsm303read.hex
//

int main(void)
 {
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
    
  i2c_init();                                // init I2C interface
  
  sei();
  
  uart_puts_P("\r\nREADY\r\n");

  int8_t err;
  
  err = LSM303DLHInitACC( I2C_DEV_LSM303DLH_ACC1 );
  
  if ( !err ) err = LSM303DLHInitMAG( I2C_DEV_LSM303DLH_MAG );
  
  if ( err ) uart_puts_P("ERROR\r\n");

  LSM303DLHData acc_data, mag_data;
  
  while ( 1 ) {

    err = LSM303DLHReadACC( I2C_DEV_LSM303DLH_ACC1, &acc_data );
    
    if ( !err ) LSM303DLHReadMAG( I2C_DEV_LSM303DLH_MAG, &mag_data );
    
    if ( err ) {
      uart_puts_P("ERROR\r\n");
    }
    // send ACC & MAG data via UART
    else {
      uart_puts_P("ADATA ");
      UartSendLSM303Data( &acc_data );
      uart_puts_P("MDATA ");
      UartSendLSM303Data( &mag_data );
    }
    
    // max. 10 measurements per second
    for ( int8_t i=0; i<10; ++i)
      _delay_ms( 10.0 );
  }
  
  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
    
