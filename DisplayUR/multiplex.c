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

#include <avr/interrupt.h>
#include <avr/io.h>
#include <compat/deprecated.h>


#include "multiplex.h"

const char SEGMENTE[] = {119, 18, 107, 59, 30, 61, 125, 19, 127, 31};

volatile unsigned char gSegmentCounter = 0;
volatile unsigned char gSegmentData[kNSegments] = { 0 };

volatile uint16_t gMultiplexDisplay1 = 0;
volatile uint16_t gMultiplexDisplay2 = 0;
volatile unsigned char gMultiplexMode;

// --------------------------------------------------------------------------

// Timer2 overflow interrupt
//
// port D drives the individual segments
// port B selects the digits, PORTB = 0x01 -> left-most digit
//
ISR(TIMER2_OVF_vect)
 {
  PORTD = 0;
  
  if ( (gMultiplexMode & kDisplayOn) != kDisplayOn ) return;
  
  PORTB = (1<<gSegmentCounter);

  PORTD = gSegmentData[gSegmentCounter];

  if ( (gSegmentCounter++) >= kNSegments ) gSegmentCounter = 0;   
}
	
// --------------------------------------------------------------------------

void MultiplexInit(void)
 {
  gMultiplexMode = kDataMode | kDisplayOn;
  gMultiplexDisplay1 = gMultiplexDisplay2 = 0;
  
  //enable interrupt for Timer2 overflow 
  TIMSK |= (1 << TOIE2);
  
  // set prescaler to 1024 
  TCCR2 |= (1<<CS22); 

  return;
}

// --------------------------------------------------------------------------

void MultiplexSet(uint16_t data1,uint16_t data2)
 {
  gMultiplexDisplay1 = data1;
  gMultiplexDisplay2 = data2;

  if ( (gMultiplexMode & kDataMode) == kDataMode ) {
    
    for ( uint8_t segment=0; segment<kNSegments; segment++) {
  
      switch (segment) {	  

        case 0:
  	    gSegmentData[segment] = SEGMENTE[(gMultiplexDisplay1 % 1000 / 100)];
  	    break;  
        case 1:
  	    gSegmentData[segment] = SEGMENTE[(gMultiplexDisplay1 % 100 / 10)];
  	    break;	    
        case 2:
  	    gSegmentData[segment] = SEGMENTE[(gMultiplexDisplay1 % 10)];
  	    break;
        case 3:
  	    gSegmentData[segment] = SEGMENTE[(gMultiplexDisplay2 % 1000 / 100)];
  	    break;
        case 4:
  	    gSegmentData[segment] = SEGMENTE[(gMultiplexDisplay2 % 100 / 10)];
  	    break;  
        case 5:
  	    gSegmentData[segment] = SEGMENTE[(gMultiplexDisplay2 % 10)];
  	    break;
      
      } // switch ( segment ) ...
      
    } // for ( segment=... )
  }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
