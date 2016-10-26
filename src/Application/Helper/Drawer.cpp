#pragma once
#ifndef OPENMAKAENGINE_DRAWER_CPP
#define OPENMAKAENGINE_DRAWER_CPP

#include "Drawer.h"
#include "../Controller.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace om;

Drawer::Drawer(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Creating Drawer instance.." << endl;
    }
}

Drawer::~Drawer(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting Drawer instance.." << endl;
    }
}

void Drawer::drawContour(cv::Mat& image, vector<cv::Point2f> points2d, cv::Scalar color, int thickness, int lineType, int shift)
{
	// for all points
	for (size_t i = 0; i < points2d.size(); i++)
	{
		// rescale point a coordinates
		cv::Point2f a;
		a.x = static_cast<int>(points2d[i].x + 0.5);
		a.y = static_cast<int>(points2d[i].y + 0.5);

		// rescale point b coordinates
		cv::Point2f b;
		b.x = static_cast<int>(points2d[(i + 1) % points2d.size()].x + 0.5);
		b.y = static_cast<int>(points2d[(i + 1) % points2d.size()].y + 0.5);

		// draw line
		line(image, a, b, color, thickness, lineType, shift);
	}
}

void Drawer::drawContourWithRescale(cv::Mat &image, vector<cv::Point2f> points2d, cv::Scalar color, int thickness,
                         int lineType, int shift) {

    // for all points
    for (size_t i = 0; i < points2d.size(); i++) {

        // rescale point a coordinates
        cv::Point2f a;
        a.x = static_cast<int>(points2d[i].x * SceneFrame::IMAGE_SCALE + 0.5);
        a.y = static_cast<int>(points2d[i].y * SceneFrame::IMAGE_SCALE + 0.5);

        // resale point b coordinates
        cv::Point2f b;
        b.x = static_cast<int>(points2d[(i + 1) % points2d.size()].x * SceneFrame::IMAGE_SCALE + 0.5);
        b.y = static_cast<int>(points2d[(i + 1) % points2d.size()].y * SceneFrame::IMAGE_SCALE + 0.5);

        // draw line
        line(image, a, b, color, thickness, lineType, shift);
    }
}

void Drawer::drawKeypoints(cv::Mat &image, vector<cv::KeyPoint> keyPoints, cv::Scalar color) {

    // for all keypoints
    for (unsigned int i = 0; i < keyPoints.size(); i++) {

        // rescale coordinates
        int x = static_cast<int>((keyPoints[i].pt.x * SceneFrame::IMAGE_SCALE) + 0.5);
        int y = static_cast<int>((keyPoints[i].pt.y * SceneFrame::IMAGE_SCALE) + 0.5);

        // draw circles
        cv::circle(image, cv::Point(x, y), 10, cv::Scalar(255, 0, 0, 255));
    }
}

void Drawer::drawKeypointsWithResponse(cv::Mat &image, vector<cv::KeyPoint> keyPoints, cv::Scalar color) {

    // for all keypoints
    for (unsigned int i = 0; i < keyPoints.size(); i++) {

        // rescale coordinates
	    auto x = static_cast<int>((keyPoints[i].pt.x * SceneFrame::IMAGE_SCALE) + 0.5);
	    auto y = static_cast<int>((keyPoints[i].pt.y * SceneFrame::IMAGE_SCALE) + 0.5);

        // draw circles
        circle(image, cv::Point(x, y), static_cast<int>(keyPoints[i].response + 0.5), cv::Scalar(255, 0, 0, 255));
    }
}

cv::Mat Drawer::drawMatchesWindow(cv::Mat query, cv::Mat pattern, const vector<cv::KeyPoint> &queryKp,
                                  const vector<cv::KeyPoint> &trainKp, vector<cv::DMatch> matches,
                                  int maxMatchesDrawn) {
    cv::Mat outImg;

    if (matches.size() > maxMatchesDrawn) {
        matches.resize(maxMatchesDrawn);
    }

    drawMatches
            (
                    query,
                    queryKp,
                    pattern,
                    trainKp,
                    matches,
                    outImg,
                    cv::Scalar(0, 200, 0, 255),
                    cv::Scalar::all(-1),
                    vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
            );

    return outImg;
}

cv::Mat Drawer::drawKeypointsWindow(cv::Mat query, cv::Mat pattern, const vector<cv::KeyPoint> &queryKp,
                                    const vector<cv::KeyPoint> &trainKp, vector<cv::DMatch> matches,
                                    int maxMatchesDrawn) {
    cv::Mat outImg;

    if (matches.size() > maxMatchesDrawn) {
        matches.resize(maxMatchesDrawn);
    }

    drawMatches
            (
                    query,
                    queryKp,
                    pattern,
                    trainKp,
                    matches,
                    outImg,
                    cv::Scalar(0, 200, 0, 255),
                    cv::Scalar::all(-1),
                    vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
            );

    return outImg;
}

#endif
