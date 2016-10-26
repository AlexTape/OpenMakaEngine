#pragma once
#ifndef OPENMAKAENGINE_GEOMETRY_CPP
#define OPENMAKAENGINE_GEOMETRY_CPP


#include <vector>
#include <iostream>

#include "../Controller.h"
#include "Geometry.h"

using namespace std;
using namespace om;

Geometry::Geometry(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Creating Geometry instance.." << endl;
    }
}

Geometry::~Geometry(void) {
    if (Controller::MODE_DEBUG) {
	    cout << "Deleting Geometry instance.." << endl;
    }
}

bool Geometry::isRectangle(vector<cv::Point2f> &rectanglePoints) {

    // check the validity of transformed rectangle shape
    // the sign of outer products of each edge vector must be the same
    bool returnThis = true;

    if (rectanglePoints.size() == 4) {

        float vector[4][2];
        int i;

        vector[0][0] = rectanglePoints[1].x - rectanglePoints[0].x;
        vector[0][1] = rectanglePoints[1].y - rectanglePoints[0].y;
        vector[1][0] = rectanglePoints[2].x - rectanglePoints[1].x;
        vector[1][1] = rectanglePoints[2].y - rectanglePoints[1].y;
        vector[2][0] = rectanglePoints[3].x - rectanglePoints[2].x;
        vector[2][1] = rectanglePoints[3].y - rectanglePoints[2].y;
        vector[3][0] = rectanglePoints[0].x - rectanglePoints[3].x;
        vector[3][1] = rectanglePoints[0].y - rectanglePoints[3].y;

        int multiplicator;
        float product = vector[3][0] * vector[0][1] - vector[3][1] * vector[0][0];
        if (product > 0) {
            multiplicator = 1;
        } else {
            multiplicator = -1;
        }

        for (i = 0; i < 3; i++) {
            product = vector[i][0] * vector[i + 1][1] - vector[i][1] * vector[i + 1][0];
            if (product * multiplicator <= 0) {
                returnThis = false;
                break;
            }
        }

    } else {
        returnThis = false;
    }

    return returnThis;
}

vector<cv::Point2f> Geometry::rescale(vector<cv::Point2f>& points2d)
{
	// create bucket
	vector<cv::Point2f> coords = vector<cv::Point2f>(points2d.size());

	// for all points
	for (size_t i = 0; i < points2d.size(); i++)
	{
        // rescale point a coordinates
        cv::Point2f a;
        a.x = static_cast<int>(points2d[i].x * SceneFrame::IMAGE_SCALE + 0.5);
        a.y = static_cast<int>(points2d[i].y * SceneFrame::IMAGE_SCALE + 0.5);

		// save
		coords[i] = a;
	}
	// return rescaled coords
	return coords;
}

CvRect Geometry::fitRectangle(vector<cv::Point2f> rect, RECTANGLE_MODE mode)
{
	// Rect as big as possible
	if (mode == MAX) 
	{
		if (rect[0].y != rect[3].y)
		{
			rect[0].y = MIN(rect[0].y, rect[3].y);
			rect[3].y = rect[0].y;
		}
		if (rect[1].y != rect[2].y)
		{
			rect[1].y = MAX(rect[1].y , rect[2].y);
			rect[2].y = rect[1].y;
		}
		if (rect[0].x != rect[1].x)
		{
			rect[0].x = MIN(rect[0].x , rect[1].x);
			rect[1].x = rect[0].x;
		}
		if (rect[2].x != rect[3].x)
		{
			rect[2].x = MAX(rect[2].x ,rect[3].x);
			rect[3].x = rect[2].x;
		}
	}
	//Rect middle 
	if (mode == FIT) 
	{
		if (rect[0].y != rect[3].y)
		{
			rect[0].y = (rect[0].y + rect[3].y) / 2;
			rect[3].y = rect[0].y;
		}
		if (rect[1].y != rect[2].y)
		{
			rect[1].y = (rect[1].y + rect[2].y) / 2;
			rect[2].y = rect[1].y;
		}
		if (rect[0].x != rect[1].x)
		{
			rect[0].x = (rect[0].x + rect[1].x) / 2;
			rect[1].x = rect[0].x;
		}
		if (rect[2].x != rect[3].x)
		{
			rect[2].x = (rect[2].x + rect[3].x) / 2;
			rect[3].x = rect[2].x;
		}
	}
	//Rect as smaller as possible
	if (mode == MIN) 
	{
		if (rect[0].y != rect[3].y)
		{
			rect[0].y = MAX(rect[0].y,rect[3].y);
			rect[3].y = rect[0].y;
		}
		if (rect[1].y != rect[2].y)
		{
			rect[1].y = MIN(rect[1].y , rect[2].y);
			rect[2].y = rect[1].y;
		}
		if (rect[0].x != rect[1].x)
		{
			rect[0].x = MAX(rect[0].x , rect[1].x);
			rect[1].x = rect[0].x;
		}
		if (rect[2].x != rect[3].x)
		{
			rect[2].x = MIN(rect[2].x ,rect[3].x);
			rect[3].x = rect[2].x;
		}
	}
	return cvRect(rect[0].x, rect[0].y, rect[3].x - rect[0].x, rect[1].y - rect[0].y);
}

cv::Mat Geometry::cutRoi(cv::Mat& image, CvRect rect)
{
	cv::Mat cuttedImage = image.clone();
	return cuttedImage(rect) = cv::Scalar(255, 255, 255);
}

cv::Mat Geometry::getRoi(cv::Mat& image, CvRect rect)
{
	cv::Mat cuttedImage = image.clone();
	return cuttedImage(rect);
}

#endif
