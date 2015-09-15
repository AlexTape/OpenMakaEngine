/*
 * starter_video.cpp
 *
 *  Created on: Nov 23, 2010
 *      Author: Ethan Rublee
 *
 * A starter sample for using opencv, get a video stream and display the images
 * easy as CV_PI right?
 */
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>

#include "Application/Controller.h"

using namespace cv;
using namespace std;
using namespace om;

int processTests();

int processImagePair();

int processVideo();

static string path = "/home/thinker/ClionProjects/OpenMakaEngine";

int main(int ac, char **av) {

    cv::initModule_nonfree();

//    processVideo();
//    processImagePair();
    processTests();

    return 0;
}

int processImagePair() {

    namedWindow("Main", CV_WINDOW_KEEPRATIO); //resizable window;

    Mat sceneRgbImage, sceneGrayImage;

    sceneRgbImage = cv::imread(path + "/images/card_frame.jpg");
    if (sceneRgbImage.empty()) {
        std::cout << "Scene image cannot be read" << std::endl;
        return 1;
    }

    cvtColor(sceneRgbImage, sceneGrayImage, CV_RGB2GRAY);

    Controller *controller = Controller::getInstance();

    if (!controller->isInitialized) {
        controller->initialize(sceneRgbImage, path);
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

int processTests() {

    Controller *controller = Controller::getInstance();
    Controller::STORAGE_PATH = path;

    // do testcase 0 (all) 51 times
    controller->test(0, 51);

    // idle to view images
    if (Controller::MODE_USE_WINDOWS) {
        bool shouldQuit = false;
        do {
            // Read the keyboard input:
            int keyCode = cv::waitKey(5);
            if (keyCode == 27 || keyCode == 'q') {
                shouldQuit = true;
            }
        } while (!shouldQuit);
    }

}

int processVideo() {

    VideoCapture capture(1);

    if (!capture.isOpened()) {
        cerr << "Failed to open video stream\n" << endl;
        return 1;
    }

    namedWindow("Main", CV_WINDOW_KEEPRATIO); //resizable window;

    Mat rgbFrame, grayFrame;
    capture.read(rgbFrame);

    // Try to read the pattern:
    cv::Mat patternImage, patternImageGray;
    patternImage = cv::imread(path + "/images/card.jpg");
    if (patternImage.empty()) {
        std::cout << "Input image cannot be read" << std::endl;
        return 2;
    }
    cvtColor(patternImage, patternImageGray, CV_RGB2GRAY);

    Controller *controller = Controller::getInstance();

    for (; ;) {
        capture.read(rgbFrame);
        if (rgbFrame.empty()) break;

        if (!controller->isInitialized) {
            controller->initialize(rgbFrame, path);
        }

        cvtColor(rgbFrame, grayFrame, CV_RGB2GRAY);
        if (grayFrame.empty()) {
            std::cout << "Cannot open video capture device" << std::endl;
        }

        // call display function with frame data
        controller->displayFunction(rgbFrame, grayFrame);

        imshow("Main", rgbFrame);

        char key = (char) waitKey(5); //delay N millis, usually long enough to display and capture input
        switch (key) {
            case 'q':
                controller->isModeObjectDetection(true);
                cout << "Recognition started.." << endl;
                break;
            case 'w':
                controller->isModeObjectDetection(false);
                cout << "Recognition stopped!" << endl;
                break;
            case 'e':
                controller->isModeTracking(true);
                cout << "Tracking started.." << endl;
                break;
            case 'r':
                controller->isModeTracking(false);
                cout << "Tracking stopped!" << endl;
                break;
            case 's':
                controller->createObjectPattern(patternImage, patternImageGray);
                cout << "Image registered" << endl;
                break;
            case 'd':
                controller->createObjectPattern(rgbFrame, grayFrame);
                cout << "Frame registered" << endl;
                break;
            case 't':
                cout << "Configure SURF as detector" << endl;
                cout << "RESULT=" << controller->setDetector("SURF");
                break;
            case 'z':
                cout << "Configure SURF as extractor" << endl;
                cout << "RESULT=" << controller->setExtractor("SURF");
                break;
            case 'u':
                cout << "Configure BF as matcher" << endl;
                cout << "RESULT=" << controller->setMatcher("BF");
                break;
            case 27: //escape key
                return 0;
            default:
                break;
        }
    }
}
