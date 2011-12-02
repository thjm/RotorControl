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

volatile int gMultiplexDisplay1;
volatile int gMultiplexDisplay2;

/**  */
extern void MultiplexInit(void);

/**  */
extern void MultiplexOn(void);
extern void MultiplexOff(void);

/**  */
inline void MultiplexSet1(int val1)
 { gMultiplexDisplay1 = val1; }

/**  */
inline void MultiplexSet2(int val2)
 { gMultiplexDisplay2 = val2; }

/**  */
inline void MultiplexSet(int val1, int val2)
 { gMultiplexDisplay1 = val1; gMultiplexDisplay2 = val2; }

#endif //_MULTIPLEX_H
