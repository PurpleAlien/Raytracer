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
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "includes.h"

/* Threads */
#define NUMTHREADS 2 //Make sure HEIGHT/NUMTHREADS is even!
#include <pthread.h>

/* Global variables, so threads can access them */
scene *myScene = NULL;
perlin *myNoise = NULL;
unsigned char *img = NULL;
int sectionsize = 0;

/* Renderthread prototype */
void *renderThread(void *arg);

/* Other Prototypes */
colour raytrace(ray *viewRay, scene *myScene);

/* Thread information structure */
typedef struct {                /* Used as argument to thread_start() */
        pthread_t thread_id;        /* ID returned by pthread_create() */
        int       thread_num;       /* Application-defined thread # */
}thread_info;


/* Not really the best or most accurate way, but it works... */
double AutoExposure(scene *myScene)
{
#define ACCUMULATION_SIZE 256
        double exposure = -1.0f;
        double accumulationfactor = (double)max(myScene->width, myScene->height);
        double projectionDistance = 0.0f;

        if ((myScene->persp.type == CONIC) && myScene->persp.FOV > 0.0f && myScene->persp.FOV < 189.0f) {
                projectionDistance = 0.5f * myScene->width / tanf ((double)(PIOVER180) * 0.5f * myScene->persp.FOV);
        }

        accumulationfactor = accumulationfactor / ACCUMULATION_SIZE;
        colour mediumPoint = {0.0f, 0.0f, 0.0f};
        const double mediumPointWeight = 1.0f / (ACCUMULATION_SIZE*ACCUMULATION_SIZE);
        int x,y;
        for (y = 0; y < ACCUMULATION_SIZE; ++y) {
                for (x = 0 ; x < ACCUMULATION_SIZE; ++x) {

                        if (myScene->persp.type == ORTHOGONAL || projectionDistance == 0.0f) {

                                ray viewRay = { {(double)x*accumulationfactor, (double)y * accumulationfactor, -10000.0f}, { 0.0f, 0.0f, 1.0f}};
                                colour currentColor = raytrace(&viewRay, myScene);
                                colour tmp = colourMul(&currentColor, &currentColor);
                                tmp = colourCoefMul(mediumPointWeight, &tmp);
                                mediumPoint = colourAdd(&mediumPoint,&tmp);
                        } else {
                                vector dir = {((double)(x)*accumulationfactor - 0.5f * myScene->width) / projectionDistance,
                                              ((double)(y)*accumulationfactor - 0.5f * myScene->height) / projectionDistance,
                                              1.0f
                                             };
                                double norm = vectorDot(&dir,&dir);
                                if (norm == 0.0f)
                                        break;
                                dir = vectorScale(invsqrtf(norm), &dir);
                                ray viewRay = { {0.5f * myScene->width,  0.5f * myScene->height, 0.0f}, {dir.x, dir.y, dir.z} };
                                colour currentColor = raytrace(&viewRay, myScene);
                                colour tmp = colourMul(&currentColor, &currentColor);
                                tmp = colourCoefMul(mediumPointWeight, &tmp);
                                mediumPoint = colourAdd(&mediumPoint,&tmp);
                        }
                }
        }

        double mediumLuminance = sqrtf(0.2126f * mediumPoint.red + 0.715160f * mediumPoint.green + 0.072169f * mediumPoint.blue);
        if (mediumLuminance > 0.001f) {
                // put the medium luminance to an intermediate gray value
                exposure = logf(0.6f) / mediumLuminance;
        }
        return exposure;
}



colour raytrace(ray *viewRay, scene *myScene)
{
        colour output = {0.0f, 0.0f, 0.0f};
        double coef = 1.0f;
        int level = 0;
	
        do {
                vector hitpoint,n;
                int currentSphere = -1;
                int currentTriangle = -1;
                material currentMat;
                double t = 20000.0f;
                double temp;
                vector n1;

                unsigned int i;
                for (i = 0; i < myScene->numSpheres; ++i) {
                        if (collideRaySphere(viewRay, &myScene->spheres[i], &t)) {
                                currentSphere = i;
                        }
                }

                for (i = 0; i < myScene->numTriangles; ++i) {
                        if (collideRayTriangle(viewRay, &myScene->triangles[i], &t, &n1)) {
                                currentTriangle = i;
                                currentSphere = -1;
                        }
                }

                if (currentSphere != -1) {
                        vector scaled = vectorScale(t, &viewRay->dir);
                        hitpoint = vectorAdd(&viewRay->start, &scaled);
                        n = vectorSub(&hitpoint, &myScene->spheres[currentSphere].pos);
                        temp = vectorDot(&n,&n);
                        if (temp == 0.0f) break;
                        temp = invsqrtf(temp);
                        n = vectorScale(temp, &n);
                        currentMat = myScene->materials[myScene->spheres[currentSphere].material];
                } else if (currentTriangle != -1) {
                        vector scaled = vectorScale(t, &viewRay->dir);
                        hitpoint = vectorAdd(&viewRay->start, &scaled);
                        n=n1;
                        temp = vectorDot(&n,&n);
                        if (temp == 0.0f) break;
                        temp = invsqrtf(temp);
                        n = vectorScale(temp, &n);
                        currentMat = myScene->materials[myScene->triangles[currentTriangle].material];
                } else {
                        /* No hit - add background */
                       	colour test = {0.05,0.05,0.35};
                        colour tmp = colourCoefMul(coef, &test);
                        output = colourAdd(&output,&tmp);
                        break;
                }

                /* Bump mapping using Perlin noise */
                if (currentMat.bump != 0.0) {

                        double noiseCoefx = noise(0.1 * hitpoint.x, 0.1 * hitpoint.y, 0.1 * hitpoint.z, myNoise);
                        double noiseCoefy = noise(0.1 * hitpoint.y, 0.1 * hitpoint.z, 0.1 * hitpoint.x, myNoise);
                        double noiseCoefz = noise(0.1 * hitpoint.z, 0.1 * hitpoint.x, 0.1 * hitpoint.y, myNoise);


                        n.x = (1.0f - currentMat.bump ) * n.x + currentMat.bump * noiseCoefx;
                        n.y = (1.0f - currentMat.bump ) * n.y + currentMat.bump * noiseCoefy;
                        n.z = (1.0f - currentMat.bump ) * n.z + currentMat.bump * noiseCoefz;

                        temp = vectorDot(&n, &n);
                        if (temp == 0.0f) {
                                break;
                        }
                        temp = invsqrtf(temp);
                        n = vectorScale(temp, &n);
                }

                bool inside;

                if (vectorDot(&n,&viewRay->dir) > 0.0f) {
                        n = vectorScale(-1.0f,&n);
                        inside = TRUE;
                } else {
                        inside = FALSE;
                }

                if (!inside) {

                        ray lightRay;
                        lightRay.start = hitpoint;

                        /* Find the value of the light at this point */
                        unsigned int j;
                        for (j = 0; j < myScene->numLights; ++j) {
                                light currentLight = myScene->lights[j];
                                lightRay.dir = vectorSub(&currentLight.pos,&hitpoint);

                                double lightprojection = vectorDot(&lightRay.dir,&n);

                                if ( lightprojection <= 0.0f ) continue;

                                double lightdist = vectorDot(&lightRay.dir,&lightRay.dir);
                                double temp = lightdist;

                                if ( temp == 0.0f ) continue;
                                temp = invsqrtf(temp);
                                lightRay.dir = vectorScale(temp,&lightRay.dir);
                                lightprojection = temp * lightprojection;

                                /* Calculate the shadows */
                                bool inshadow = FALSE;
                                double t = lightdist;

                                unsigned int k;

                                for (k = 0; k < myScene->numSpheres; ++k) {
                                        if (collideRaySphere(&lightRay, &myScene->spheres[k], &t)) {
                                                inshadow = TRUE;
                                                break;
                                        }

                                }

                                for (k = 0; k < myScene->numTriangles; ++k) {
                                        if (collideRayTriangle(&lightRay, &myScene->triangles[k], &t, &n1)) {
                                                inshadow = TRUE;
                                                break;
                                        }
                                }


                                if (!inshadow) {
                                        /* Diffuse refraction using Lambert */
                                        double lambert = vectorDot(&lightRay.dir, &n) * coef;
                                        double noiseCoef = 0.0f;
                                        int level = 0;
                                        switch (currentMat.MatType) {

                                        case TURBULENCE:
                                                for (level = 1; level < 10; level++) {
                                                        noiseCoef += (1.0f / level )
                                                                     * fabsf(noise(level * 0.05 * hitpoint.x,
                                                                                   level * 0.05 * hitpoint.y,
                                                                                   level * 0.05 * hitpoint.z,
                                                                                   myNoise));
                                                }
                                                colour diff1 = colourCoefMul(noiseCoef,&currentMat.diffuse);
                                                colour diff2 = colourCoefMul((1.0f - noiseCoef), &currentMat.mdiffuse);
                                                colour temp1 = colourAdd(&diff1, &diff2);
                                                output = colourAdd(&output, &temp1);
                                                break;

                                        case MARBLE:
                                                for (level = 1; level < 10; level ++) {
                                                        noiseCoef +=  (1.0f / level)
                                                                      * fabsf(noise(level * 0.05 * hitpoint.x,
                                                                                    level * 0.05 * hitpoint.y,
                                                                                    level * 0.05 * hitpoint.z,
                                                                                    myNoise));
                                                }
                                                noiseCoef = 0.5f * sinf( (hitpoint.x + hitpoint.y) * 0.05f + noiseCoef) + 0.5f;
                                                colour diff3 = colourCoefMul(noiseCoef,&currentMat.diffuse);
                                                colour diff4 = colourCoefMul((1.0f - noiseCoef), &currentMat.mdiffuse);
                                                colour tmp1 = colourAdd(&diff3, &diff4);

                                                colour lamint = colourCoefMul(lambert, &currentLight.intensity);
                                                colour lamint_scaled = colourCoefMul(coef, &lamint);

                                                colour temp2 = colourMul(&lamint_scaled, &tmp1);
                                                output = colourAdd(&output, &temp2);
                                                break;

                                                /* Basically Gouraud */
                                        default:

                                                output.red += lambert * currentLight.intensity.red * currentMat.diffuse.red;
                                                output.green += lambert * currentLight.intensity.green * currentMat.diffuse.green;
                                                output.blue += lambert * currentLight.intensity.blue * currentMat.diffuse.blue;
                                                break;
                                        }

                                        /* Blinn */
                                        double viewprojection = vectorDot(&viewRay->dir, &n);
                                        vector blinnDir = vectorSub(&lightRay.dir, &viewRay->dir);
                                        double temp = vectorDot(&blinnDir, &blinnDir);
                                        if (temp != 0.0f ) {
                                                double blinn = invsqrtf(temp) * max(lightprojection - viewprojection , 0.0f);
                                                blinn = coef * powf(blinn, currentMat.power);
                                                colour tmp_1 = colourCoefMul(blinn, &currentMat.specular);
                                                colour tmp_2 = colourMul(&tmp_1, &currentLight.intensity);
                                                output = colourAdd(&output, &tmp_2);
                                        }

                                }
                        }
                        /* Iterate over the reflection */

                        coef *= currentMat.reflection;
                        double reflect = 2.0f * vectorDot(&viewRay->dir, &n);
                        viewRay->start = hitpoint;

                        vector tmp = vectorScale(reflect, &n);
                        viewRay->dir = vectorSub(&viewRay->dir, &tmp);
                }
                level++;
                /* Limit iteration depth to 10 */
        } while ((coef > 0.0f) && (level < 10));
        return output;
}

void *renderThread(void *arg)
{

        int x,y;
        thread_info *tinfo = (thread_info *)arg;

        /* Calculate which part to render based on thread id */
        int limits[]={(tinfo->thread_num*sectionsize),(tinfo->thread_num*sectionsize)+sectionsize};

        /* Autoexposure */
        double exposure = AutoExposure(myScene);

        double projectionDistance = 0.0f;
        if ((myScene->persp.type == CONIC) && myScene->persp.FOV > 0.0f && myScene->persp.FOV < 189.0f) {
                projectionDistance = 0.5f * myScene->width / tanf((double)(PIOVER180) * 0.5f * myScene->persp.FOV);
        }


        for (y = limits[0]; y < limits[1]; ++y) {
                for (x = 0; x < myScene->width; ++x) {
                        colour output = {0.0f, 0.0f, 0.0f};
                        double fragmentx, fragmenty;

                        /* Antialiasing */
                        for (fragmentx = x; fragmentx < x + 1.0f; fragmentx += 0.5f) {
                                for (fragmenty = y; fragmenty < y + 1.0f; fragmenty += 0.5f) {
                                        double sampleRatio=0.25f;
                                        colour temp = {0.0f, 0.0f, 0.0f};
                                        double totalWeight = 0.0f;

                                        if (myScene->persp.type == ORTHOGONAL || projectionDistance == 0.0f) {
                                                ray viewRay = { {fragmentx, fragmenty, -10000.0f},{ 0.0f, 0.0f, 1.0f}};
                                                int i;
                                                for (i = 0; i < myScene->complexity; ++i) {
                                                        colour result = raytrace(&viewRay, myScene);
                                                        totalWeight += 1.0f;
                                                        temp = colourAdd(&temp,&result);
                                                }
                                                temp = colourCoefMul((1.0f/totalWeight), &temp);
                                        } else {
                                                vector dir = {(fragmentx - 0.5f * myScene->width) / projectionDistance,
                                                              (fragmenty - 0.5f * myScene->height) / projectionDistance,
                                                              1.0f
                                                             };

                                                double norm = vectorDot(&dir,&dir);
                                                if (norm == 0.0f)
                                                        break;

                                                dir = vectorScale(invsqrtf(norm),&dir);

                                                vector start = {0.5f * myScene->width,  0.5f * myScene->height, 0.0f};

                                                vector tmp = vectorScale(myScene->persp.clearPoint,&dir);
                                                vector observed = vectorAdd(&start,&tmp);

                                                int i;
                                                for (i = 0; i < myScene->complexity; ++i) {
                                                        ray viewRay = { {start.x, start.y, start.z}, {dir.x, dir.y, dir.z} };

                                                        if (myScene->persp.dispersion != 0.0f) {
                                                                vector disturbance;
                                                                disturbance.x = (myScene->persp.dispersion / RAND_MAX) * (1.0f * rand());
                                                                disturbance.y = (myScene->persp.dispersion / RAND_MAX) * (1.0f * rand());
                                                                disturbance.z = 0.0f;

                                                                viewRay.start = vectorAdd(&viewRay.start, &disturbance);
                                                                viewRay.dir = vectorSub(&observed, &viewRay.start);
                                                                norm = vectorDot(&viewRay.dir,&viewRay.dir);

                                                                if (norm == 0.0f)
                                                                        break;

                                                                viewRay.dir = vectorScale(invsqrtf(norm),&viewRay.dir);
                                                        }
                                                        colour result = raytrace(&viewRay, myScene);
                                                        totalWeight += 1.0f;
                                                        temp = colourAdd(&temp,&result);
                                                }
                                                temp = colourCoefMul((1.0f/totalWeight), &temp);
                                        }

                                        temp.blue = (1.0f - expf(temp.blue * exposure));
                                        temp.red =  (1.0f - expf(temp.red * exposure));
                                        temp.green = (1.0f - expf(temp.green * exposure));

                                        colour adjusted = colourCoefMul(sampleRatio, &temp);
                                        output = colourAdd(&output, &adjusted);
                                }
                        }

                        /* gamma correction */
                        double invgamma = 0.45f; //Fixed value from sRGB standard
                        output.blue = powf(output.blue, invgamma);
                        output.red = powf(output.red, invgamma);
                        output.green = powf(output.green, invgamma);

                        img[(x+y*myScene->width)*3+2] = (unsigned char)min(output.red*255.0f, 255.0f);
                        img[(x+y*myScene->width)*3+1] = (unsigned char)min(output.green*255.0f, 255.0f);
                        img[(x+y*myScene->width)*3+0] = (unsigned char)min(output.blue*255.0f,255.0f);
                }
        }
        pthread_exit((void *) arg);
}


int main(int argc, char *argv[])
{

        /* Scene */
        scene mine;
        mine.materials = NULL;
        mine.spheres = NULL;
        mine.triangles = NULL;
        mine.lights = NULL;
        myScene = &mine;

        perlin noise;
        myNoise = &noise;
        noise_init(myNoise);

        /* Threads */
        thread_info *tinfo;
        pthread_attr_t attr;
        int t;

        /* Allocate memory for pthread_create() arguments */
        tinfo = calloc(NUMTHREADS, sizeof(thread_info));
        if (tinfo == NULL) {
                printf("Error allocating memory for pthread_create() arguments. \n");
                return -1;
        }

        if (argc < 2) {
                printf("specify scene file. \n");
                return -1;
        }

        /* Build the scene */
        if (tokenizer(argv[1], myScene) == -1) {
                exit(-1);
        }


        /* Let's try the 3ds loader... */
        if (argc == 3) {

                obj_type object;
                Load3DS(&object, argv[2]); /* spaceship.3ds */
                int index;

                printf("Polygons: %d \n", object.polygons_qty);

                myScene->triangles = (triangle *)realloc(myScene->triangles, object.polygons_qty*(sizeof(triangle)));
                memset(myScene->triangles,0,sizeof(triangle));

                myScene->numTriangles = object.polygons_qty;
                for (index=0;index<object.polygons_qty;index++) {
                        /* First Vertex */
                        myScene->triangles[index].v2.x = object.vertex[object.polygon[index].a].x+130;
                        myScene->triangles[index].v2.y = object.vertex[object.polygon[index].a].y+150;
                        myScene->triangles[index].v2.z = object.vertex[object.polygon[index].a].z+350;
                        myScene->triangles[index].v2 = vectorScale(2,&myScene->triangles[index].v2);
                        /* Second Vertex */
                        myScene->triangles[index].v1.x = object.vertex[object.polygon[index].b].x+130;
                        myScene->triangles[index].v1.y = object.vertex[object.polygon[index].b].y+150;
                        myScene->triangles[index].v1.z = object.vertex[object.polygon[index].b].z+350;
                        myScene->triangles[index].v1 = vectorScale(2,&myScene->triangles[index].v1);
                        /* Third Vertex */
                        myScene->triangles[index].v3.x = object.vertex[object.polygon[index].c].x+130;
                        myScene->triangles[index].v3.y = object.vertex[object.polygon[index].c].y+150;
                        myScene->triangles[index].v3.z = object.vertex[object.polygon[index].c].z+350;
                        myScene->triangles[index].v3 = vectorScale(2,&myScene->triangles[index].v3);

                        /* Default Material */
                        myScene->triangles[index].material = 3;
                }
        }

        /* Allocate memory for the image */
        if (img)
                free(img);
        img = (unsigned char *)malloc(3*myScene->width*myScene->height);
        memset(img, 0, 3*myScene->width*myScene->height);

        /* Calculate section size per thread */
        sectionsize = myScene->height/NUMTHREADS;
        if ((sectionsize % 2)!=0)
                printf("Warning: Height/numthreads not even - there will be errors in the image! \n");

        /* Pthread options */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        /* Create the render threads */
        for (t=0; t<NUMTHREADS; t++) {
                tinfo[t].thread_num = t;
                if (pthread_create(&tinfo[t].thread_id, &attr, renderThread, &tinfo[t])) {
                        printf("Creation of thread %d failed. \n", t);
                        exit(-1);
                }
        }

        if (pthread_attr_destroy(&attr)) {
                printf("Error destroying thread attributes. \n");
        }

        /* Wait for render threads to finish */
        for (t=0; t<NUMTHREADS; t++) {
                if (pthread_join(tinfo[t].thread_id, NULL)) {
                        printf("Error waiting for thread. \n");
                }
        }

        /* Save the image */
        savebmp("out.bmp", img, myScene);

        /* Free allocated memory */
        if (img)
                free(img);
        if (myScene->lights)
                free(myScene->lights);
        if (myScene->triangles)
                free(myScene->triangles);
        if (myScene->spheres)
                free(myScene->spheres);
        if (myScene->materials)
                free(myScene->materials);

        /* Exit */
        pthread_exit(NULL);

        if (tinfo)
                free(tinfo);

        return 0;
}
