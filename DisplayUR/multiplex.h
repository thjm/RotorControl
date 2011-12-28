/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:        
 known Problems: none
 Version:        11.01.2009
 Description:    Power Supply LED Display

 Dieses Programm ist freie Software. Sie können es unter den Bedingungen der 
 GNU General Public License, wie von der Free Software Foundation veröffentlicht, 
 weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz oder 
 (nach Ihrer Option) jeder späteren Version. 

 Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, 
 daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, 
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT 
 FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License. 

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem 
 Programm erhalten haben. 
 Falls nicht, schreiben Sie an die Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 
----------------------------------------------------------------------------*/

#ifndef _MULTIPLEX_H
#define _MULTIPLEX_H

#include <stdint.h>
#include <string.h>

/** Number of segments to be displayed. */
enum { kNSegments = 6 };

enum {
  kDisplayOff = 0x00,
  kDisplayOn = 0x01,
};

extern volatile unsigned char gMultiplexMode;
extern volatile uint8_t gSegmentData[];

/** This function initializes and starts the Timer2 which is used for
  * 7-segment multiplexing. */
extern void MultiplexInit(void);

/**  */
static inline void MultiplexOn(void)
 { gMultiplexMode |= kDisplayOn; }

/**  */
static inline void MultiplexOff(void)
 { gMultiplexMode &= ~kDisplayOn; }

/** Set the value for the leftmost 3 digits. */
extern void MultiplexSetL(uint16_t l_data);

/** Set the value for the rightmost 3 digits. */
extern void MultiplexSetR(uint16_t r_data);

/** Set the values for the display */
static inline void MultiplexSet(uint16_t l_data,uint16_t r_data)
 { MultiplexSetL( l_data ); MultiplexSetR( r_data ); }

/** Write raw values to the leftmost 3 digits. */
static inline void MultiplexSetLRaw(const uint8_t *l_raw)
 { memcpy((void *)&gSegmentData[0], l_raw, 3 ); } 

/** Write raw values to the rightmost 3 digits. */
static inline void MultiplexSetRRaw(uint8_t *r_raw)
 { memcpy((void *)&gSegmentData[3], r_raw, 3 ); } 

/** Write raw values to the display. */

#endif //_MULTIPLEX_H
