
//
// File   : analyzedat.cc
//
// Purpose: Evaluation of data of LSM303DLH stored in file
//
// $Id: analyzedat.cc,v 1.5 2012/06/11 14:38:03 mathes Exp $
//


#include <iostream>
#include <string>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unistd.h>   // getopt()

/** @file analyzedat.cc
  * Evaluation of LSM303DLH data from file.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

#ifdef DO_HISTOGRAMS
 #include <TFile.h>
 #include <TH1.h>
#endif // DO_HISTOGRAMS

using namespace std;

#include "../LSM303/vector.c"
#include "common.cc"

#define USE_NMEA_FORMAT   1

// ---------------------------------------------------------------------------

//
// g++ -g -Wall -o analyzedat analyzedat.cc
//

bool ReadTagFormat(FILE *file,vector_t *a,vector_t *m)
 {
  if ( feof(file) ) return false;

  if ( !a || !m ) return false;

  bool status = true;
  bool a_read = false, m_read = false;
  char line[80];

  do {

    fgets( line, sizeof(line)-1, file );

    if ( feof(file) ) return false;

    if ( strstr(line, "ADATA") != NULL ) {
      if ( sscanf( line, "ADATA %f %f %f", &a->x, &a->y, &a->z ) == 3 )
        a_read = true;
      else
        status = false;
    }
    else if ( strstr(line, "MDATA") != NULL ) {
      if ( sscanf( line, "MDATA %f %f %f", &m->x, &m->y, &m->z ) == 3 )
        m_read = true;
      else
        status = false;

      //cout << "mx " << m->x << " my " << m->y << " mz " << m->z << endl;
    }

  } while ( !a_read || !m_read );

  return status;
}

// ---------------------------------------------------------------------------

#if (defined USE_NMEA_FORMAT)
# define ReadData(_file_,_a_,_m_) ReadNMEAFormat(_file_,_a_,_m_)
#else
# define ReadData(_file_,_a_,_m_) ReadTagFormat(_file_,_a_,_m_)
#endif // USE_NMEA_FORMAT

static void Usage(const char *argv0)
 {
  cout << "Usage: " << argv0 << " -i <input_file>" << endl;
  cout << endl;
  cout << "where" << endl;
  cout << "\t-c               : do first a calibration" << endl;
  cout << "\t-i <input_file>  : name of input file" << endl;
  cout << "\t-h,-?            : display this help page" << endl;
  cout << endl;
}

// ---------------------------------------------------------------------------

// at home (mockup, 2012-06-05, on bread-board)
vector_t gMinDefault_MAG = { -474, -257, -257 };
vector_t gMaxDefault_MAG = {   36,  238,  238 };

int main(int argc,char **argv)
 {
  string input_filename("lsm303-nmea.dat");
  bool do_calibrate = false;

  // --- check and read program parameters from the command line

  if ( argc < 3 ) {
    Usage( argv[0] );
    exit( EXIT_FAILURE );
  }

  int getopt_status;

  do {

    getopt_status = getopt( argc, argv, "chi:?" );

    if ( getopt_status != EOF ) {

      switch ( getopt_status ) {

        case 'c': do_calibrate = true;
                  break;

	case 'i': input_filename = optarg;
                  break;

        case 'h':
        case '?': Usage(argv[0]);
                  break;

        default: cout << "Encountered unknown option: " << getopt_status
	              << "," << (char)getopt_status << endl;
      }
    }

  } while ( getopt_status != EOF );

  // --- program setup

  cout << argv[0] << ": reading from file " << input_filename << " ..." << endl;

  FILE *file = fopen( input_filename.c_str(), "r" );
  if ( !file ) {
    cerr << argv[0] << ": error opening file!" << endl;
    exit( EXIT_FAILURE );
  }

#ifdef DO_HISTOGRAMS
  string histo_filename = input_filename;
  histo_filename.erase( histo_filename.find(".dat"), string::npos );
  histo_filename += ".root";

  TFile *hfile = new TFile( histo_filename.c_str(), "recreate" );

  TH1F *hhead = new TH1F( "head", "Calculated heading", 360, -0.5, 359.5 );
  TH1F *dhead = new TH1F( "dhead", "Difference 3D-2D-heading", 11, -10.0, 10.0 );

  TH1F *hdvst = new TH1F( "hdvst", "heading versus sample id", 1000, -0.5, 999.5 );
  unsigned int sample_id = 0;
#endif // DO_HISTOGRAMS

  vector_t a, m;

  // --- find minimum and maximum value reading for each axis

  vector_t a_min = {  99999,  99999,  99999 };
  vector_t a_max = { -99999, -99999, -99999 };
  vector_t m_min = {  99999,  99999,  99999 };
  vector_t m_max = { -99999, -99999, -99999 };

  if ( do_calibrate ) {

    do {

      if ( !ReadData( file, &a, &m ) ) break;

      if ( a.x < a_min.x ) a_min.x = a.x;
      if ( a.x > a_max.x ) a_max.x = a.x;
      if ( a.y < a_min.y ) a_min.y = a.y;
      if ( a.y > a_max.y ) a_max.y = a.y;
      if ( a.z < a_min.z ) a_min.z = a.z;
      if ( a.z > a_max.z ) a_max.z = a.z;

      if ( m.x < m_min.x ) m_min.x = m.x;
      if ( m.x > m_max.x ) m_max.x = m.x;
      if ( m.y < m_min.y ) m_min.y = m.y;
      if ( m.y > m_max.y ) m_max.y = m.y;
      if ( m.z < m_min.z ) m_min.z = m.z;
      if ( m.z > m_max.z ) m_max.z = m.z;

    } while ( !feof(file) );

    rewind( file );

  } // do_calibrate
  else {
    m_min = gMinDefault_MAG;
    m_max = gMaxDefault_MAG;
  }

  // --- output minimum and maximum values

  cout << "Reading for magnetic field: " << endl;
  cout << "  min= " << m_min.x << " " << m_min.y
       << " " << m_min.y << endl;
  cout << "  max= " << m_max.x << " " << m_max.y
       << " " << m_max.y << endl;
  cout << "Reading for acceleration: " << endl;
  cout << "  min= " << a_min.x << " " << a_min.y
       << " " << a_min.z << endl;
  cout << "  max= " << a_max.x << " " << a_max.y
       << " " << a_max.z << endl;

  // --- read again, now for data analysis

  vector_t p = {0, -1, 0}; // X: to the right, Y: backward, Z: down

  do {

    if ( !ReadData( file, &a, &m ) ) break;

    // shift and scale
    m.x = (m.x - m_min.x) / (m_max.x - m_min.x) * 2 - 1.0;
    m.y = (m.y - m_min.y) / (m_max.y - m_min.y) * 2 - 1.0;
    m.z = (m.z - m_min.z) / (m_max.z - m_min.z) * 2 - 1.0;

    //cout << "m(x,y)= " << m.x << "," << m.y
    //     << " " << m.x*m.x + m.y*m.y << endl;

    int heading3D = GetHeading3D(&a, &m, &p );

#ifdef DO_HISTOGRAMS
    hdvst->Fill( (float)sample_id, (float)heading3D );
    sample_id++;
#endif // DO_HISTOGRAMS

    vector_t m2 = { 0, 0, 0 };

    m2.x = m.x / sqrt(m.x*m.x + m.y*m.y);
    m2.y = m.y / sqrt(m.x*m.x + m.y*m.y);

    //cout << "m2(x,y)= " << m2.x << "," << m2.y
    //     << " " << m2.x*m2.x + m2.y*m2.y << endl;

    int heading2D = round( atan2( m2.x, m2.y) * 180. / M_PI - 180. );
    if ( heading2D < 0 ) heading2D += 360;

#ifdef DO_HISTOGRAMS
    hhead->Fill( (float)heading3D );
    dhead->Fill( (float)heading3D - (float)heading2D );
#endif // DO_HISTOGRAMS

    cout << "Heading= " << heading3D << " (3D) "
         << heading2D << " (2D)" << endl;

  } while ( !feof(file) );

  // --- cleanup and propgram termination

  fclose( file );

#ifdef DO_HISTOGRAMS
  hfile->Write();
  hfile->Close();
  delete hfile;
#endif // DO_HISTOGRAMS

  exit( EXIT_SUCCESS );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
