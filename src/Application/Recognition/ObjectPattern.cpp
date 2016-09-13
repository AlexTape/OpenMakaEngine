#pragma once
#ifndef OPENMAKAENGINE_OBJECTPATTERN_CPP
#define OPENMAKAENGINE_OBJECTPATTERN_CPP

#include <iostream>
#include "ObjectPattern.h"
#include "../Controller.h"

using namespace std;
using namespace om;

ObjectPattern::ObjectPattern(const cv::Mat &grayImage) {

    if (Controller::MODE_DEBUG) {
        cout << "Creating ObjectPattern instance.." << endl;
    }

    image = grayImage;

    points2d = std::vector<cv::Point2f>(4);
    points3d = std::vector<cv::Point3f>(4);

    build();
}

ObjectPattern::~ObjectPattern(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting ObjectPattern instance.." << endl;
    }
}

void ObjectPattern::build() {

    // set size
    size = cv::Size(image.cols, image.rows);

    // set normalized dimensions
    float maximumSize = max(size.width, size.height);
    float widthUnit = size.width / maximumSize;
    float heightUnit = size.height / maximumSize;

    // set points2d
    points2d[0] = cv::Point2f(0, 0);
    points2d[1] = cv::Point2f(size.width, 0);
    points2d[2] = cv::Point2f(size.width, size.height);
    points2d[3] = cv::Point2f(0, size.height);

    // set points3d
    points3d[0] = cv::Point3f(-widthUnit, -heightUnit, 0);
    points3d[1] = cv::Point3f(widthUnit, -heightUnit, 0);
    points3d[2] = cv::Point3f(widthUnit, heightUnit, 0);
    points3d[3] = cv::Point3f(-widthUnit, heightUnit, 0);
}

#endif
