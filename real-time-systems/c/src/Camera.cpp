#include "Camera.h"
#include <chrono>

#define FPS (30)

/**
 * Construct a new instance of the camera class.
 * @param width This is the width of the natively captured images. This is the resolution the camera generates.
 * @param height This is the height of the natively captured images. This is the resolution the camera generates.
 * @param threadName This is the name of the thread that is to be used to run the image capture.
 */
Camera::Camera(int width, int height, std::string threadName, uint32_t period) :
		PeriodicTask(threadName, period) {

	/**
	 * 1.0 Start by instantiating a VideoCapture object which will grab the images from the camera.
	 */
	capture = new VideoCapture(0);

	/**
	 * 2.0 Next, set the width, height, and frames per second parameters of the capture.
	 */
	capture->set(CV_CAP_PROP_FRAME_WIDTH, width);
	capture->set(CV_CAP_PROP_FRAME_HEIGHT, height);
	capture->set(CV_CAP_PROP_FPS, FPS);

	/**
	 * 3.0 Instantiate a new matrix that will hold the last frame.
	 */
	lastFrame = new Mat();

	/**
	 * 4.0 Check to see that the capture is opened.  If if isn't, print out a failure method and exit the program with an error code.
	 */
	if (!capture->isOpened()) {
		cout << "Failed to connect to the camera." << endl;
		exit(-1);
	}
}

/**
 * This is the destructor for the camera. It will delete all dynamically allocated objects.
 */
Camera::~Camera() {
	/**
	 * 1.0 Release the camera.
	 */
	capture->release();

	/**
	 * 2.0 Delete all allocated objects.
	 */
	delete lastFrame;
	delete capture;
}

/**
 * This is the task method for the camera. It will do the capture images from the camera hardware, keeping the image that is available current.
 */
void Camera::taskMethod() {
	/**
	 * 1.0 Lock the mutex that protects the last frame.
	 */

	mtx.lock();

	/**
	 * 2.0 Read the next image in.
	 */
	capture->grab();
	capture->retrieve(*lastFrame);

	/**
	 * 3.0 Unlock the mutex.
	 */
	mtx.unlock();

}

/**
 * This method will return the next picture from the camera, following the algorithms described here:
 * @return The return will be a matrix of the picture that was last grabbed from the camera.  If the last frame is empty, the return will be an empty matrix.
 */
Mat Camera::takePicture() {
	/**
	 * 1.0 Create an empty matrix object.
	 */
	Mat newMat = Mat();
	/**
	 * 2.0 If the last frame is not empty.
	 */
	if (!lastFrame->empty()) {
		/**
		 * 2.1 Lock the mutex protecting the last frame.
		 */
		mtx.lock();

		/**
		 * 2.2 Copy the last frame to the return value.
		 */
		lastFrame->copyTo(newMat);

		/**
		 * 2.3 Unlock the mutex.
		 */
		mtx.unlock();
	}
	/**
	 * 3.0 Return the matrix object.
	 */
	return newMat;
}

