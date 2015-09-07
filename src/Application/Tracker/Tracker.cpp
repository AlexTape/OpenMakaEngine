#ifndef OPENMAKAENGINE_TRACKER_CPP
#define OPENMAKAENGINE_TRACKER_CPP

//! Big Thanks to Takuya Minagawa
//! source @https://github.com/takmin/OpenCV-Marker-less-AR
//! forked @https://github.com/AlexTape/OpenCV-Marker-less-AR

#include "Tracker.h"
#include "../Controller.h"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/video/tracking.hpp>

using namespace std;
using namespace om;

// init static members
Tracker *Tracker::inst_ = NULL;
int Tracker::MAX_CORNERS;
double Tracker::QUALITY_LEVEL;
double Tracker::MINIMUM_DISTANCE;

Tracker *Tracker::getInstance() {
    if (inst_ == NULL) {
        inst_ = new Tracker();
    }
    return inst_;
}

Tracker::Tracker(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Creating Tracker instance.." << endl;
    }
}

Tracker::~Tracker(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting Tracker instance.." << endl;
    }
}

void Tracker::initialize(const cv::Mat &frame, vector<cv::Point2f> &actualObjectPosition) {

    cv::Mat mask = createAreaMask(frame.size(), actualObjectPosition);

    goodFeaturesToTrack(frame, corners, MAX_CORNERS, QUALITY_LEVEL,
                        MINIMUM_DISTANCE, mask);

    frame.copyTo(lastImage);
    objectPosition = actualObjectPosition;

    float d[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    homography = cv::Mat(3, 3, CV_32FC1, d).clone();
    vstatus.clear();
}

bool Tracker::process(const cv::Mat &sceneImage) {

    std::vector<cv::Point2f> nextCorners;
    std::vector<float> verror;

    if (corners.size() == 0) {
        if (Controller::MODE_STATISTICS) {
            Controller::statistics("TrackerCorners", (int) corners.size());
        }
        if (Controller::MODE_DEBUG) {
            cout << "TrackerCorners=" << corners.size() << endl;
        }
        return false;
    }

    // vstatus gives the total number of found features
    // verror will contian known errors
    cv::calcOpticalFlowPyrLK(lastImage, sceneImage, corners, nextCorners, vstatus,
                             verror);


    int validFeatures = 0;
    vector<unsigned char>::iterator status_itr = vstatus.begin();
    while (status_itr != vstatus.end()) {
        if (*status_itr > 0) validFeatures++;
        status_itr++;
    }

//    vector<cv::Point2f> rightPoints;
//    vector<int> rightPointsToFindBackIndex;
//
//    for (unsigned int i = 0; i<vstatus.size(); i++) {
//        float test = verror[i];
//        if (vstatus[i] && verror[i] > 0) {
//            // keep original point index in optical flow array for further use
//            rightPointsToFindBackIndex.push_back(i);
//            rightPoints.push_back(nextCorners[i]);
//        } else {
//            vstatus[i] = 0;
//        }
//    }
//
//    int a = rightPoints.size();
//    int b = validFeatures;


//    if (rightPoints.size() < 6) {
//        return false;

    if (validFeatures < 6) {
        return false;

    } else {

        homography = findHomography(cv::Mat(corners), cv::Mat(nextCorners),
                                    vstatus, CV_RANSAC, 5);

        if (countNonZero(homography) == 0) {

            return false;

        } else {

            vector<cv::Point2f> nextObjectPosition = calcAffineTransformation(
                    objectPosition, homography);

            if (!isObjectInsideImage(lastImage.size(), nextObjectPosition)) {
                return false;
            }

            if (!isRectangle(nextObjectPosition)) {
                return false;
            }

            int ret = isInsideArea(nextCorners, nextObjectPosition,
                                   vstatus);
            if (ret < 6) {
                return false;
            }

            sceneImage.copyTo(lastImage);

            corners = nextCorners;

            objectPosition = nextObjectPosition;
        }
    }
    return true;
}

int Tracker::isInsideArea(vector<cv::Point2f> &points, vector<cv::Point2f> &cornerPoints,
                          vector<unsigned char> &status) {

    CV_Assert(cornerPoints.size() == 4);
    CV_Assert(points.size() == status.size());

    // ax+by+c=0
    float a[4];
    float b[4];
    float c[4];

    a[0] = cornerPoints[3].y - cornerPoints[0].y;
    a[1] = cornerPoints[2].y - cornerPoints[1].y;
    a[2] = cornerPoints[1].y - cornerPoints[0].y;
    a[3] = cornerPoints[2].y - cornerPoints[3].y;

    b[0] = cornerPoints[0].x - cornerPoints[3].x;
    b[1] = cornerPoints[1].x - cornerPoints[2].x;
    b[2] = cornerPoints[0].x - cornerPoints[1].x;
    b[3] = cornerPoints[3].x - cornerPoints[2].x;

    c[0] = cornerPoints[0].y * cornerPoints[3].x
           - cornerPoints[3].y * cornerPoints[0].x;
    c[1] = cornerPoints[1].y * cornerPoints[2].x
           - cornerPoints[2].y * cornerPoints[1].x;
    c[2] = cornerPoints[0].y * cornerPoints[1].x
           - cornerPoints[1].y * cornerPoints[0].x;
    c[3] = cornerPoints[3].y * cornerPoints[2].x
           - cornerPoints[2].y * cornerPoints[3].x;

    float xMax, xMin, yMax, yMin;
    xMax = cornerPoints[0].x;
    xMin = cornerPoints[0].x;
    yMax = cornerPoints[0].y;
    yMin = cornerPoints[0].y;

    int i;
    for (i = 1; i < 4; i++) {
        if (cornerPoints[i].x > xMax) xMax = cornerPoints[i].x;
        if (cornerPoints[i].x < xMin) xMin = cornerPoints[i].x;
        if (cornerPoints[i].y > yMax) yMax = cornerPoints[i].y;
        if (cornerPoints[i].y < yMin) yMin = cornerPoints[i].y;
    }

    float values[4];
    int size = points.size();
    int count = 0;
    for (int j = 0; j < size; j++) {
        if (status[j] > 0) {
            for (i = 0; i < 4; i++) {
                values[i] = a[i] * points[j].x + b[i] * points[j].y + c[i];
            }
            if (values[0] * values[1] <= 0 && values[2] * values[3] <= 0) {
                count++;
            } else {
                status[j] = 0;
            }
        }
    }

    return count;
}

bool Tracker::isRectangle(vector<cv::Point2f> &rectanglePoints) {

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

bool Tracker::isObjectInsideImage(cv::Size imageSize, vector<cv::Point2f> &points) {

    // test is all points are inside the image area
    vector<cv::Point2f>::iterator itr = points.begin();
    while (itr != points.end()) {
        if (itr->x < 0 || itr->x >= imageSize.width || itr->y < 0
            || itr->y >= imageSize.height) {
            return false;
        } else {
            itr++;
        }
    }
    return true;
}

cv::Mat Tracker::getLastDirection(vector<cv::Point2f> &pointVector) {

    // Convert Point2f structure vector to Mat type in the homogeneous coordinates
    int size = pointVector.size();
    cv::Mat objectDirection(3, size, CV_64FC1);

    for (int i = 0; i < size; i++) {
        objectDirection.at<double>(0, i) = (double) (pointVector[i].x);
        objectDirection.at<double>(1, i) = (double) (pointVector[i].y);
        objectDirection.at<double>(2, i) = 1.0;
    }

    return objectDirection;
}

vector<cv::Point2f> Tracker::calcAffineTransformation(vector<cv::Point2f> &pointVector,
                                                      cv::Mat &transformation) {
    vector<cv::Point2f> objectPosition;

    if (pointVector.empty()) {
        return objectPosition;
    }

    cv::Mat lastDirection = getLastDirection(pointVector);
    cv::Mat newDirection = transformation * lastDirection;

    cv::Point2f points;
    int cols = newDirection.cols;
    for (int i = 0; i < cols; i++) {
        points.x = (float) (newDirection.at<double>(0, i)
                        / newDirection.at<double>(2, i));
        points.y = (float) (newDirection.at<double>(1, i)
                        / newDirection.at<double>(2, i));
        objectPosition.push_back(points);
    }

    return objectPosition;
}

cv::Mat Tracker::createAreaMask(cv::Size imageSize, vector<cv::Point2f> &points) {

    // create feature areaMask
    cv::Mat areaMask(imageSize, CV_8UC1);

    int zero = 0;
    areaMask = zero;

    // ax + by + c = 0
    float a[4];
    float b[4];
    float c[4];

    a[0] = points[3].y - points[0].y;
    a[1] = points[2].y - points[1].y;
    a[2] = points[1].y - points[0].y;
    a[3] = points[2].y - points[3].y;

    b[0] = points[0].x - points[3].x;
    b[1] = points[1].x - points[2].x;
    b[2] = points[0].x - points[1].x;
    b[3] = points[3].x - points[2].x;

    c[0] = points[0].y * points[3].x - points[3].y * points[0].x;
    c[1] = points[1].y * points[2].x - points[2].y * points[1].x;
    c[2] = points[0].y * points[1].x - points[1].y * points[0].x;
    c[3] = points[3].y * points[2].x - points[2].y * points[3].x;

    float xMax, xMin, yMax, yMin;
    xMax = 0;
    xMin = imageSize.width;
    yMax = 0;
    yMin = imageSize.height;

    int i;
    for (i = 0; i < 4; i++) {
        if (points[i].x > xMax) xMax = points[i].x;
        if (points[i].x < xMin) xMin = points[i].x;
        if (points[i].y > yMax) yMax = points[i].y;
        if (points[i].y < yMin) yMin = points[i].y;
    }
    if (xMax >= imageSize.width) xMax = imageSize.width - 1;
    if (yMax >= imageSize.height) yMax = imageSize.height - 1;
    if (xMin < 0) xMin = 0;
    if (yMin < 0) yMin = 0;

    unsigned char *maskPointer = areaMask.data;
    int x, y;
    int offset;
    float values[4];
    for (y = yMin; y <= yMax; y++) {
        offset = y * imageSize.width;
        for (x = xMin; x <= xMax; x++) {
            for (i = 0; i < 4; i++) {
                values[i] = a[i] * x + b[i] * y + c[i];
            }
            if (values[0] * values[1] <= 0 && values[2] * values[3] <= 0)
                *(maskPointer + offset
                  + x) = 255;
        }
    }

    return areaMask;
}

#endif
