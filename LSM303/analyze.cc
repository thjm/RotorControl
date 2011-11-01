
//
// File   : analyze.cc
//
// Purpose: Evaluation of data of LSM303DLH stored in file
//
// $Id: analyze.cc,v 1.1 2011/11/01 16:18:08 mathes Exp $
//


#include <iostream>
#include <string>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

/** @file compass.cc
  * Evaluation of LSM303DLH data from file.
  * @author H.-J. Mathes <dc2ip@darc.de>
  */

using namespace std;

#include "vector.c"

#define USE_NMEA_FORMAT   1

// ---------------------------------------------------------------------------

//
// g++ -g -Wall -o analyze analyze.cc
//

// Returns a heading (in degrees) given an acceleration vector a due to gravity, a magnetic vector m, and a facing vector p.
int GetHeading3D(const vector *a, const vector *m, const vector *p)
 {
  vector E;
  vector N;

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

// ---------------------------------------------------------------------------

bool ReadNMEAFormat(FILE *file,vector *a,vector *m)
 {
  if ( feof(file) ) return false;
  
  if ( !a || !m ) return false;
  
  bool status = true;
  char line[80];

  do {
    
    fgets( line, sizeof(line)-1, file );
    
    if ( feof(file) ) return false;
  
    if ( strstr(line,"$ACRAW") != NULL ) {
      if ( sscanf( line, "$ACRAW,%f,%f,%f,%f,%f,%f",
                    &a->x, &a->y, &a->z, &m->x, &m->y, &m->z ) == 6 )
        break;
//      else
//        status = false;
    }
    
  } while ( !feof(file ) );
  
  return status;
}

// ---------------------------------------------------------------------------

bool ReadTagFormat(FILE *file,vector *a,vector *m)
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

int main(int argc,char **argv)
 {
  if ( argc < 2 ) {
    cerr << "Usage: " << argv[0] << " <filename>" << endl;
    exit( EXIT_FAILURE );
  }
  
  cout << argv[0] << ": reading from file " << argv[1] << " ..." << endl;
  
  FILE *file = fopen( argv[1], "r" );
  if ( !file ) {
    cerr << argv[0] << ": error opening file!" << endl;
    exit( EXIT_FAILURE );
  }
  
  vector a, m;
  
  // --- find minimum and maximum value reading for each axis
  
  vector a_min = {  99999,  99999,  99999 };
  vector a_max = { -99999, -99999, -99999 };
  vector m_min = {  99999,  99999,  99999 };
  vector m_max = { -99999, -99999, -99999 };
  
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
  
  // --- output minimum and maximum values
  
  cout << "Reading for magnetig field: " << endl;
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
  
  rewind( file );
  
  vector p = {0, -1, 0}; // X: to the right, Y: backward, Z: down
  
  do {
  
    if ( !ReadData( file, &a, &m ) ) break;
    
    // shift and scale
    m.x = (m.x - m_min.x) / (m_max.x - m_min.x) * 2 - 1.0;
    m.y = (m.y - m_min.y) / (m_max.y - m_min.y) * 2 - 1.0;
    m.z = (m.z - m_min.z) / (m_max.z - m_min.z) * 2 - 1.0;
    
    int heading3D = GetHeading3D(&a, &m, &p );
    
    cout << "3D-Heading= " << heading3D << endl;
  
  } while ( !feof(file) );
    
  fclose( file );
  
  exit( EXIT_SUCCESS );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
