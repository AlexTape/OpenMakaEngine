#ifndef OPENMAKAENGINE_ANALYZER_H
#define OPENMAKAENGINE_ANALYZER_H

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "ObjectPattern.h"
#include "SceneFrame.h"
#include "../Helper/Timer.h"

#define AnalyzerTAG "OpenMaka::Analyzer"

namespace om {

    class Analyzer {

    private:

        static Analyzer *inst_;

        Analyzer(void);

        cv::Ptr<cv::FeatureDetector> detector;

        cv::Ptr<cv::DescriptorExtractor> extractor;

        cv::Ptr<cv::DescriptorMatcher> matcher;

        int distance;

        ObjectPattern *activeObjectPattern;

        om::Timer *clock;

        om::Timer *timer;

    public:

        static std::string DETECTOR;
        static std::string EXTRACTOR;
        static std::string MATCHER;
        static int MINIMUM_INLIERS;
        static int MINIMUM_MATCHES;
        static float NN_DISTANCE_RATIO;
        static double RANSAC_REPROJECTION_THRESHOLD;

        bool IS_BRUTEFORCE_MATCHER;
        static int K_GROUPS;

        virtual ~Analyzer(void);

        bool isInitialized;

        static Analyzer *getInstance();

        bool releaseOpenCV();

        void initExtractor(std::string &type);

        void initDetector(std::string &type);

        void initMatcher(std::string &type);

        bool initialize();

        bool analyze(cv::Mat &gray, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors);


        bool process(SceneFrame &sceneFrame);

        bool createObjectPattern(cv::Mat &image);

        bool analyzeSceneFrame(SceneFrame &sceneFrame);

        void matchBinaryDescriptors(SceneFrame &sceneFrame, std::vector<cv::Point2f> &goodTrainKeypoints,
                                    std::vector<cv::Point2f> &goodSceneKeypoints);

        void matchFloatDescriptors(SceneFrame &sceneFrame, std::vector<cv::Point2f> &goodTrainKeypoints,
                                   std::vector<cv::Point2f> &goodSceneKeypoints);

        bool refineMatches(SceneFrame &query, ObjectPattern &pattern);

        void train(const cv::Mat &descriptors);

        void match(SceneFrame &sceneFrame);

        int calcInliers(SceneFrame &sceneFrame, std::vector<cv::Point2f> &goodTrainKeypoints,
                        std::vector<cv::Point2f> &goodSceneKeypoints);

        bool missingObjectPattern();

        bool isRectangle(std::vector<cv::Point2f> &rectanglePoints);

    };

};

#endif
