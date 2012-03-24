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

/* Braindead tokenizer... Beats having the scene hardcoded in a header file. */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "includes.h"

int tokenizer(char *file, scene *myScene)
{

        char *delim_1 = "=";
        char *delim_2 = ",";

        char *token, *subtoken;
        char *saveptr1, *saveptr2;

        int material_index = 0, sphere_index = 0, triangle_index = 0, light_index = 0;

        char *error = NULL;

        FILE *fp;
        fp = fopen(file,"r");

        if (fp != NULL) {
                char line[255];

                while (fgets (line, sizeof line, fp) != NULL) {

                        /* Comments */
                        if (line[0] == '#') {

                        }

                        /* Scene */
                        if (strcmp(line,"Scene{\n") == 0) {
                                while (line[0]!= '}') {
                                        error = fgets(line, sizeof(line), fp);
                                        if (!error)
                                                printf("Error reading line from scene file. \n");
                                        /* Number of materials */
                                        if (line[0] == '\t' && line[1] == 'm') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                int num = strtol(saveptr1, (char **) NULL, 10);
                                                myScene->numMaterials = num;
                                                myScene->materials = (material *)calloc(num, sizeof(material));
                                        }
                                        /* Number of Spheres */
                                        if (line[0] == '\t' && line[1] == 's') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                int num = strtol(saveptr1, (char **) NULL, 10);
                                                myScene->numSpheres = num;
                                                myScene->spheres = (sphere *)calloc(num, sizeof(sphere));
                                        }
                                        /* Number of Triangles */
                                        if (line[0] == '\t' && line[1] == 't') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                int num = strtol(saveptr1, (char **) NULL, 10);
                                                myScene->numTriangles = num;
                                                myScene->triangles = (triangle *)calloc(num, sizeof(triangle));
                                        }
                                        /* Number of Lights */
                                        if (line[0] == '\t' && line[1] == 'l') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                int num = strtol(saveptr1, (char **) NULL, 10);
                                                myScene->numLights = num;
                                                myScene->lights = (light *)calloc(num, sizeof(light));
                                        }
                                        /* Image width */
                                        if (line[0] == '\t' && line[1] == 'w') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->width = strtol(saveptr1, (char **) NULL, 10);
                                        }
                                        /* Image height */
                                        if (line[0] == '\t' && line[1] == 'h') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->height = strtol(saveptr1, (char **) NULL, 10);
                                        }
                                        /* Perspective Type */
                                        if (line[0] == '\t' && line[1] == 'P') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->persp.type = strtol(saveptr1, (char **) NULL, 10);
                                        }
                                        /* FOV */
                                        if (line[0] == '\t' && line[1] == 'F') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->persp.FOV = strtod(saveptr1, (char **) NULL);
                                        }
                                        /* ClearPoint */
                                        if (line[0] == '\t' && line[1] == 'C' && line[2] == 'l') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->persp.clearPoint = strtod(saveptr1, (char **) NULL);
                                        }
                                        /* Dispersion */
                                        if (line[0] == '\t' && line[1] == 'D') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->persp.dispersion = strtod(saveptr1, (char **) NULL);
                                        }
                                        /* Complexity */
                                        if (line[0] == '\t' && line[1] == 'C' && line[2] == 'o') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->complexity = strtol(saveptr1, (char **) NULL, 10);
                                        }

                                }
                        }
                        /* Materials */
                        if (strcmp(line,"Material{\n") == 0) {
                                while (line[0]!= '}') {
                                        error = fgets(line, sizeof(line), fp);
                                        if (!error)
                                                printf("Error reading Material line from scene file. \n");
                                        /* Material Type */
                                        if (line[0] == '\t' && line[1] == 'M') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->materials[material_index].MatType = strtol(saveptr1, (char **) NULL, 10);
                                        }
                                        /* Procedural Material Diffuse */
                                        if (line[0] == '\t' && line[1] == 'm' && line[2] == 'd') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                /* Get three values */
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->materials[material_index].mdiffuse.red = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->materials[material_index].mdiffuse.green = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->materials[material_index].mdiffuse.blue = strtod(subtoken, (char **) NULL);
                                        }
                                        /* Diffuse */
                                        if (line[0] == '\t' && line[1] == 'd' && line[2] == 'i') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                /* Get three values */
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->materials[material_index].diffuse.red = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->materials[material_index].diffuse.green = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->materials[material_index].diffuse.blue = strtod(subtoken, (char **) NULL);
                                        }
                                        /* Reflection */
                                        if (line[0] == '\t' && line[1] == 'r') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->materials[material_index].reflection = strtod(saveptr1, (char **) NULL);
                                        }
                                        /* Bump */
                                        if (line[0] == '\t' && line[1] == 'b') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->materials[material_index].bump = strtod(saveptr1, (char **) NULL);
                                        }
                                        /* Specular */
                                        if (line[0] == '\t' && line[1] == 's') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                /* Get three values */
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->materials[material_index].specular.red = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->materials[material_index].specular.green = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->materials[material_index].specular.blue = strtod(subtoken, (char **) NULL);
                                        }
                                        /* Power */
                                        if (line[0] == '\t' && line[1] == 'p') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->materials[material_index].power = strtod(saveptr1, (char **) NULL);
                                        }
                                }
                                material_index++;
                        }
                        /* Spheres */
                        if (strcmp(line,"Sphere{\n") == 0) {
                                while (line[0]!= '}') {
                                        error = fgets(line, sizeof(line), fp);
                                        if (!error)
                                                printf("Error reading Sphere line from scene file. \n");
                                        /* Position */
                                        if (line[0] == '\t' && line[1] == 'p') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                /* Get three values */
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->spheres[sphere_index].pos.x = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->spheres[sphere_index].pos.y = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->spheres[sphere_index].pos.z = strtod(subtoken, (char **) NULL);
                                        }
                                        /* Size */
                                        if (line[0] == '\t' && line[1] == 's') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->spheres[sphere_index].size = strtod(saveptr1, (char **) NULL);
                                        }
                                        /* Material */
                                        if (line[0] == '\t' && line[1] == 'm') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->spheres[sphere_index].material = strtol(saveptr1, (char **) NULL, 10);
                                        }
                                }
                                sphere_index++;
                        }
                        /* Lights */
                        if (strcmp(line,"Light{\n") == 0) {
                                while (line[0]!= '}') {
                                        error = fgets(line, sizeof(line), fp);
                                        if (!error)
                                                printf("Error reading Light line from scene file. \n");
                                        /* Position */
                                        if (line[0] == '\t' && line[1] == 'p') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                /* Get three values */
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->lights[light_index].pos.x = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->lights[light_index].pos.y = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->lights[light_index].pos.z = strtod(subtoken, (char **) NULL);
                                        }
                                        /* Colour */
                                        if (line[0] == '\t' && line[1] == 'c') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                /* Get three values */
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->lights[light_index].intensity.red = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->lights[light_index].intensity.green = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->lights[light_index].intensity.blue = strtod(subtoken, (char **) NULL);
                                        }
                                }
                                light_index++;
                        }
                        /* Triangles */
                        if (strcmp(line,"Triangle{\n") == 0) {
                                while (line[0]!= '}') {
                                        error = fgets(line, sizeof(line), fp);
                                        if (!error)
                                                printf("Error reading Triangle line from scene file. \n");
                                        /* Corner 1 */
                                        if (line[0] == '\t' && line[1] == 'c' && line[7] == '1') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                /* Get three values */
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->triangles[triangle_index].v1.x = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->triangles[triangle_index].v1.y = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->triangles[triangle_index].v1.z = strtod(subtoken, (char **) NULL);
                                        }
                                        /* Corner 2 */
                                        if (line[0] == '\t' && line[1] == 'c' && line[7] == '2') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                /* Get three values */
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->triangles[triangle_index].v2.x = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->triangles[triangle_index].v2.y = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->triangles[triangle_index].v2.z = strtod(subtoken, (char **) NULL);
                                        }
                                        /* Corner 3 */
                                        if (line[0] == '\t' && line[1] == 'c' && line[7] == '3') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                /* Get three values */
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->triangles[triangle_index].v3.x = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->triangles[triangle_index].v3.y = strtod(subtoken, (char **) NULL);
                                                saveptr1 = saveptr2;
                                                subtoken = strtok_r(saveptr1,delim_2,&saveptr2);
                                                myScene->triangles[triangle_index].v3.z = strtod(subtoken, (char **) NULL);
                                        }
                                        /* Material */
                                        if (line[0] == '\t' && line[1] == 'm') {
                                                token = strtok_r(line,delim_1,&saveptr1);
                                                if (token == NULL)
                                                        break;
                                                myScene->triangles[triangle_index].material = strtol(saveptr1, (char **) NULL, 10);
                                        }
                                }
                                triangle_index++;
                        }

                }/* while fgets... */
                fclose(fp);
        } else {
                printf("File not found. \n");
                return -1;
        }
        return 0;
}
