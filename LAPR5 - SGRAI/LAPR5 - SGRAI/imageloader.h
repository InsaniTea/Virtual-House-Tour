#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <AL/alut.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>


#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#ifdef _WIN32
#include <GL/glaux.h>
#endif

#ifndef IMAGE_LOADER_H_INCLUDED
#define IMAGE_LOADER_H_INCLUDED

//Represents an image
class Image {
	public:
		Image(char* ps, int w, int h);
		~Image();
		
		/* An array of the form (R1, G1, B1, R2, G2, B2, ...) indicating the
		 * color of each pixel in image.  Color components range from 0 to 255.
		 * The array starts the bottom-left pixel, then moves right to the end
		 * of the row, then moves up to the next column, and so on.  This is the
		 * format in which OpenGL likes images.
		 */
		char* pixels;
		int width;
		int height;
};

//Reads a bitmap image from file.
Image* loadBMP(const char* filename);


//Returns an array indicating pixel data for an RGBA image that is the same as
//image, but with an alpha channel indicated by the grayscale image alphaChannel
char* addAlphaChannel(Image* image, Image* alphaChannel);

//Makes the image into a texture, using the specified grayscale image as an
//alpha channel and returns the id of the texture
GLuint loadAlphaTexture(Image* image, Image* alphaChannel);


#endif
