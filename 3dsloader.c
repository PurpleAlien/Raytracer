/* Based on http://www.spacesimulator.net/wiki/index.php?title=Tutorials:3ds_Loader */

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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>

#include "3dsloader.h"
#include "def.h"

long filelength(int f)
{
        struct stat buf;
        fstat(f, &buf);
        return(buf.st_size);
}

/* This should be done with a matrix */
void rotate(float *x, float *y, float *z)
{

        float x2,y2,z2,x3,y3,z3,x4,y4,z4;

		/* rotation angle z-axis, 0 to 2pi */
        x2 = *x * cos(0.5*PI/3) - *y * sin(0.5*PI/3);
        y2 = *x * sin(0.5*PI/3) + *y * cos(0.5*PI/3);
        z2 = *z;

		/* rotate x-axis */
        x3 = x2 * cos(1.8*PI/2) - z2 * sin(1.8*PI/2);
        y3 = y2;
        z3 = x2 * sin(1.8*PI/2) + z2 * cos(1.8*PI/2);

		/* rotate y-axis */
        x4 = x3;
        y4 = y3 * cos(2.2*PI/3) - z3 * sin(2.2*PI/3);
        z4 = y3 * sin(2.2*PI/3) + z3 * cos(2.2*PI/3);

        /* update variables */
		*x = x4;
        *y = y4;
        *z = z4;

}

char Load3DS(obj_type *object, char *filename)
{
        int i;
        int error;
        FILE *file;

		/* chunk identifier */
        unsigned short chunk_id;
		/* chunk lenght */
        unsigned int chunk_length;

        unsigned char uchar;
		/*number of elements in each chunk*/
        unsigned short quantity;

		/* flag that stores some face information */
        unsigned short face_flags;

        if ((file=fopen(filename, "rb"))== NULL) return 0;

		/* Loop to scan the whole file */
        while (ftell (file) < filelength (fileno (file))) {
				/* read the chunk header */
                error = fread(&chunk_id, 2, 1, file);
                if (error != 1)
                        printf("Error reading 3ds chunk header. \n");
                printf("ChunkID: %x\n",chunk_id);
				/* read the lenght of the chunk */
                error = fread (&chunk_length, 4, 1, file);
                if (error != 1)
                        printf("Error reading 3ds chunk length. \n");
                printf("ChunkLenght: %x\n",chunk_length);

                switch (chunk_id) {
                        /* ----------------- MAIN3DS -----------------
                         * Description: Main chunk, contains all the other chunks
                         * Chunk ID: 4d4d
                         * Chunk Lenght: 0 + sub chunks
                         */
                case 0x4d4d:
                        break;
                        /* ----------------- EDIT3DS -----------------
                         * Description: 3D Editor chunk, objects layout info
                         * Chunk ID: 3d3d (hex)
                         * Chunk Lenght: 0 + sub chunks
                         */
				case 0x3d3d:
                        break;
                        /* --------------- EDIT_OBJECT ---------------
                         * Description: Object block, info for each object
                         * Chunk ID: 4000 (hex)
                         * Chunk Lenght: len(object name) + sub chunks
                         */
				case 0x4000:
                        i=0;
                        do {
                                error = fread(&uchar, 1, 1, file);
                                if (error != 1)
                                        printf("Error reading name.\n");
                                object->name[i]=uchar;
                                i++;
                        } while (uchar != '\0' && i<20);
                        break;

                        /* --------------- OBJ_TRIMESH ---------------
                         * Description: Triangular mesh, contains chunks for 3d mesh info
                         * Chunk ID: 4100 (hex)
                         * Chunk Lenght: 0 + sub chunks
                         */
                case 0x4100:
                        break;
                        /* --------------- TRI_VERTEXL ---------------
                         * Description: Vertices list
                         * Chunk ID: 4110 (hex)
                         * Chunk Lenght: 1 x unsigned short (number of vertices)
                         *             + 3 x float (vertex coordinates) x (number of vertices)
                         *             + sub chunks
                         */
                case 0x4110:
                        error = fread (&quantity, sizeof (unsigned short), 1, file);
                        if (error != 1)
                                printf("Error reading 3ds TRI_VERTEXL. \n");
                        object->vertices_qty = quantity;
                        printf("Number of vertices: %d\n",quantity);
                        for (i=0; i<quantity; i++) {
                                error = fread (&object->vertex[i].x, sizeof(float), 1, file);
                                if (error != 1)
                                        printf("Error reading 3ds TRI_VERTEXL vertex x. \n");
                                printf("Vertices list x: %f\n",object->vertex[i].x);
                                error = fread (&object->vertex[i].y, sizeof(float), 1, file);
                                if (error != 1)
                                        printf("Error reading 3ds vertex y. \n");
                                printf("Vertices list y: %f\n",object->vertex[i].y);
                                error = fread (&object->vertex[i].z, sizeof(float), 1, file);
                                if (error != 1)
                                        printf("Error reading 3ds vertex z. \n");
                                printf("Vertices list z: %f\n",object->vertex[i].z);

                                rotate(&object->vertex[i].x,&object->vertex[i].y,&object->vertex[i].z);

                        }
                        break;
                        /* --------------- TRI_FACEL1 ----------------
                         * Description: Polygons (faces) list
                         * Chunk ID: 4120 (hex)
                         * Chunk Lenght: 1 x unsigned short (number of polygons)
                         *             + 3 x unsigned short (polygon points) x (number of polygons)
                         *             + sub chunks
                         */
                case 0x4120:
                        error = fread (&quantity, sizeof (unsigned short), 1, file);
                        if (error != 1)
                                printf("Error reading 3ds TRI_FACEL1. \n");
                        object->polygons_qty = quantity;
                        printf("Number of polygons: %d\n",quantity);

                        for (i=0; i<quantity; i++) {
                                error = fread (&object->polygon[i].a, sizeof (unsigned short), 1, file);
                                if (error != 1)
                                        printf("Error reading 3ds TRI_FACEL1 plygon a. \n");
                                printf("Polygon point a: %d\n",object->polygon[i].a);
                                error = fread (&object->polygon[i].b, sizeof (unsigned short), 1, file);
                                if (error != 1)
                                        printf("Error reading 3ds TRI_FACEL1 plygon b. \n");
                                printf("Polygon point b: %d\n",object->polygon[i].b);
                                error = fread (&object->polygon[i].c, sizeof (unsigned short), 1, file);
                                if (error != 1)
                                        printf("Error reading 3ds TRI_FACEL1 plygon c. \n");
                                printf("Polygon point c: %d\n",object->polygon[i].c);
                                error = fread (&face_flags, sizeof (unsigned short), 1, file);
                                if (error != 1)
                                        printf("Error reading face flags. \n");
                                printf("Face flags: %x\n",face_flags);
                        }
                        break;
                        /* ------------- TRI_MAPPINGCOORS ------------
                         * Description: Vertices list
                         * Chunk ID: 4140 (hex)
                         * Chunk Lenght: 1 x unsigned short (number of mapping points)
                         *             + 2 x float (mapping coordinates) x (number of mapping points)
                         *             + sub chunks
                         */
                case 0x4140:
                        error = fread(&quantity, sizeof (unsigned short), 1, file);
                        if (error != 1)
                                printf("Error reading 3ds TRI_MAPPINGCOORDS\n");
                        for (i=0; i<quantity; i++) {
                                error = fread(&object->mapcoord[i].u, sizeof (float), 1, file);
                                if (error != 1)
                                        printf("Error reading mapping coordinate u. \n");
                                printf("Mapping list u: %f\n",object->mapcoord[i].u);
                                error = fread (&object->mapcoord[i].v, sizeof (float), 1, file);
                                if (error != 1)
                                        printf("Error reading mapping coordinate v. \n");
                                printf("Mapping list v: %f\n",object->mapcoord[i].v);
                        }
                        break;
                        /* ----------- Skip unknow chunks ------------
                         * We need to skip all the chunks that currently we don't use
                         * We use the chunk lenght information to set the file pointer
                         * to the same level next chunk
                         */
                default:
                        fseek(file, chunk_length-6, SEEK_CUR);
                }
        }
		/* Closes the file stream */
        fclose (file);
        return 0;
}
