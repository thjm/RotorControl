
/*
 * File   : lsm303test.c
 *
 * Purpose: Simple test program for AVR TWI interface with LSM303DLH
 *
 * $Id: lsm303test.c,v 1.4 2011/10/24 20:33:00 mathes Exp $
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
#include <LSM303DLH.h>

#define LSM303DLH_USE_ACC
#define LSM303DLH_USE_MAG

#ifdef USE_UART
# include <uart.h>
#endif // USE_UART

/** local prototypes */

static void delay_sec(uint8_t n_sec);

/** LED stuff ... */

#define LED_PORT       PORTB
#define LED_PORT_DDR   DDRB

#define LED_YELLOW     (1<<PB2)
#define LED_GREEN      (1<<PB3)
#define LED_RED        (1<<PB4)

#define InitLEDPort()  { DDRB |= LED_RED | LED_YELLOW | LED_GREEN; }

#define RedLEDOn()     { PORTB |= LED_RED; }
#define RedLEDOff()    { PORTB &= ~LED_RED; }
#define YellowLEDOn()  { PORTB |= LED_YELLOW; }
#define YellowLEDOff() { PORTB &= ~LED_YELLOW; }
#define GreenLEDOn()   { PORTB |= LED_GREEN; }
#define GreenLEDOff()  { PORTB &= ~LED_GREEN; }

static char buffer[10];

// --------------------------------------------------------------------------

static void PrintA(uint8_t al, uint8_t ah, char *what) 
 {
#ifdef USE_UART
#if 1
  int16_t a = al | (ah << 8);
  uart_puts( what );
  uart_puts_P("= ");
  itoa(a, buffer, 10);
  uart_puts( buffer );
#else
  itoa(al, buffer, 10);
  uart_puts( what );
  uart_puts_P("= ");
  uart_puts( buffer );
  uart_puts_P(" ");
  itoa(ah, buffer, 10);
  uart_puts( buffer );
#endif
  uart_puts_P("\r\n");
#endif // USE_UART
}

// --------------------------------------------------------------------------

#ifdef LSM303DLH_USE_ACC
static void LSM303DLHTestACC(void)
 {
  uint8_t ret;
  
#ifdef USE_UART
  uart_puts_P("\r\n");
#endif // USE_UART

  // --- read accelerometer values

  //i2c_start();
  //i2c_write_byte(0x30); // write acc
  ret = i2c_start( I2C_DEV_LSM303DLH_ACC1 | I2C_WRITE );
  if ( ret ) {
    /* failed to issue start condition, possibly no device found */
    i2c_stop();
    YellowLEDOff();
    RedLEDOn(); 			     // show error */
#ifdef USE_UART
    uart_puts_P("Error 1!\r\n");
#endif // USE_UART
  }
  else {
    /* issuing start condition ok, device accessible */
    //i2c_write_byte(0xa8); // OUT_X_L_A, MSB set to enable auto-increment
    i2c_write(0xa8);
  
    //i2c_start();  	    // repeated start
    //i2c_write_byte(0x31); // read acc
    ret = i2c_rep_start( I2C_DEV_LSM303DLH_ACC1 | I2C_READ );
    if ( ret ) {
      i2c_stop();
      YellowLEDOff();
      RedLEDOn(); 			     // show error */
#ifdef USE_UART
      uart_puts_P("Error 2!\r\n");
#endif // USE_UART
    }
    else {
      unsigned char axl = i2c_readAck(); // i2c_read(1);
      unsigned char axh = i2c_readAck();
      unsigned char ayl = i2c_readAck();
      unsigned char ayh = i2c_readAck();
      unsigned char azl = i2c_readAck();
      unsigned char azh = i2c_readNak(); // i2c_read(0);
      i2c_stop();

      PrintA( axl, axh, "AX" );
      PrintA( ayl, ayh, "AY" );
      PrintA( azl, azh, "AZ" );
    }
  }

  delay_sec(1);
}
#endif // LSM303DLH_USE_ACC

// --------------------------------------------------------------------------

#ifdef LSM303DLH_USE_MAG
static void LSM303DLHTestMAG(void)
 {
#ifdef USE_UART
  uart_puts_P("\r\n");
#endif // USE_UART

  LSM303DLHData mag_data;
  
  // --- read magnetometer values
  
  uint8_t ret = LSM303DLHReadMAG( I2C_DEV_LSM303DLH_MAG, &mag_data );
  
  if ( ret ) { 

    YellowLEDOff();
    RedLEDOn(); 			     // show error

#ifdef USE_UART
    if ( ret == I2C_ERR_NO_DEVICE ) uart_puts_P("MAG: I2C_ERR_NO_DEV\r\n");
#endif // USE_UART

#ifdef USE_UART
    if ( ret == I2C_ERROR ) uart_puts_P("MAG: I2C_ERROR\r\n");
#endif // USE_UART

    return;
  }

  
  PrintA( mag_data.fSensorX & 0xff, 
          (mag_data.fSensorX & 0xff00) >> 8, "MX" );
  PrintA( mag_data.fSensorY & 0xff, 
          (mag_data.fSensorY & 0xff00) >> 8, "MY" );
  PrintA( mag_data.fSensorZ & 0xff, 
          (mag_data.fSensorZ & 0xff00) >> 8, "MZ" );

  delay_sec(1);
}
#endif // LSM303DLH_USE_MAG

// --------------------------------------------------------------------------

//
// avrdude -p atmega8 -P /dev/parport1 -c stk200 -y -U flash:w:lsm303test.hex
//

int main(void)
 {
  InitLEDPort();
  YellowLEDOn();

  delay_sec(2);
    
  i2c_init();                                // init I2C interface
#ifdef USE_UART
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
  
  sei();

  uart_puts_P("\r\n'lsm303test' ready!\r\n");
#endif // USE_UART

#ifdef LSM303DLH_USE_ACC
  LSM303DLHInitACC( I2C_DEV_LSM303DLH_ACC1 );
#endif // LSM303DLH_USE_ACC

#ifdef LSM303DLH_USE_MAG
  LSM303DLHInitMAG( I2C_DEV_LSM303DLH_MAG );
#endif // LSM303DLH_USE_MAG

  while ( 1 ) {
#ifdef USE_UART
    uart_puts_P("Running test... ");
#endif // USE_UART
#if (defined LSM303DLH_USE_ACC) || (defined LSM303DLH_USE_MAG)
# ifdef LSM303DLH_USE_ACC
    LSM303DLHTestACC();
# endif // LSM303DLH_USE_ACC
# ifdef LSM303DLH_USE_MAG
    LSM303DLHTestMAG();
# endif // LSM303DLH_USE_MAG
#else
    uart_puts_P("\r\n");
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
