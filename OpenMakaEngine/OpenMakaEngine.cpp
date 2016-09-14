// OpenMakaEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "../src/Application/Controller.h"

//#include "Application/Controller.h"

using namespace cv;
using namespace std;
using namespace om;

static string path = "C:\\Users\\tebbje\\workspace\\OpenMakaEngine";

int compareImages()
{
		Mat sceneRgbImage, sceneGrayImage;

	sceneRgbImage = cv::imread(path + "\\images\\card_frame.bmp");
	if (sceneRgbImage.empty())
	{
		std::cout << "Scene image cannot be read" << std::endl;
		return 1;
	}

	cvtColor(sceneRgbImage, sceneGrayImage, CV_RGB2GRAY);

	Controller* controller = Controller::getInstance();

	if (!controller->isInitialized)
	{
		controller->initialize(sceneRgbImage, path);
	}

	controller->isModeObjectDetection(true);

	// call display function with frame data
	bool shouldQuit = false;
	do
	{
		// display single window with one analyzer configuration
		controller->displayFunction(sceneRgbImage, sceneGrayImage);

		// Read the keyboard input:
		int keyCode = cv::waitKey(5);
		if (keyCode == 27 || keyCode == 'q')
		{
			shouldQuit = true;
		}
	}
	while (!shouldQuit);
	return 0;
}

int processTests()
{
		
    Controller *controller = Controller::getInstance();
    Controller::STORAGE_PATH = path;

    // do testcase 0 (all) 1 times
    controller->test(0, 1);

    // idle to view images
    //if (Controller::MODE_USE_WINDOWS) {
        bool shouldQuit = false;
        do {
            // Read the keyboard input:
            int keyCode = cv::waitKey(5);
            if (keyCode == 27 || keyCode == 'q') {
                shouldQuit = true;
            }
        } while (!shouldQuit);
   // }

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	processTests();

	return 0;
}


