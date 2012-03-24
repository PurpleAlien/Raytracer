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

#ifndef _3DSLOADER_H_
#define _3DSLOADER_H_

#define MAX_VERTICES 10000 // Max number of vertices (for each object)
#define MAX_POLYGONS 10000 // Max number of polygons (for each object)

/* Vertex */
typedef struct{
    float x,y,z;
}vertex_type;

/* Polygon */
typedef struct{
    int a,b,c;
}polygon_type;

typedef struct{
    float u,v;
}mapcoord_type;

/* Object */
typedef struct {
	char name[20];
    
        int vertices_qty;
        int polygons_qty;

        vertex_type vertex[MAX_VERTICES]; 
        polygon_type polygon[MAX_POLYGONS];
        mapcoord_type mapcoord[MAX_VERTICES];
        
}obj_type;

char Load3DS(obj_type *p_object, char *p_filename);

#endif /* _3DSLOADER_H_ */
