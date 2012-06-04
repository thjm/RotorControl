
/*
 * File   : compass.c
 *
 * $Id: compass.c,v 1.5 2012/06/04 20:34:57 mathes Exp $
 *
 * Copyright:      Hermann-Josef Mathes  mailto: dc2ip@darc.de
 * Author:         Hermann-Josef Mathes
 * Remarks:
 * Known problems: development status
 * Version:        $Revision: 1.5 $ $Date: 2012/06/04 20:34:57 $
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
#include <math.h>          // round(), atan2()
#include <avr/pgmspace.h>


/** @file compass.c
  * Contains all functions which deal with the messages from the
  *          ACC/MAG sensors and their interpretation.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#include "global.h"

#include <uart.h>

#include "vector.h"    // both are in ./LSM303 directory
#include "num2uart.h"

/* local data types and variables */

/** Min/max readings for MAG sensor at **THIS** location. */
static vector_t gMin_MAG = { -480, -196, -196 };
static vector_t gMax_MAG = {   40,  284,  284 };

/* local prototypes */

static void CompassMessageConvert(i_vector_t*acc,i_vector_t* mag);
static uint8_t CompassMessageDecode(uint8_t newchar);

static int GetAverageHeading(int cur_heading);

// Returns a heading (in degrees) given an acceleration vector a due to gravity, a magnetic vector m, and a facing vector p.
static int GetHeading3D(const vector_t *a,const vector_t *m,const vector_t *p);

// --------------------------------------------------------------------------

static vector_t gACC, gMAG;

void CommpassMessageReceive(unsigned int uart_data) {

  static uint8_t msg_complete = FALSE;
  static vector_t p = { 0.0, -1.0, 0.0 }; // X: to the right, Y: backward, Z: down
  
  if ( (uart_data >> 8) == 0) {

#ifdef ECHO_RS485
    uart_putc( uart_data & 0xff );
#endif // ECHO_RS485

    msg_complete = CompassMessageDecode( uart_data & 0xff );

    if ( msg_complete ) {

      // parse messages from individual buffers
      i_vector_t acc, mag;
      
      CompassMessageConvert( &acc, &mag );
      
      gACC.x  = acc.x;
      gACC.y  = acc.y;
      gACC.z  = acc.z;
      
      gMAG.x  = mag.x;
      gMAG.y  = mag.y;
      gMAG.z  = mag.z;
      
      // shift and scale
      gMAG.x = (gMAG.x - gMin_MAG.x) / (gMax_MAG.x - gMin_MAG.x) * 2.0 - 1.0;
      gMAG.y = (gMAG.y - gMin_MAG.y) / (gMax_MAG.y - gMin_MAG.y) * 2.0 - 1.0;
      gMAG.z = (gMAG.z - gMin_MAG.z) / (gMax_MAG.z - gMin_MAG.z) * 2.0 - 1.0;
      
      int heading3D = GetHeading3D( &gACC, &gMAG, &p );
      
      int heading3D_averaged = GetAverageHeading( heading3D );
      
      // -> 5 degrees resolution ...
      heading3D_averaged = 5 * (heading3D_averaged / 5);
      
      //SetCurrentHeading( heading3D );
      SetCurrentHeading( heading3D_averaged );
      
      CompassMessageInit();  // reset decoding engine
      
      msg_complete = FALSE;  // ready to wait for next message
    }
  }
  else { // UART receive error
  }
}

// --------------------------------------------------------------------------

#define N_VALUES    5

// perform an averaging of several heading values
// handles also the averaging at 359/0 degrees
// negative values are invalid
static int GetAverageHeading(int cur_heading) {

  static int last_average = 0;
  static int last_heading[N_VALUES] = { 0 };
  static uint8_t n_values = 0;
  
  // normalize input value
  while ( cur_heading >= 360 ) cur_heading -= 360;
  while ( cur_heading < 0 ) cur_heading += 360;
  
  int average = 0;
  
  if ( n_values < N_VALUES ) {
    last_heading[n_values++] = cur_heading;
  
    // take current value as average ...
    average = cur_heading;
    
    last_average = average;
  
    return average;
  }
  else {
    
    for ( uint8_t i=1; i<n_values; ++i )
      last_heading[i-1] = last_heading[i];
    
    last_heading[n_values-1] = cur_heading;
  }
  
  // calculate the sum, take 359/0 degree issue into account
  for ( uint8_t i=0; i<n_values; ++i ) {
    if ( last_average > 270 && last_heading[i] < 90 )
      average += last_heading[i] + 360;
    else if ( last_average < 90 && last_heading[i] > 270 )
      average += last_heading[i] - 360;
    else
      average += last_heading[i];
  }
  
  average /= n_values;
  
  // finally normalize the average
  if ( average >= 360 )
    average -= 360;
  else if ( average < 0 )
    average += 360;
  
  last_average = average;
  
  return average;
}

// --------------------------------------------------------------------------

typedef enum {

  kSENTENCE_TYPE_UNKNOWN = 0,
  kSENTENCE_TYPE_ACOK,
  kSENTENCE_TYPE_ACERR,
  kSENTENCE_TYPE_ACRAW,
  
} ESentenceType;

static uint8_t gSentenceType = kSENTENCE_TYPE_UNKNOWN;

/* raw data string of individual quantities */

static char gACC_x_raw[7];
static char gACC_y_raw[7];
static char gACC_z_raw[7];

static char gMAG_x_raw[5];
static char gMAG_y_raw[5];
static char gMAG_z_raw[5];

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
      case 1: gACC_x_raw[index++] = newchar; return FALSE;
      case 2: gACC_y_raw[index++] = newchar; return FALSE;
      case 3: gACC_z_raw[index++] = newchar; return FALSE;
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

  for (uint8_t i=0; i<sizeof(gACC_x_raw); ++i) {
    gACC_x_raw[i] = 0;
    gACC_y_raw[i] = 0;
    gACC_z_raw[i] = 0;
  }

  for (uint8_t i=0; i<sizeof(gMAG_x_raw); ++i) {
    gMAG_x_raw[i] = 0;
    gMAG_y_raw[i] = 0;
    gMAG_z_raw[i] = 0;
  }
}

// --------------------------------------------------------------------------

static void CompassMessageConvert(i_vector_t* acc,i_vector_t* mag) {

  if ( !acc || !mag ) return;

  acc->x = atoi( gACC_x_raw );
  acc->y = atoi( gACC_y_raw );
  acc->z = atoi( gACC_z_raw );

  mag->x = atoi( gMAG_x_raw );
  mag->y = atoi( gMAG_y_raw );
  mag->z = atoi( gMAG_z_raw );
}

// --------------------------------------------------------------------------

// Returns a heading (in degrees) given an acceleration vector a due to gravity, a magnetic vector m, and a facing vector p.
int GetHeading3D(const vector_t *a, const vector_t *m, const vector_t *p) {

  vector_t E;
  vector_t N;

  // cross magnetic vector (magnetic north + inclination) with "down" (acceleration vector) to produce "east"
  vector_cross(m, a, &E);
  vector_normalize(&E);

  // cross "down" with "east" to produce "north" (parallel to the ground)
  vector_cross(a, &E, &N);
  vector_normalize(&N);

  // compute heading
  int heading = round(atan2(vector_dot(&E, p), vector_dot(&N, p)) * 180 / M_PI);
  if ( heading < 0 )
    heading += 360;
  
  return heading;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
