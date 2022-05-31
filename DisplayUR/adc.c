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

#include "adc.h"

volatile unsigned long display_value_1;
volatile unsigned long display_value_2;
volatile unsigned int adc_count;

//---------------------------------------------------------------------------
//ADC Interrupt
void adc_init (void)
 {
  //ADC initialisieren
  ADMUX = (1 << MUX0);
  //Free Running Mode, Division Factor 128, Interrupt on
  ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADFR)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADIE);
}

//---------------------------------------------------------------------------
//ADC Interrupt
ISR (ADC_vect)
 {
  ADCSRA = 0;
  if ((ADMUX&0x0F) == 1) {
    //ADC (SPANNUNG)
    ADMUX = 0;
    display_value_1 += ADC*0.44;
  }
  else {
    //ADC (STROM)
    ADMUX = (1<<MUX0);
    display_value_2 += ADC*0.9;
    adc_count++;
  }
  ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADFR)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADIE);
}
