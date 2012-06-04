
//
// File   : compass1.cc
//
// Purpose: Evaluation of data of LSM303DLH read from serial port.
//
// $Id: compass1.cc,v 1.7 2012/06/04 13:43:14 mathes Exp $
//


#include <iostream>
#include <iomanip>
#include <string>
#include <list>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

/** @file compass1.cc
  * Evaluation of data of LSM303DLH read from serial port.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

/* two functions for keyboard interaction */
extern int getch(void);
extern int kbhit(void);

#include <SerialPort.h>

// will conflict with std::vector ...
#include "../LSM303/vector.c"

// using namespace std;
using std::cout;
using std::cerr;
using std::endl;
using std::setw;

#include "common.cc"

enum {

  kModeMask = 0xff,
  kCalibrate = 0,
  kMeasure = 1,
  
  kDebug = 0x8000 

};

static int gOperationMode = kMeasure;

#if 1
// // at home (shack, some time ago)
// vector gMinDefault_MAG = { -364, -535, -535 };
// vector gMaxDefault_MAG = {  202,  -83,  -83 };
// at home (Ira's office, 2012-05-17, new setup)
vector_t gMinDefault_MAG = { -480, -196, -196 };
vector_t gMaxDefault_MAG = {   40,  284,  284 };
#else
// in the office ?
vector_t gMinDefault_MAG = { -236, -135, -135 };
vector_t gMaxDefault_MAG = {   83,  151,  151 };
#endif

// ---------------------------------------------------------------------------

static void UiMenu()
 {
  cout << "\nEnter your choice:" << endl << endl;
  cout << "C, c - enter Calibration mode" << endl;
  cout << "D, d - toggle Debug mode" << endl;
  cout << "M, m - enter Measurement mode (default)" << endl;
  cout << "O, o - toggle continous display" << endl;
  cout << endl;
  cout << "X, x - Exit from program" << endl;
  cout << endl;
}

// ---------------------------------------------------------------------------

//
// compile & link with:
//  export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
//  g++ -g -Wall -o compass1 -lserial compass1.cc
//
// run with:
//  ./compass1 /dev/ttyUSB1
//

int main(int argc, char** argv)
 {
  if ( argc < 2 ) {
    cerr << "Usage: " << argv[0] << " <serial-port>" << endl;
    exit( EXIT_FAILURE );
  }

  // Open the serial port. 
  //
  SerialPort serial_port( argv[1] );

  // Set communication parameters & open the port, catch exceptions
  //
  try {
    serial_port.Open( SerialPort::BAUD_9600,
                      SerialPort::CHAR_SIZE_8,
                      SerialPort::PARITY_NONE,
                      SerialPort::STOP_BITS_1,
                      SerialPort::FLOW_CONTROL_NONE );
  }
  catch ( SerialPort::AlreadyOpen& ex ) {
    cerr << "Error: Port is already open: " << ex.what() << endl ;
    exit(EXIT_FAILURE);
  }
  catch ( SerialPort::OpenFailed& ex ) {
    cerr << "Error: Could not open port: " << ex.what() << endl ;
    exit(EXIT_FAILURE);
  }
  catch ( ... ) {
    cerr << "Caught unknown exception in SerialPort::Open()"
            << " in file " << __FILE__
            << ", line " << __LINE__
            << " !" << endl;
    exit(EXIT_FAILURE);
  }

  if ( !serial_port.IsOpen() ) {
    cerr << argv[0] << ": failed to open " << argv[1] << "!" << endl;
    exit( EXIT_FAILURE );
  }

  char gps_data[240];
  int gps_data_ptr = 0;
  bool gps_msg_start = false, gps_msg_complete = false;
  
  vector_t p = {0, -1, 0}; // X: to the right, Y: backward, Z: down

  vector_t vMin = {  99999,  99999,  99999 };
  vector_t vMax = { -99999, -99999, -99999 };
  
  vector_t m_min = gMinDefault_MAG;
  vector_t m_max = gMaxDefault_MAG;
  
  int last_average = 0; // last average
  std::list<int> headings;
  
  char choice;
  bool display_on = false;
  bool leave = false;
  
  UiMenu();
  
  while ( !leave ) {
  
    if ( kbhit() ) {
    
      choice = getch();

      switch ( choice ) {

	case 'c': // mode 'calibrate'
	case 'C': 
	          gOperationMode = kCalibrate;
                  m_min = vMin; m_max = vMax;
		  break;

	case 'd': // debug mode
	case 'D':
	          gOperationMode ^= kDebug;
		  cout << "DEBUG mode is " << ((gOperationMode & kDebug) ? "ON" : "OFF") << endl;
		  break;

	case 'm': // mode 'measure'
	case 'M': 
	          gOperationMode = kMeasure;
	          
		  // output most recent calibration constants
		    
                  cout << "Reading for magnetic field: " << endl;
                  cout << "  min= " << setw(5) << m_min.x 
		       << " " << setw(5) << m_min.y 
                       << " " << setw(5) << m_min.y << endl; 
                  cout << "  max= " << setw(5) << m_max.x 
		       << " " << setw(5) << m_max.y 
                       << " " << setw(5) << m_max.y << endl; 

                  break;

        case 'o':
	case 'O': display_on = !display_on;
                  break;
	
	case 'x': // exit
        case 'X': leave = true;
                  break;
	
	default: UiMenu();
      }
      
      cout << endl;
      
    } // if ( kbhit() ) ...

    vector_t a, m;
    
    if ( !leave ) {
      
      char data;
      
      if ( serial_port.IsDataAvailable() ) {
    
        data = serial_port.ReadByte( 0 );
	
	if ( gps_data_ptr == sizeof(gps_data) ) {
	  cerr << "Input buffer overrun(" << gps_data_ptr 
	       << "), discarding message!" << endl;
	  gps_data_ptr = 0;
	  gps_msg_start = false;
	}
	
//	if ( gOperationMode & kDebug ) {
//	  cout << data;
//	}
	
	switch ( data ) {
	  
	  case '$':  gps_msg_start = true;
	             gps_data_ptr = 0;
	             gps_data[gps_data_ptr++] = data;
	             break;
	
	  case 0x0d:
	  case 0x0a: gps_msg_start = false;
	             gps_data[gps_data_ptr++] = 0;
	             gps_msg_complete = true;
		     break;
	
          default:   gps_data[gps_data_ptr++] = data;

        } // switch ( data ) ...
      
      } // if ( serial_port.IsDataAvailable() )
      
    }  // if ( !leave ) ...
    
    if ( gps_msg_complete ) {
      
//      if ( gOperationMode & kDebug ) {
//        cout << gps_data;
//      }
      
      gps_msg_complete = false;
      
      if ( gOperationMode == kCalibrate || gOperationMode & kDebug )
        cout << "ACMSG: " << gps_data << endl;

      if ( !ReadNMEAFormat( gps_data, &a, &m ) ) continue;
      
      switch ( gOperationMode & kModeMask ) {
      
        case kCalibrate:
    	     if ( m.x < m_min.x ) m_min.x = m.x;
    	     if ( m.x > m_max.x ) m_max.x = m.x;
    	     if ( m.y < m_min.y ) m_min.y = m.y;
    	     if ( m.y > m_max.y ) m_max.y = m.y;
    	     if ( m.z < m_min.z ) m_min.z = m.z;
    	     if ( m.z > m_max.z ) m_max.z = m.z;
	     break;

	case kMeasure:
	     // shift and scale
             m.x = (m.x - m_min.x) / (m_max.x - m_min.x) * 2 - 1.0;
             m.y = (m.y - m_min.y) / (m_max.y - m_min.y) * 2 - 1.0;
             m.z = (m.z - m_min.z) / (m_max.z - m_min.z) * 2 - 1.0;
    
             int heading3D = GetHeading3D(&a, &m, &p );
    
             if ( display_on )
               cout << "3D-Heading= " << setw(3) << heading3D;
	     
	     headings.push_back( heading3D );

	     if ( headings.size() == 10 ) {

	       int average = 0;

	       for ( std::list<int>::iterator it = headings.begin();
	                                      it != headings.end(); ++it ) {
	         
		 if ( last_average > 270 && *it < 90 )
		   average += *it + 360;
		 else if ( last_average < 90 && *it > 270 )
		   average += *it - 360;
		 else
	           average += *it;
	       }
	       
	       average /= 10;
	       
	       if ( average >= 360 ) 
	         average -= 360;
	       else if ( average < 0 )
	         average += 360;
	       
	       last_average = average;
	       
	       headings.pop_front();
	       
	       if ( display_on )
	         cout << " \t** " << setw(3) << average << " ** \t" 
	              << setw(3) << 5 * ( average / 5);
	     }
	     
	     if ( display_on ) cout << endl;

	     break;
      }
    }
    
    usleep( 100 );
    
  } // while ( !leave )

  cout << endl;

  serial_port.Close();

  exit( EXIT_SUCCESS );
}

/* ------------------------------------------------------------------------- */
/* ---     special functions for linux as kbhit() or getch() on PC       --- */
/* ------------------------------------------------------------------------- */

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>

/** Get a character from the console.
  *
  * It is not necessary to preee <Enter> to send the character to the 
  * application
  */
int getch(void)
 {
  struct termios org_mode, cur_mode;
  int tty_desc;
  char ch;

  tty_desc = open ( "/dev/tty", O_RDWR );

  ioctl ( tty_desc, TCGETS, &org_mode );
  ioctl ( tty_desc, TCGETS, &cur_mode );

  cur_mode.c_iflag &= ~ICRNL;
  cur_mode.c_lflag &= ~(ICANON | ECHO);
  cur_mode.c_cc[VTIME] = 0;
  cur_mode.c_cc[VMIN] = 1;
  ioctl ( tty_desc, TCSETS, &cur_mode );

  read ( tty_desc, &ch, 1 );

  ioctl ( tty_desc, TCSETS, &org_mode );
  close ( tty_desc );

  return ch;
}

/* ------------------------------------------------------------------------- */

/** Return 1 if any key on the keyboard was pressed.
  *
  * The value of the key can then be inquired using the getch() function.
  */
int kbhit(void)
 {
  struct termios org_mode, cur_mode;
  int tty_desc;
  int nchars, result = 0;

  tty_desc = open ( "/dev/tty", O_RDWR );

  ioctl ( tty_desc, TCGETS, &org_mode );
  ioctl ( tty_desc, TCGETS, &cur_mode );

  cur_mode.c_iflag &= ~ICRNL;
  cur_mode.c_lflag &= ~(ICANON | ECHO);
  cur_mode.c_cc[VTIME] = 0;
  cur_mode.c_cc[VMIN] = 1;
  ioctl ( tty_desc, TCSETS, &cur_mode );

  ioctl ( tty_desc, FIONREAD, &nchars );
  if ( nchars > 0 ) result = 1;

  ioctl ( tty_desc, TCSETS, &org_mode );
  close ( tty_desc );

  return result;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
