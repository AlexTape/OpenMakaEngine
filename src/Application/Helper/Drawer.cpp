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

void Drawer::drawContour(cv::Mat &image, std::vector<cv::Point2f> points2d, cv::Scalar color, int thickness,
                         int lineType, int shift) {

    // for all points
    for (size_t i = 0; i < points2d.size(); i++) {

        // rescale point a coordinates
        cv::Point2f a;
        a.x = (int) ((points2d[i].x * SceneFrame::IMAGE_SCALE) + 0.5);
        a.y = (int) ((points2d[i].y * SceneFrame::IMAGE_SCALE) + 0.5);

        // resale point b coordinates
        cv::Point2f b;
        b.x = (int) ((points2d[(i + 1) % points2d.size()].x * SceneFrame::IMAGE_SCALE) + 0.5);
        b.y = (int) ((points2d[(i + 1) % points2d.size()].y * SceneFrame::IMAGE_SCALE) + 0.5);

        // draw line
        cv::line(image, a, b, color, thickness, lineType, shift);
    }
}

void Drawer::drawKeypoints(cv::Mat &image, std::vector<cv::KeyPoint> keyPoints, cv::Scalar color) {

    // for all keypoints
    for (unsigned int i = 0; i < keyPoints.size(); i++) {

        // rescale coordinates
        int x = (int) ((keyPoints[i].pt.x * SceneFrame::IMAGE_SCALE) + 0.5);
        int y = (int) ((keyPoints[i].pt.y * SceneFrame::IMAGE_SCALE) + 0.5);

        // draw circles
        cv::circle(image, cv::Point(x, y), 10, cv::Scalar(255, 0, 0, 255));
    }
}

void Drawer::drawKeypointsWithResponse(cv::Mat &image, std::vector<cv::KeyPoint> keyPoints, cv::Scalar color) {

    // for all keypoints
    for (unsigned int i = 0; i < keyPoints.size(); i++) {

        // rescale coordinates
        int x = (int) ((keyPoints[i].pt.x * SceneFrame::IMAGE_SCALE) + 0.5);
        int y = (int) ((keyPoints[i].pt.y * SceneFrame::IMAGE_SCALE) + 0.5);

        // draw circles
        cv::circle(image, cv::Point(x, y), (int) (keyPoints[i].response + 0.5), cv::Scalar(255, 0, 0, 255));
    }
}

cv::Mat Drawer::drawMatchesWindow(cv::Mat query, cv::Mat pattern, const std::vector<cv::KeyPoint> &queryKp,
                                  const std::vector<cv::KeyPoint> &trainKp, std::vector<cv::DMatch> matches,
                                  int maxMatchesDrawn) {
    cv::Mat outImg;

    if (matches.size() > maxMatchesDrawn) {
        matches.resize(maxMatchesDrawn);
    }

    cv::drawMatches
            (
                    query,
                    queryKp,
                    pattern,
                    trainKp,
                    matches,
                    outImg,
                    cv::Scalar(0, 200, 0, 255),
                    cv::Scalar::all(-1),
                    std::vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
            );

    return outImg;
}

cv::Mat Drawer::drawKeypointsWindow(cv::Mat query, cv::Mat pattern, const std::vector<cv::KeyPoint> &queryKp,
                                    const std::vector<cv::KeyPoint> &trainKp, std::vector<cv::DMatch> matches,
                                    int maxMatchesDrawn) {
    cv::Mat outImg;

    if (matches.size() > maxMatchesDrawn) {
        matches.resize(maxMatchesDrawn);
    }

    cv::drawMatches
            (
                    query,
                    queryKp,
                    pattern,
                    trainKp,
                    matches,
                    outImg,
                    cv::Scalar(0, 200, 0, 255),
                    cv::Scalar::all(-1),
                    std::vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
            );

    return outImg;
}

#endif
