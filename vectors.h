/*
 * Copyright (C) 2009 by Johan Dams, VAMK <jd@puv.fi>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */

#ifndef _VECTORS_H_
#define _VECTORS_H_

/* Vector definition */
typedef struct{
        double x, y, z;
}vector;


/* Function Prototypes */

/* Add two vectors and return the resulting vector */
vector vectorAdd(vector *v1, vector *v2);
/* Subtract two vectors and return the resulting vector */
vector vectorSub(vector *v1, vector *v2);
/* Multiply two vectors and return the resulting scalar (dot product) */
double vectorDot(vector *v1, vector *v2);
/* Calculate Vector x Scalar */ 
vector vectorScale(double c, vector *v);
/* Calculate Vector cross product */
vector vectorCross(vector *v1, vector *v2);

#endif /* _VECTORS_H_ */
