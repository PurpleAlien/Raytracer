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
#include "includes.h"

/* Output data as bitmap file */
void savebmp(char *filename, unsigned char *img, scene *myScene)
{
        int i;
        int error;
        FILE *f;
        int filesize = 54 + 3*myScene->width*myScene->height;

        unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
        unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
        unsigned char bmppad[3] = {0,0,0};

        /* Construct header with filesize part */
        bmpfileheader[ 2] = (unsigned char)(filesize    );
        bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
        bmpfileheader[ 4] = (unsigned char)(filesize>>16);
        bmpfileheader[ 5] = (unsigned char)(filesize>>24);

        /* Construct header with width and height part */
        bmpinfoheader[ 4] = (unsigned char)(       myScene->width    );
        bmpinfoheader[ 5] = (unsigned char)(       myScene->width>> 8);
        bmpinfoheader[ 6] = (unsigned char)(       myScene->width>>16);
        bmpinfoheader[ 7] = (unsigned char)(       myScene->width>>24);
        bmpinfoheader[ 8] = (unsigned char)(       myScene->height    );
        bmpinfoheader[ 9] = (unsigned char)(       myScene->height>> 8);
        bmpinfoheader[10] = (unsigned char)(       myScene->height>>16);
        bmpinfoheader[11] = (unsigned char)(       myScene->height>>24);

        f = fopen(filename,"wb");
        error = fwrite(bmpfileheader,1,14,f);
        if (error != 14)
                printf("Error writing bmpfileheader \n");
        error = fwrite(bmpinfoheader,1,40,f);
        if (error != 40)
                printf("Error writing bmpinfoheader \n");

        for (i=0; i<myScene->height; i++) {
                error = fwrite(img+(myScene->width*(i)*3),3,myScene->width,f);
                if (error != myScene->width)
                        printf("Error writing line\n");
                error = fwrite(bmppad,1,(4-(myScene->width*3)%4)%4,f);
                if (error != (4-(myScene->width*3)%4)%4)
                        printf("Error writing bmp padding\n");
        }

        fclose(f);
}
