
/*
 * File   : vector.h
 *
 * Purpose: header file for vector.c
 *
 * $Id: vector.h,v 1.3 2012/05/19 11:33:35 mathes Exp $
 *
 */


#ifndef _vector_h_
#define _vector_h_

/** Data structure to hold 3D-vector information. */
typedef struct _vector {

  float x, y, z;

} vector_t;

/** Calculate the cross-product of two vectors 'a' and 'b'. */
extern void vector_cross(const vector_t *a,const vector_t *b,vector_t *out);

/** Calculate the inner product (dot-product) of two vectors 'a' and 'b'. */
extern float vector_dot(const vector_t *a,const vector_t *b);

/** Normalize the given vector, i.e. to have a length of 1. */
extern void vector_normalize(vector_t *a);

#endif /* _vector_h_ */
