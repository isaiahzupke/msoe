/*
 * main.cpp
 * This is the main file for the Camera Capture program.
 *      Author: W. Schilling
 */

using namespace std;
#include "ImageTransmitter.h"
#include "Camera.h"
#include "ImageCapturer.h"
#include <chrono>
#include <iostream>
#include "RunnableClass.h"
#include <sys/syscall.h>
#include <unistd.h>


using namespace std;

/**
 * This is the main program.
 */
int main(int argc, char* argv[]) {
	// These are the image sizes for the camera (c) and the transmitted image (t), both height (h) and width (w).
	int cw, ch, tw, th, fps, lpudp;

	if (argc != 9)
	{
		printf("Usage: %s ip port cameraWidth cameraHeight TransmitWidth transmitHeight <frame per second to send> <Lines per UDP Message>", argv[0]);
		exit(0);
	}

	cout << "Main thread id is : " << syscall(SYS_gettid) << "\n";

	// Convert the parameters into integers.
	int port = atoi(argv[2]);
	cw = atoi(argv[3]);
	ch = atoi(argv[4]);
	tw = atoi(argv[5]);
	th = atoi(argv[6]);
	fps = atoi(argv[7]);
	lpudp = atoi(argv[8]);


	// Instantiate a camera.
	Camera* myCamera = new Camera(cw, ch, "Camera", 1000000/30);

	// Figure out the port to use.
	ImageTransmitter* it = new ImageTransmitter(argv[1], port, lpudp);
	myCamera->start(10);

	// Start capturing and streaming.
	ImageCapturer *is = new ImageCapturer(myCamera, it, tw, th, "Image Stream", (1000000/fps));
	is->start();

	string msg;
	cin >> msg;

	while (msg.compare("QUIT")!=0)
	{
		cout << "Looping thread id is : " << syscall(SYS_gettid) << "\n";
		if (msg.compare("P")==0)
		{
			RunnableClass::printThreads();
		}
		else if (msg.compare("R")==0)
		{
			RunnableClass::resetAllThreadInformation();
		}

		cin >> msg;
	}

	is->stop();
	is->waitForShutdown();

	myCamera->stop();
	myCamera->waitForShutdown();
	
	delete myCamera;
	delete it;
	delete is;
}
