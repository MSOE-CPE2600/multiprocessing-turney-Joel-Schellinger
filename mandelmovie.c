/// 
//  mandel.c
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
///
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "jpegrw.h"
#include <string.h>

#define FILE_NAME_LEN 13 // "mandel##.jpg" 12 chars +1 for \0
#define NUM_IMGS 50
// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void compute_image( imgRawImage *img, double xmin, double xmax,
									double ymin, double ymax, int max );
static void show_help();


int main( int argc, char *argv[] )
{
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.
	double xcenter = -0.6702094071878258;
	double ycenter =  0.4580605576199168;
	double zoom_factor = 0.9;
	double xscale = 10;
	double yscale = 0; // calc later
	int    image_width = 1000;
	int    image_height = 1000;
	int    max = 1000;
	int n_procs;
	int n_imgs;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"p"))!=-1) {
		switch(c) 
		{
			case 'p':
				n_procs = atoi(optarg);
				break;
		}
	}

	// Calculate y scale based on x scale (settable) and image sizes in X and Y (settable)

	for (int i = 0; i < NUM_IMGS; i++) {
		char outfile[FILE_NAME_LEN] = "";
		snprintf(outfile, FILE_NAME_LEN, "mandel%02d.jpg", i); // Store i in a str
		// Create a raw image of the appropriate size.
		xscale *= zoom_factor;
		yscale = xscale / image_width * image_height;
		imgRawImage* img = initRawImage(image_width,image_height);

		// Fill it with a black
		setImageCOLOR(img,0);

		// Compute the Mandelbrot image
		compute_image(img,xcenter-xscale/2,xcenter+xscale/2,ycenter-yscale/2,ycenter+yscale/2,max);
		// Save the image in the stated file.
		storeJpegImageFile(img, outfile);
		// free the mallocs
		freeRawImage(img);
	}

	return 0;
}




/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iter;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max )
{
	int i,j;

	int width = img->width;
	int height = img->height;

	// For every pixel in the image...

	for(j=0;j<height;j++) {

		for(i=0;i<width;i++) {

			// Determine the point in x,y space for that pixel.
			double x = xmin + i*(xmax-xmin)/width;
			double y = ymin + j*(ymax-ymin)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,max);

			// Set the pixel in the bitmap.
			setPixelCOLOR(img,i,j,iteration_to_color(iters,max));
		}
	}
}


/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}


// Show help message
void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=1000)\n");
	printf("-H <pixels> Height of the image in pixels. (default=1000)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}
