//============================================================================
// Name        : OpenCVCameraDemo.cpp
// Author      : W. Schilling
// Version     :
// Copyright   :
// Description : This demo will show how openCV can be used to capture an image as well as how OPENCV can do basic processing on an image.
// Note: Before this demo can be run, the camera must be connected and the OS kernel module must be loaded using
// sudo modprobe bcm2835-v4l2.
// Also, the camera must be anabled in the raspberry pi firmware.
//
//============================================================================

#include<iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// This definition will set the frame rate for the device.  The frame rate is how many images are captured by the camera each second.
#define FRAME_RATE (30)

int main(int argc, char* argv[])
{
	// These variables will keep track of the camera height and width.  This is the raw resolution that is to be captured by the camera.
	int cameraw;
	int camerah;


	if (argc!=3)
	{
		printf("Usage: %s <camera width> <camera height>\n", argv[0]);
		exit(0);
	}

	cameraw = atoi(argv[1]); // Note: Typical camera width might be 1920
	camerah = atoi(argv[2]); // Typical camera height might be 1080.


	// The following code is typically executed as part of an initialization script.
	// Open up a video capture device using camera 0.  The video capture sets up the
    VideoCapture *capture = new VideoCapture(0);
    capture->set(CV_CAP_PROP_FRAME_WIDTH,cameraw);
    capture->set(CV_CAP_PROP_FRAME_HEIGHT,camerah);
    capture->set(CV_CAP_PROP_FPS, FRAME_RATE);

    // This line will make sure we are able to connect to the camera.  If we are not, then the program can not continue running.
    if(!capture->isOpened()){
	    cout << "Failed to connect to the camera." << endl;
	    exit(-1);
    }


    int index;
    // Declare an openCV matrix which will keep the image.
    Mat *image = new Mat();

    // The following code is typically in a loop such that the camera is always grabbing the last frame.  In our case, we are
    // simply simulating this continuous execution that usually is on a thread.
    for (index = 0; index < 10; index++)
    {
    	// Obtain the first frame from the picture and transfer it from the camera hardware into a temp buffer.
    	capture->grab();

    	// Now transfer the image from the camera buffer in memory into a matrix class.
    	capture->retrieve(*image);
    }

    // The image itself is in the matrix image.  If we want to make a copy of it, we can do so with the following code.
    Mat *imageCopy = new Mat();
    image->copyTo(*imageCopy);

    // Now that we have a copy of the image, we can do various things with the image.
    // Lets resize it to a size we want.  By default, we are going to use half of the camera's initial resolution, but these numbers can be anything.
    Mat * resizedImage = new Mat();
    // This class keeps track of the new size of the image.  The /2 is just for example.  This can be anything.
    Size sz(cameraw/2, camerah/2);
    resize(*imageCopy, *resizedImage, sz);

    // Now we are going to do a bit of image processing here.
    // Start by making a grayscale version of the image.
    Mat gray, edges;
    cvtColor(*resizedImage, gray, CV_BGR2GRAY);

    // Now detect the edges.
    Canny(gray, edges, 0, 30, 3);

    // Now write out the images.  This may or may not be what you ultimately do with the images.
    // You may just want to get at parts of the raw data, such as image->data[offset], whereby offset is the 3*width*y + x for the given x,y pixel.
    imwrite("camera.png", *imageCopy);
    imwrite("resized.png", *resizedImage);
    imwrite("gray.png", gray);
    imwrite("edges.png", edges);

    // Now clean up everything.
    delete resizedImage;
    delete imageCopy;
    delete image;
    delete capture;


    return 0;

}
