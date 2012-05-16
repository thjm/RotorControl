
//
// File   : common.cc
//
// Purpose: Common code parts for C++ analysis stuff...
//
// $Id: common.cc,v 1.1 2012/05/16 21:05:47 mathes Exp $
//


/** @file common.cc
  * Common code parts for C++ analysis stuff...
  */

// ---------------------------------------------------------------------------

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

bool ReadNMEAFormat(const char *line,vector *a,vector *m)
 {
  if ( !line || *line == 0 ) return false;
  
  if ( !a || !m ) return false;
  
  bool status = false;

  if ( strstr(line,"$ACRAW") != NULL ) {
    if ( sscanf( line, "$ACRAW,%f,%f,%f,%f,%f,%f",
  		  &a->x, &a->y, &a->z, &m->x, &m->y, &m->z ) == 6 )
      status = true;
  }
  
  return status;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
