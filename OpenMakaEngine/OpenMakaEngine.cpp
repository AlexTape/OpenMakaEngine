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

int _tmain(int argc, _TCHAR* argv[])
{
	//namedWindow("Main", CV_WINDOW_KEEPRATIO); //resizable window;

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
