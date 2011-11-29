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

volatile unsigned char segcounter = 0;
volatile int display1 = 0;
volatile int display2 = 0;


//############################################################################
//Timer3 Interrupt
ISR (TIMER2_OVF_vect)
//############################################################################
{	
	PORTD = 0;
	PORTB = (1<<segcounter);
	
	switch (segcounter)
	{	
		case 0:
			PORTD = SEGMENTE[(display1 % 1000 / 100)];
			break;	
		case 1:
			PORTD = SEGMENTE[(display1 % 100 / 10)];
			break;		
		case 2:
			PORTD = SEGMENTE[(display1 % 10)];
			break;
		case 3:
			PORTD = SEGMENTE[(display2 % 1000 / 100)];
			break;
		case 4:
			PORTD = SEGMENTE[(display2 % 100 / 10)];
			break;	
		case 5:
			PORTD = SEGMENTE[(display2 % 10)];
			break;
	}
	if ((segcounter++)>5) segcounter = 0;	
}
	

//############################################################################
//Diese Routine startet und inizialisiert den Timer
void init_multiplex (void)
//############################################################################
{
	//Interrupt for the Clock enable  
    TIMSK |= (1 << TOIE2);
	//Setzen des Prescaler auf 1024 
	TCCR2 |= (1<<CS22); 
	return;
}


