
/*
 * File   : compass.c
 *
 * $Id: compass.c,v 1.1 2012/05/21 05:30:15 mathes Exp $
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        $Revision: 1.1 $ $Date: 2012/05/21 05:30:15 $
 * Description:    Contains all functions which deal with the messages from the
 *                 ACC/MAG sensors and their interpretation.
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

/** @file compass.c
  * Contains all functions which deal with the messages from the
  *          ACC/MAG sensors and their interpretation.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include "global.h"

/* local data types and variables */

typedef enum {

  kSENTENCE_TYPE_UNKNOWN = 0,
  kSENTENCE_TYPE_ACOK,
  kSENTENCE_TYPE_ACERR,
  kSENTENCE_TYPE_ACRAW,
  
} ESentenceType;

static uint8_t gSentenceType = kSENTENCE_TYPE_UNKNOWN;

/* raw data string of individual quantities */

//static char gACC_x_raw[7];
//static char gACC_y_raw[7];
//static char gACC_z_raw[7];

static char gMAG_x_raw[5];
static char gMAG_y_raw[5];
static char gMAG_z_raw[5];

/* local prototypes */

static void CompassMessageConvert(i_vector_t* mag);
static uint8_t CompassMessageDecode(uint8_t newchar);

// --------------------------------------------------------------------------

void CommpassMessageReceive(unsigned int uart_data) {

  static uint8_t msg_complete = FALSE;
  
  if ( (uart_data >> 8) == 0) {

    msg_complete = CompassMessageDecode( uart_data & 0xff );

    if ( msg_complete ) {
    }
  }
  else { // UART receive error
  }
}

// --------------------------------------------------------------------------

// inspired by G.Dion's (WhereAVR) MsgHandler() function
//
static uint8_t CompassMessageDecode(uint8_t newchar) {

  static uint8_t commas;			// Number of commas for far in sentence
  static uint8_t index;				// Individual array index

  if ( newchar == 0 ) {				// A NULL character resets decoding
  
    commas = 25;			    	// Set to an outrageous value
    gSentenceType = kSENTENCE_TYPE_UNKNOWN;	// Clear local parse variable
    return FALSE;
  }

  if ( newchar == '$' ) {			// Start of Sentence character, reset
  
    commas = 0; 			    	// No commas detected in sentence for far
    gSentenceType = kSENTENCE_TYPE_UNKNOWN;	// Clear local parse variable
    return FALSE;
  }

  if ( newchar == ',' ) {			// If there is a comma
  
    commas += 1;			    	// Increment the comma count
    index = 0;  			    	// And reset the field index
    return FALSE;
  }

  //if ( newchar == '*' ) {			// If there is a '*' -> checksum
  //  return TRUE;
  //}
 
  if ( newchar == '\n' ) {			// If there is a linefeed character
    gSentenceType = kSENTENCE_TYPE_UNKNOWN;	// Clear local parse variable
    return TRUE;
  }

  if ( commas == 0 ) {
  
    switch ( newchar ) {		       	// "AC" are simply skipped here
      
      case 'E': gSentenceType = kSENTENCE_TYPE_ACERR;
  		break;
      
      case 'O': gSentenceType = kSENTENCE_TYPE_ACOK;
  		break;
      
      case 'R': gSentenceType = kSENTENCE_TYPE_ACRAW;
  		break;
      
      default: break;
    }

    return FALSE;
  }

  if ( gSentenceType == kSENTENCE_TYPE_ACOK ) {         // $ACOK sentence decode initiated
  
  }

  if ( gSentenceType == kSENTENCE_TYPE_ACERR ) {        // $ACERR sentence decode initiated
   
  }

  if ( gSentenceType == kSENTENCE_TYPE_ACRAW ) {	// $ACRAW sentence decode initiated
   
    // example: "$ACRAW,768,-704,-16208,-278,-342,337*E4"
     
    switch ( commas ) {
      //case 1: gACC_x_raw[index++] = newchar; return FALSE;
      //case 2: gACC_y_raw[index++] = newchar; return FALSE;
      //case 3: gACC_z_raw[index++] = newchar; return FALSE;
      case 4: gMAG_x_raw[index++] = newchar; return FALSE;
      case 5: gMAG_y_raw[index++] = newchar; return FALSE;
      case 6: gMAG_z_raw[index++] = newchar; return FALSE;
    }
    
    return FALSE;
  }

  return FALSE;

}  // end of CompassMessageDecode()

// --------------------------------------------------------------------------

void CompassMessageInit(void) {

  CompassMessageDecode( 0 );

//  for (uint8_t i=0; i<sizeof(gACC_x_raw); ++i) {
//    gACC_x_raw[i] = 0;
//    gACC_y_raw[i] = 0;
//    gACC_z_raw[i] = 0;
//  }

  for (uint8_t i=0; i<sizeof(gMAG_x_raw); ++i) {
    gMAG_x_raw[i] = 0;
    gMAG_y_raw[i] = 0;
    gMAG_z_raw[i] = 0;
  }
}

// --------------------------------------------------------------------------

static void CompassMessageConvert(i_vector_t* mag) {

  if ( !mag ) return;

  mag->fX = atoi( gMAG_x_raw );
  mag->fY = atoi( gMAG_y_raw );
  mag->fZ = atoi( gMAG_z_raw );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
