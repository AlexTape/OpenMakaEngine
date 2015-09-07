//
// Created by thinker on 27.07.15.
//

#ifndef OPENMAKAENGINE_OBJECTPATTERN_H
#define OPENMAKAENGINE_OBJECTPATTERN_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

namespace om {

    class ObjectPattern {

    public:

        ObjectPattern(const cv::Mat &grayImage);

        cv::Size size;
        cv::Mat image;

        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;

        std::vector<cv::Point2f> points2d;
        std::vector<cv::Point3f> points3d;

        virtual ~ObjectPattern(void);

        void build();

    };

};

#endif
