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

/** Number of segments to be displayed. */
enum { kNSegments = 6 };

enum {
  kDisplayOff = 0x00,
  kDisplayOn = 0x01,
  
  kDataMode = 0x00,      /// convert and display two integer values
  kDirectMode = 0x10,    /// display content of display buffer
};

//extern int gMultiplexDisplay1;
//extern int gMultiplexDisplay2;
extern volatile unsigned char gMultiplexMode;

/** This function initializes and starts the Timer2 which is used for
  * 7-segment multiplexing. */
extern void MultiplexInit(void);

/**  */
static inline void MultiplexOn(void)
 { gMultiplexMode |= kDisplayOn; }

/**  */
static inline void MultiplexOff(void)
 { gMultiplexMode &= ~kDisplayOn; }

#if 0
/**  */
static inline void MultiplexSet1(uint16_t val1)
 { gMultiplexDisplay1 = val1; }

/**  */
static inline void MultiplexSet2(uint16_t val2)
 { gMultiplexDisplay2 = val2; }
#endif

/**  */
extern void MultiplexSet(uint16_t data1,uint16_t data2);

#endif //_MULTIPLEX_H
