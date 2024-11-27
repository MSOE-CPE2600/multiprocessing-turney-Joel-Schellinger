/**
 * @file mandelmovie.c
 * @brief Creates a movie of zooming in on 
 * a point in the mandelbrot series
 * Compile Instructions: make
 * Course: CPE2600
 * Section: 111
 * Assignment: Vector operations
 * @author Joel Schellinger
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "jpegrw.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <stdbool.h>

// Macros
#define FILE_NAME_LEN 14 // "mandel###.jpg" 12 chars +1 for \0

// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void compute_image(imgRawImage *img, double xmin, double xmax,
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
	double initial_scale = 10;
	int n_procs = 1;
	int n_imgs = 50;
	bool build_img = false;

	// The following should not be changed
	const int image_width = 1000;
	const int image_height = 1000;
	const int max = 1000;

	// For each command line argument given,
	// override the appropriate configuration value.
	while((c = getopt(argc,argv,"h b x:y:z:p:i:"))!=-1) {
		switch(c) 
		{
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 'z':
				zoom_factor = atof(optarg);
				break;
			case 'p':
				n_procs = atoi(optarg);
				break;
			case 'h':
				show_help();
				exit(EXIT_SUCCESS);
				break;
			case 'b':
				build_img = true;
				break;
			case 'i':
				n_imgs = atoi(optarg);
				break;
		}
	}

	// Create the number of processes that the user asked for
	for(int i = 0; i < n_procs; i++) {
		pid_t pid = fork();
		if (pid < 0) {
			perror("fork failed");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {

			// Start at the process number
			int start = i;

			// Ends when there are no more images to process
            int end = n_imgs;
			
			// Increment by the number of processes
			for (int j = start; j < end; j += n_procs) {
				// Provide some form of progress for the user
				printf("Processing img %d in process %d\n", j, i);

				// Make the output file name
				char outfile[FILE_NAME_LEN] = "";
				snprintf(outfile, FILE_NAME_LEN, "mandel%03d.jpg", j); // Store i in a str

				// The scale gets scaled by the zoom factor every iteration
				// The initial scale is multiplied by zoom factor j times
				// More simply zoom_factor to the power of j times initial scale
				double xscale = initial_scale*pow(zoom_factor, j);
				double yscale = xscale / image_width * image_height;
				imgRawImage* img = initRawImage(image_width,image_height);

				// Fill it with a black image
				setImageCOLOR(img,0);

				// Compute the Mandelbrot image
				compute_image(img,xcenter-xscale/2,xcenter+xscale/2,ycenter-yscale/2,ycenter+yscale/2,max);
				
				// Save the image in the stated file.
				storeJpegImageFile(img, outfile);
				
				// free the mallocs
				freeRawImage(img);
			}
			exit(0);
		}
	}

	// Parent process waits for all children
    for (int i = 0; i < n_procs; i++) {
        wait(NULL);
    }

	// Exit if not building image
	if (!build_img) { 
		exit(EXIT_SUCCESS);
	}

	// Build the image
	pid_t pid = fork();
	if (pid == 0) {
		// Child process: execute ffmpeg
		execl("/usr/bin/ffmpeg", "ffmpeg", "-i", "mandel%03d.jpg", "mandel.mpg", NULL);
		perror("execl failed");
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		// Parent process: wait for the child to complete
		wait(NULL);
		execl("/usr/bin/ffplay", "ffplay", "mandel.mpg", NULL);
		perror("execl failed");
	} else {
		perror("fork failed");
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
	printf("Use: mandelmovie [options]\n");
	printf("Where options are:\n");
	printf("-x <coord>  	X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  	Y coordinate of image center point. (default=0)\n");
	printf("-z <zoom> 		Zoom factor to scale down each image by (default=0.9)\n");
	printf("-p <processes> 	Set number of processes. (default=1)\n");
	printf("-i <images> 	Set the number of images for the video. (default=50)\n");
	printf("-b			  	Build and show the movie.\n");
	printf("-h          	Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandelmovie -x -0.5 -y -0.5 -z 0.75\n");
	printf("mandelmovie -x -.38 -y -.665 -p 5 -b\n");
	printf("mandelmovie -x 0.286932 -y 0.014287 -z .9 -p 20 -i 200\n\n");
}
