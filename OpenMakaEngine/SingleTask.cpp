// OpenMakaEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include "../src/Application/Controller.h"
#include "../src/Application/Recognition/Analyzer.h"
#include "../src/Application/Helper/Drawer.h"

using namespace cv;
using namespace std;
using namespace om;

static string path = "C:\\Users\\tebbje\\workspace\\OpenMakaEngine";

int compareImages()
{
	// input image
	Mat inputImage, inputImageGray;

	inputImage = imread(path + "\\images\\card_frame.bmp");
	if (inputImage.empty())
	{
		cout << "Scene image cannot be read" << endl;
		return 1;
	}

	// make gray
	cvtColor(inputImage, inputImageGray, CV_RGB2GRAY);

	// set maximum size of processing image
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
	cv::Mat objectImage = cv::imread(path + "\\images\\1card.png",
	                                 CV_LOAD_IMAGE_GRAYSCALE);
	analyzer->createObjectPattern(objectImage);

	// recreate object pattern if it is not existing
	analyzer->missingObjectPattern();
	
	// create new scene frame
	SceneFrame* sceneFrame = new SceneFrame(inputImage, inputImageGray);

	// analyzer processing
	bool objectFound = analyzer->process(*sceneFrame);
	
	// drawing green contours
	Drawer::drawContour(inputImage, sceneFrame->objectPosition, cv::Scalar(0, 255, 0));

	// log to console
	cout << "Found: " << objectFound << endl;
	cout << sceneFrame->objectPosition.at(0).x << ":" << sceneFrame->objectPosition.at(0).y << endl;
	cout << sceneFrame->objectPosition.at(1).x << ":" << sceneFrame->objectPosition.at(1).y << endl;
	cout << sceneFrame->objectPosition.at(2).x << ":" << sceneFrame->objectPosition.at(2).y << endl;
	cout << sceneFrame->objectPosition.at(3).x << ":" << sceneFrame->objectPosition.at(3).y << endl;
	
	// display image in window
	cvNamedWindow("Name", CV_WINDOW_NORMAL);
	imshow("Name", inputImage);
	cvWaitKey(0);

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	compareImages();

	return 0;
}
