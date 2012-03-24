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

#include <math.h>

#include "includes.h"

bool collideRayTriangle(ray *r, triangle *t, double *result, vector *normal)
{

        double det, invdet;
        vector edge1 = vectorSub(&t->v2, &t->v1);
        vector edge2 = vectorSub(&t->v3, &t->v1);

        /* Find the cross product of edge2 and the ray direction */
        vector s1 = vectorCross(&r->dir, &edge2);

        det = vectorDot(&edge1, &s1);
        if (det > -0.000001 && det < 0.000001) {
                return FALSE;
        }

        invdet = 1/det;

        vector s2 = vectorSub(&r->start, &t->v1);

        double u = vectorDot(&s2, &s1) * invdet;

        if (u < 0 || u > 1) {
                return FALSE;
        }

        vector s3 = vectorCross(&s2, &edge1);

        double v = vectorDot(&r->dir, &s3) * invdet;

        if (v < 0 || (u + v) > 1) {
                return FALSE;
        }

        double tmp = vectorDot(&edge2, &s3) * invdet;

        if ((tmp < 0) || (tmp > *result)) {
                return FALSE;
        }

        /* subtract tiny amount - otherwise artifacts due to floating point imprecisions... */
        *result = tmp-0.005;
        *normal = vectorCross(&edge2, &edge1);

        return TRUE;
}

