
/*
 * File   : vector.c
 *
 * Purpose: Vector analysis routines.
 *
 * $Id: vector.c,v 1.2 2011/10/24 16:14:11 mathes Exp $
 *
 */
 
#include <math.h>

/** @file vector.c
  * Vector analysis routines.
  * @author Pololu
  */

#include "vector.h"

/* -------------------------------------------------------------------------- */

void vector_cross(const vector *a, const vector *b, vector *out)
{
	out->x = a->y * b->z - a->z * b->y;
	out->y = a->z * b->x - a->x * b->z;
	out->z = a->x * b->y - a->y * b->x;
}

/* -------------------------------------------------------------------------- */

float vector_dot(const vector *a, const vector *b)
{
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

/* -------------------------------------------------------------------------- */

void vector_normalize(vector *a)
{
	float mag = sqrt(vector_dot(a, a));
	a->x /= mag;
	a->y /= mag;
	a->z /= mag;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
