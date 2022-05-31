
/*
 * File   : num2uart.h
 *
 * Purpose: Header file for num2uart.c
 *
 */


//
// Some useful conversion functions, don't need printf() for debugging
// See also: http://www.mikrocontroller.net/topic/12165#new
//

#ifndef _num2uart_h_
#define _num2uart_h_


#ifdef __cplusplus
extern "C" {
#endif

/** Convert a single byte into its hexadecimal representation and
  * output it to the UART.
  */
extern void hex2uart(unsigned char val);

/** Output an integer to the UART. */
extern void int2uart(int val);

/** Output a float to the UART. */
extern void float2uart(float val);

#ifdef __cplusplus
}
#endif

#endif  // _num2uart_h_
