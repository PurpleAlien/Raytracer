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

#ifndef _RAYTRACE_H_
#define _RAYTRACE_H_

#include "colour.h"
#include "vectors.h"


#define GOURAUD 0
#define MARBLE 1
#define TURBULENCE 2
/* Material definition */
typedef struct{
        /* Type of Material */
        int MatType;
        colour diffuse;
        /* Second diffuse for procedural materials */
        colour mdiffuse;
        double bump, reflection;
        colour specular;
        double power;
}material;

/* Light definition */
typedef struct{
        vector pos;
        colour intensity;
}light;

/* Lightray definition */
typedef struct{
        vector start;
        vector dir;
}ray;

#define ORTHOGONAL 0
#define CONIC 1
typedef struct{
    unsigned char type;
    
    double FOV;
    double clearPoint;
    double dispersion;
}perspective;

#endif /* _RAYTRACE_H_ */
