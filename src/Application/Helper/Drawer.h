#ifndef OPENMAKAENGINE_DRAWER_H
#define OPENMAKAENGINE_DRAWER_H

#include <opencv2/core/core.hpp>
#include "../Recognition/ObjectPattern.h"

namespace om {

    class Drawer {
    private:

        Drawer(void);

        virtual ~Drawer(void);

    public:

	    static void drawContour(cv::Mat &image, std::vector<cv::Point2f> points2d, cv::Scalar color, int thickness = 4,
                                int lineType = 8, int shift = 0);

        static void drawContourWithRescale(cv::Mat &image, std::vector<cv::Point2f> points2d, cv::Scalar color, int thickness = 4,
                                int lineType = 8, int shift = 0);

        static void drawKeypoints(cv::Mat &image, std::vector<cv::KeyPoint> keyPoints, cv::Scalar color);

        static cv::Mat drawMatchesWindow(cv::Mat query, cv::Mat pattern, const std::vector<cv::KeyPoint> &queryKp,
                                         const std::vector<cv::KeyPoint> &trainKp, std::vector<cv::DMatch> matches,
                                         int maxMatchesDrawn);

        static cv::Mat drawKeypointsWindow(cv::Mat query, cv::Mat pattern, const std::vector<cv::KeyPoint> &queryKp,
                                    const std::vector<cv::KeyPoint> &trainKp, std::vector<cv::DMatch> matches,
                                    int maxMatchesDrawn);

        static void drawKeypointsWithResponse(cv::Mat &image, std::vector<cv::KeyPoint> keyPoints, cv::Scalar color);
    };

};


#endif
