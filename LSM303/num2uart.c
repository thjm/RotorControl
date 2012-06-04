

/*
 * File   : num2uart.c
 *
 * Purpose: Simple and fast printing of numbers to UART
 *
 * Authors: - Peter Danneger
 *          - Dirk ?
 *
 * References: http://www.mikrocontroller.net/topic/12165#new
 *
 * $Id: num2uart.c,v 1.3 2012/06/04 13:31:08 mathes Exp $
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include <uart.h>

#define uchar unsigned char
#define uint unsigned int

static const unsigned char HEX[] PROGMEM = {"0123456789ABCDEF"};
static const unsigned int TEST[] PROGMEM = { 10, 100, 1000, 10000 };

// --------------------------------------------------------------------------

void hex2uart(unsigned char val)
 {
  uchar bl = pgm_read_byte(&HEX[val & 0x0F]);
  uchar bh = pgm_read_byte(&HEX[val >> 4]);
  uart_putc( bh );
  uart_putc( bl );
}

// --------------------------------------------------------------------------

void int2uart(int val)
 {
  uchar d, i;
  uchar zero;
  uint uval = val;

  if( val < 0 ){
    uval = -val;
    uart_putc( '-' );
  }

  zero = 1;
  i = 4;
  do{
    i--;
    for ( d = '0'; uval >= pgm_read_word(&TEST[i]); 
                   uval -= pgm_read_word(&TEST[i]) ) {
      d++;
      zero = 0;
    }
    if( zero == 0 )
      uart_putc( d );
  } while( i );

  uart_putc( (uchar)uval + '0' );
}

// --------------------------------------------------------------------------

#ifdef USE_FLOAT

#define DIGITS 8

void float2uart(float val)
 {
  signed char ep, dp, i, num;
  
  if( val < 0 ){
    val = -val;
    uart_putc( '-' );
  }
  ep = 0;
  if( val != 0 ){
    if( val < 1 ){
      val *= 1000;
      ep = -3;
    }

    if( val >= 1000 ){
      val *= 0.001;
      ep = 3;
    }
  }

  for( dp = 1; val >= 10; val *= 0.1 )		// normalize (x.xxx)
    dp ++;

  for( i = DIGITS;; ){				// n digits
    uart_putc( (num = val) + '0' );		// get digit
    if( --i == 0 )
      break;
    if( --dp == 0 )				// set decimal point
      uart_putc( '.' );
    val = (val - num) * 10;			// next digit
  }
  if( ep ){
    uart_putc( 'e' );
    int2uart( ep );
  }
}

#endif // USE_FLOAT

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
