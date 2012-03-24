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

#include "includes.h"

/* Check if a ray collides with a sphere */
bool collideRaySphere(ray *r, sphere *s, double *t)
{

        /* Ray/Sphere intersection
		 *
         * collideRaySpere(ray,sphere)
         *      Vector distance = ray.start - sphere.position
         *      B = distance dot ray.direction
         *      C = distance dot distance - sphere.radius^2
         *	Discriminant = B^2 - C
         *	Discriminant < 0 --> no collision
         *	Otherwise select closest solution
         */

        vector dist = vectorSub(&r->start, &s->pos);
        double B = vectorDot(&dist, &r->dir);

        double D = B * B - vectorDot(&dist, &dist) + s->size * s->size;

        /* Discriminant less than 0 ? */
        if (D < 0.0f)
                return FALSE;

        double t0 = -B - sqrtf(D);
        double t1 = -B + sqrtf(D);

        bool retvalue = FALSE;

        if ((t0 > 0.1f) && (t0 < *t)) {
                *t = t0;
                retvalue = TRUE;
        }
        if ((t1 > 0.1f) && (t1 < *t)) {
                *t = t1;
                retvalue = TRUE;
        }

        return retvalue;
}

