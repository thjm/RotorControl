
/*
 * File   : vector.h
 *
 * Purpose: header file for vector.c
 *
 * $Id: vector.h,v 1.2 2011/10/24 16:14:11 mathes Exp $
 *
 */


#ifndef _vector_h_
#define _vector_h_

/** Data structure to hold 3D-vector information. */
typedef struct vector
{
	float x, y, z;
} vector;

/** Calculate the cross-product of two vectors 'a' and 'b'. */
extern void vector_cross(const vector *a, const vector *b, vector *out);

/** Calculate the inner product (dot-product) of two vectors 'a' and 'b'. */
extern float vector_dot(const vector *a, const vector *b);

/** Normalize the given vector, i.e. to have a length of 1. */
extern void vector_normalize(vector *a);

#endif /* _vector_h_ */
