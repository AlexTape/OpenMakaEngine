// OpenMakaEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include "../src/Application/Controller.h"
#include "../src/Application/Recognition/Analyzer.h"
#include "../src/Application/Helper/Drawer.h"
#include "../src/Application/Helper/Geometry.h"

using namespace std;
using namespace om;

int work() {
	cv::namedWindow("Main", CV_WINDOW_KEEPRATIO); //resizable window;

	cv::Mat sceneRgbImage, sceneGrayImage;

	sceneRgbImage = cv::imread("C:\\Users\\tebbje\\workspace\\IMAGES\\B_10_0004564.png");
    if (sceneRgbImage.empty()) {
        std::cout << "Scene image cannot be read" << std::endl;
        return 1;
    }

	cv::cvtColor(sceneRgbImage, sceneGrayImage, CV_RGB2GRAY);

    Controller *controller = Controller::getInstance();

    if (!controller->isInitialized) {
        controller->initialize(sceneRgbImage, "C:\\Users\\tebbje\\workspace\\OpenMakaEngine");
    }

    controller->isModeObjectDetection(true);

    // call display function with frame data
    bool shouldQuit = false;
    do {

        // display single window with one analyzer configuration
        controller->displayFunction(sceneRgbImage, sceneGrayImage);

        // Read the keyboard input:
        int keyCode = cv::waitKey(5);
        if (keyCode == 27 || keyCode == 'q') {
            shouldQuit = true;
        }
    } while (!shouldQuit);


	return 0;
}

int compareImages()
{
	// input image
	cv::Mat inputImage, inputImageGray;
	cout << "Processing.." << endl;

	inputImage = cv::imread("C:\\Users\\tebbje\\workspace\\IMAGES\\B_10_0004564_s.png");
	if (inputImage.empty())
	{
		cout << "Scene image cannot be read" << endl;
		return 1;
	}

	// make gray
	cvtColor(inputImage, inputImageGray, CV_RGB2GRAY);

	// grab frame size
	Controller::FRAME_SIZE = cv::Size(inputImage.cols, inputImage.rows);
	SceneFrame::MAX_IMAGE_SIZE = static_cast<int>(2000);

	// configure analyzer
	Analyzer::DETECTOR = static_cast<string>("SIFT");
	Analyzer::EXTRACTOR = static_cast<string>("SIFT");
	Analyzer::MATCHER = static_cast<string>("BF");
	Analyzer::MINIMUM_INLIERS = static_cast<int>(5);
	Analyzer::MINIMUM_MATCHES = static_cast<int>(4);
	Analyzer::NN_DISTANCE_RATIO = static_cast<float>(0.6);
	Analyzer::K_GROUPS = static_cast<int>(2);
	Analyzer::RANSAC_REPROJECTION_THRESHOLD = static_cast<double>(3.0);

	// initialize analyzer
	auto analyzer = Analyzer::getInstance();
	analyzer->initialize();

	// add default object
	cv::Mat objectImage = cv::imread("C:\\Users\\tebbje\\workspace\\OpenMakaEngine\\images\\scala_black.png",
	                                 CV_LOAD_IMAGE_GRAYSCALE);
	analyzer->createObjectPattern(objectImage);

	// create new scene frame
	SceneFrame* sceneFrame = new SceneFrame(inputImage, inputImageGray);

	// analyzer processing
	bool objectFound = analyzer->process(*sceneFrame);
	cout << "Found: " << objectFound << endl;

	if (objectFound)
	{
		// drawing green contours
		if (Geometry::isRectangle(sceneFrame->objectPosition))
		{
			vector<cv::Point2f> coords = Geometry::rescale(sceneFrame->objectPosition);
			
			/*cv::Rect bestFit = Geometry::fitRectangle(coords);

			cv::Mat result;
			try
			{
				result = Geometry::getRoi(inputImage, bestFit);
			}
			catch (cv::Exception& e)
			{
				const char* err_msg = e.what();
				cout << "exception caught: " << err_msg << endl;
			}*/

			Drawer::drawContour(inputImage, coords, cv::Scalar(5));

			// show
			cv::namedWindow("RESULT", CV_WINDOW_NORMAL);
			cv::imshow("RESULT", inputImage);
			cvWaitKey(0);
		}
	}

	return 0;
}

int test()
{
	// tell images
	vector<string> images1;
	images1.push_back("\\objects\\book1-1.jpg");
	images1.push_back("\\objects\\book1-2.jpg");
	images1.push_back("\\objects\\book1-3.jpg");
	images1.push_back("\\objects\\book1-4.jpg");
	images1.push_back("\\objects\\book1-5.jpg");
	images1.push_back("\\objects\\book1-6.jpg");

	vector<string> images2;
	images2.push_back("\\objects\\book2-1.jpg");
	images2.push_back("\\objects\\book2-2.jpg");
	images2.push_back("\\objects\\book2-3.jpg");
	images2.push_back("\\objects\\book2-4.jpg");
	images2.push_back("\\objects\\book2-5.jpg");
	images2.push_back("\\objects\\book2-6.jpg");

	vector<string> images3;
	images3.push_back("\\objects\\hardware1-1.jpg");
	images3.push_back("\\objects\\hardware1-2.jpg");
	images3.push_back("\\objects\\hardware1-3.jpg");
	images3.push_back("\\objects\\hardware1-4.jpg");
	images3.push_back("\\objects\\hardware1-5.jpg");
	images3.push_back("\\objects\\hardware1-6.jpg");

	vector<string> images4;
	images4.push_back("\\objects\\hardware2-1.jpg");
	images4.push_back("\\objects\\hardware2-2.jpg");
	images4.push_back("\\objects\\hardware2-3.jpg");
	images4.push_back("\\objects\\hardware2-4.jpg");
	images4.push_back("\\objects\\hardware2-5.jpg");
	images4.push_back("\\objects\\hardware2-6.jpg");

	// get controller and run tests
	Controller* ctrl = Controller::getInstance();
	Controller::STORAGE_PATH = "C:\\Users\\tebbje\\workspace\\OpenMakaEngine";

	// do test
	ctrl->test(images1, 0, 13);
	ctrl->test(images2, 0, 13);
	ctrl->test(images3, 0, 13);
	ctrl->test(images4, 0, 13);

	time_t t = time(0); // get time now
	struct tm* now = localtime(& t);
	cout << (now->tm_year + 1900) << '-'
		<< (now->tm_mon + 1) << '-'
		<< now->tm_mday << " - "
		<< now->tm_hour << ":"
		<< now->tm_min
		<< endl;

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//work();
	//compareImages();
	test();

	// hold window
	cout << "DONE" << endl;
	int wait;
	cin >> wait;

	// return
	return 0;
}
