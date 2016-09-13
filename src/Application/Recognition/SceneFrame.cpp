#pragma once
#ifndef OPENMAKAENGINE_SCENEFRAME_CPP
#define OPENMAKAENGINE_SCENEFRAME_CPP

#include "SceneFrame.h"
#include "../Controller.h"

#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace om;

int SceneFrame::MAX_IMAGE_SIZE;
float SceneFrame::IMAGE_SCALE;

SceneFrame::SceneFrame(cv::Mat &rgbInputFrame, cv::Mat &grayInputFrame) {

    if (Controller::MODE_DEBUG) {
        cout << "Creating SceneFrame instance.." << endl;
    }

    rgb = rgbInputFrame;
    gray = grayInputFrame;
    objectPosition = vector<cv::Point2f>(4);

    // get initial size of gray mat
    cv::Size graySize = gray.size();

    // get largest image side
    int maxSize = 0;
    if (graySize.width > graySize.height) {
        maxSize = graySize.width;
    } else {
        maxSize = graySize.height;
    }

    // set initial scale factor
    IMAGE_SCALE = 0.1;

    // calc scale factor via max image size
    while ((maxSize / IMAGE_SCALE) >= MAX_IMAGE_SIZE) {
        IMAGE_SCALE = IMAGE_SCALE + (float) 0.1;
    }

    // calc calculative width/height
    float calcWidth = graySize.width / IMAGE_SCALE;
    float calcHeight = graySize.height / IMAGE_SCALE;

    // round to concrete width/height
    int height = (int) (calcHeight + 0.5);
    int width = (int) (calcWidth + 0.5);

    // create image holder
    cv::Mat holder;
    holder.create(height, width, CV_8UC1);

    // resize gray image
    try {
        cv::resize(gray, holder, holder.size());
    } catch (cv::Exception &exception) {
        cvError(0, "SceneFrame", "Resizing failed!", __FILE__, __LINE__);
        cout << exception.what() << endl;
    }
    gray = holder;

}

string SceneFrame::getInputResolution() {
    ostringstream ss;
    ss << rgb.cols << "x" << rgb.rows;
    return ss.str();
}

string SceneFrame::getProcessingResolution() {
    ostringstream ss;
    ss << gray.cols << "x" << gray.rows;
    return ss.str();
}

SceneFrame::~SceneFrame(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting SceneFrame instance.." << endl;
    }
}

#endif
