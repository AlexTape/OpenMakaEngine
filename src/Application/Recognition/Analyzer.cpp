#pragma once
#ifndef OPENMAKAENGINE_ANALYZER_CPP
#define OPENMAKAENGINE_ANALYZER_CPP

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <iomanip>

#include "Analyzer.h"
#include "../Controller.h"
#include "../Helper/Drawer.h"
#include "../akaze/akaze_features.h"

#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#define fpu_error(x) (isinf(x) || isnan(x))

using namespace std;
using namespace om;

// init static members
Analyzer *Analyzer::inst_ = NULL;
string Analyzer::DETECTOR;
string Analyzer::EXTRACTOR;
string Analyzer::MATCHER;
int Analyzer::MINIMUM_INLIERS;
int Analyzer::MINIMUM_MATCHES;
float Analyzer::NN_DISTANCE_RATIO;
int Analyzer::K_GROUPS;
double Analyzer::RANSAC_REPROJECTION_THRESHOLD;

Analyzer::Analyzer(void) {

    if (Controller::MODE_DEBUG) {
        cout << "Creating Analyzer instance.." << endl;
    }

    // init nonfree module for SURF support
    cv::initModule_nonfree();

    // setup measurements
    clock = new Timer();
    timer = new Timer();

    // preinit vars to avoid segmentation faults
    activeObjectPattern = 0;

    // set variables
    isInitialized = false;
}

Analyzer::~Analyzer(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting Analyzer instance.." << endl;
    }
    releaseOpenCV();
    delete activeObjectPattern;
    delete clock;
    delete timer;
}

Analyzer *Analyzer::getInstance() {
    if (inst_ == NULL) {

        // create singleton instance
        inst_ = new Analyzer();

        // init attributes
        inst_->isInitialized = false;

    }
    return inst_;
}

void Analyzer::initDetector(std::string &type) {
    if (type == "SIFT") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::SIFT(400, 3, 0.04, 25, 1.6));
    }
    else if (type == "FAST") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::FastFeatureDetector(20, true));
    }
    else if (type == "GFTT") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::GFTTDetector(1000, 0.01, 1, 3, false, 0.04));
    }
    else if (type == "MSER") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::MSER(5, 60, 14400, 0.25, .2, 200, 1.01, 0.003, 5));
    }
    else if (type == "DENSE") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::DenseFeatureDetector(1.f, 1, 0.1f, 6, 0, true, false));
    }
    else if (type == "STAR") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::StarFeatureDetector(45, 30, 10, 8, 5));
    }
    else if (type == "SURF") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::SURF(600.0, 4, 2, true, false));
    }
    else if (type == "BRISK") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::BRISK(30, 3, 1.0f));
    }
    else if (type == "ORB") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::ORB(500, 1.2f, 8, 31,
                                                            0, 2, cv::ORB::HARRIS_SCORE, 31));
    }
    else if (type == "AKAZE") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::AKAZE());
    }
}

void Analyzer::initExtractor(std::string &type) {
    if (type == "SIFT") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::SIFT(0, 3, 0.04, 10, 1.6));
        distance = cv::NORM_L2SQR;
    }
    else if (type == "BRIEF") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::BriefDescriptorExtractor(32));
        distance = cv::NORM_HAMMING;
    }
    else if (type == "ORB") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::ORB(500, 1.2f, 8, 31, 0, 2, cv::ORB::HARRIS_SCORE, 31));
        distance = cv::NORM_HAMMING;
    }
    else if (type == "SURF") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::SURF(600.0, 4, 2, true, false));
        distance = cv::NORM_L2SQR;
    }
    else if (type == "BRISK") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::BRISK(30, 3, 1.0f));
        distance = cv::NORM_HAMMING;
    }
    else if (type == "FREAK") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::FREAK(false, false, 22.0f, 4, vector<int>()));
        distance = cv::NORM_HAMMING;
    }
    else if (type == "AKAZE") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::AKAZE());
        distance = cv::NORM_HAMMING;
    }
}

void Analyzer::initMatcher(std::string &type) {

    if (type == "BF") {

        // NOTE: OpenCV Error: Assertion failed (K == 1 && update == 0 && mask.empty()) in batchDistance
        // https://github.com/MasteringOpenCV/code/issues/
        matcher = cv::Ptr<cv::DescriptorMatcher>(new cv::BFMatcher(distance, false));
        // matcher = cv::Ptr<cv::DescriptorMatcher>(new cv::BFMatcher(cv::NORM_HAMMING, false));

        // using bruteforce matching
        IS_BRUTEFORCE_MATCHER = true;

    } else {

        // use flannbased matching
        IS_BRUTEFORCE_MATCHER = false;

    }

    // else if (type == "FLANN_LSF") {
    //  indexParams = new cv::flann::LshIndexParams(12, 20, 2);
    //  searchParams = new cv::flann::SearchParams();
    //  matcher = cv::Ptr<cv::DescriptorMatcher>(new cv::FlannBasedMatcher(indexParams, searchParams));
    //  IS_BRUTEFORCE_MATCHER = false;
    // }
    // else if (type == "FLANN_KD") {
    //  indexParams = new cv::flann::KDTreeIndexParams();
    //  searchParams = new cv::flann::SearchParams();
    //  matcher = cv::Ptr<cv::DescriptorMatcher>(new cv::FlannBasedMatcher(indexParams, searchParams));
    //  IS_BRUTEFORCE_MATCHER = false;
    // }
}

bool Analyzer::analyze(cv::Mat &gray, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors) {

    bool returnThis = true;

    try {

        if (detector->name() == "Feature2D.SIFT" && extractor->name() == "Feature2D.ORB") {
            // TODO fix testConfigurations.push_back(std::vector<string>{"SIFT", "ORB", ...});
            // see http://code.opencv.org/issues/2987 and http://code.opencv.org/issues/1277

            // OpenCV Error: Assertion failed (dsize.area() || (inv_scale_x > 0 && inv_scale_y > 0)) in resize, file ../opencv-2.4.11/modules/imgproc/src/imgwarp.cpp, line 1969
            // ../opencv-2.4.11/modules/imgproc/src/imgwarp.cpp:1969: error: (-215) dsize.area() || (inv_scale_x > 0 && inv_scale_y > 0) in function resize

            // is sift-orb supposed to work? Skip this configuration till functionality is fixed..

            // fix test results (dirty way);
            if (IS_BRUTEFORCE_MATCHER) {
                Controller::statistics("Matcher", (string) "BF");
            } else {
                Controller::statistics("Matcher", (string) "FLANN");
            }
            return false;
        }

        // if there is no image data
        if (Controller::MODE_DEBUG) {
            assert(!gray.empty());
        }
        if (gray.empty()) {
            cvError(0, "Analyzer", "Input image empty!", __FILE__, __LINE__);
        }

        // detect keypoints
        keypoints.clear();
        if (Controller::MODE_DEBUG) {
            assert(keypoints.empty());
        }
        detector->detect(gray, keypoints);

        // no keypoints detected!
        if (keypoints.empty()) {
            // Keep in mind that maybe no features are present in actual image!
            if (Controller::MODE_DEBUG) {
                cvError(0, "Analyzer", "Detection keypoints empty!", __FILE__, __LINE__);
            }
            // no keypoints to compute
            return false;
        }

        // compute descriptors
        descriptors.release();
        if (Controller::MODE_DEBUG) {
            assert(descriptors.empty());
        }

        extractor->compute(gray, keypoints, descriptors);

        // note: keypoints for which a descriptor cannot be computed are removed
        if (keypoints.empty()) {
            if (Controller::MODE_DEBUG) {
                cvError(0, "Analyzer", "Compute keypoints empty!", __FILE__, __LINE__);
            }
            // if all keypoints are removed, no descriptors could be computed
            return false;
        }

    } catch (cv::Exception &exception) {
        if (Controller::MODE_DEBUG) {
            // NOTE: e.g. dark images can contain 0 features!
            cout << exception.what() << endl;
        }
        returnThis = false;
    }

    return returnThis;
}

bool Analyzer::analyzeSceneFrame(SceneFrame &sceneFrame) {

    // validate input data
    if (sceneFrame.gray.empty()) {
        if (Controller::MODE_DEBUG) {
            cvError(0, "Analyzer::process", "Scene image empty!", __FILE__, __LINE__);
        }
        return false;
    }
    if (activeObjectPattern->image.empty()) {
        if (Controller::MODE_DEBUG) {
            cvError(0, "Analyzer::process", "Object image empty!", __FILE__, __LINE__);
        }
        return false;
    }

    analyze(sceneFrame.gray, sceneFrame.keypoints, sceneFrame.descriptors);

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("SceneKeypoints", (int) sceneFrame.keypoints.size());
        Controller::statistics("ObjectKeypoints", (int) activeObjectPattern->keypoints.size());
    }

    if (Controller::MODE_DEBUG) {
        cout << "Keypoints(Scene/Object)=" << sceneFrame.keypoints.size() << "/" <<
        activeObjectPattern->keypoints.size() <<
        endl;
    }

    if (activeObjectPattern->descriptors.empty()) {
        if (Controller::MODE_DEBUG) {
            cout << "object descriptors empty" << endl;
        }
        return false;
    }

    if (sceneFrame.descriptors.empty()) {
        if (Controller::MODE_DEBUG) {
            cout << "scene descriptors empty" << endl;
        }
        return false;
    }
}

void Analyzer::matchBinaryDescriptors(SceneFrame &sceneFrame, std::vector<cv::Point2f> &goodTrainKeypoints,
                                      std::vector<cv::Point2f> &goodSceneKeypoints) {

    if (Controller::MODE_DEBUG) {
        cout << "Binary descriptors detected..." << endl;
    }

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("DescriptorType", (string) "binary");
    }

    timer->restart();

    cv::Mat resultIndex;
    cv::Mat distanceIndex;
    std::vector<std::vector<cv::DMatch> > matches;
    std::vector<int> objectKeypointIndex, sceneKeypointIndex;

    if (IS_BRUTEFORCE_MATCHER) {

        if (Controller::MODE_DEBUG) {
            cout << "BruteForce matching.." << endl;
        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("Matcher", (string) "BF");
        }

        // TODO train first
        // matcher.radiusMatch(activeObjectPattern->descriptors, sceneFrame.descriptors, matches, 1.0 );
        matcher->knnMatch(activeObjectPattern->descriptors, sceneFrame.descriptors, matches, K_GROUPS);

        for (unsigned int i = 0; i < matches.size(); ++i) {

            if (Controller::MODE_DEBUG) {
                //cout << "DistanceIndex=" << i << "; distance1=" << matches.at(i).at(0).queryIdx << "; distance2=" <<
                //matches.at(i).at(0).distance << ";" << endl;
            }

            unsigned int queryId = abs(matches.at(i).at(0).queryIdx);
            goodTrainKeypoints.push_back(activeObjectPattern->keypoints.at(queryId).pt);
            objectKeypointIndex.push_back(matches.at(i).at(0).queryIdx);

            unsigned int trainId = abs(matches.at(i).at(0).trainIdx);
            goodSceneKeypoints.push_back(sceneFrame.keypoints.at(trainId).pt);
            sceneKeypointIndex.push_back(matches.at(i).at(0).trainIdx);

        }

    } else {

        // Create Flann LSH index
        cv::flann::Index flannIndex(sceneFrame.descriptors, cv::flann::LshIndexParams(12, 20, 2),
                                    cvflann::FLANN_DIST_HAMMING);

        if (Controller::MODE_DEBUG) {
            cout << "Flann LSH Index created.." << endl;
        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("Matcher", (string) "Flann");
        }

        resultIndex = cv::Mat(activeObjectPattern->descriptors.rows, K_GROUPS, CV_32SC1);
        distanceIndex = cv::Mat(activeObjectPattern->descriptors.rows, K_GROUPS,
                                CV_32FC1);

        flannIndex.knnSearch(activeObjectPattern->descriptors, resultIndex, distanceIndex, K_GROUPS,
                             cv::flann::SearchParams());

        for (int i = 0; i < activeObjectPattern->descriptors.rows; ++i) {

            if (Controller::MODE_DEBUG) {
                //cout << "DistanceIndex=" << i << "; distance1=" << distanceIndex.at<float>(i, 0) << "; distance2=" <<
                //distanceIndex.at<float>(i, 1) << ";" << endl;
            }

            // TODO check if needed!
            if (isnan(distanceIndex.at<float>(i, 0)) || isnan(distanceIndex.at<float>(i, 1))) {
                continue;
            }

            goodTrainKeypoints.push_back(activeObjectPattern->keypoints.at(i).pt);
            objectKeypointIndex.push_back(i);

            unsigned int resultIndexId = abs(resultIndex.at<int>(i, 0));
            goodSceneKeypoints.push_back(sceneFrame.keypoints.at(resultIndexId).pt);
            sceneKeypointIndex.push_back(resultIndex.at<int>(i, 0));
        }
    }

    if (Controller::MODE_DEBUG) {
        cout << "matchingDescriptors=" << timer->getMillis() << endl;
    }

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("MatchingDescriptors(ms)", (double) timer->getMillis());
    }
}

void Analyzer::matchFloatDescriptors(SceneFrame &sceneFrame, std::vector<cv::Point2f> &goodTrainKeypoints,
                                     std::vector<cv::Point2f> &goodSceneKeypoints) {

    if (Controller::MODE_DEBUG) {
        cout << "Float descriptors detected..." << endl;
    }

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("DescriptorType", (string) "float");
    }

    timer->restart();

    // temp result objects
    cv::Mat resultIndex;
    cv::Mat distanceIndex;
    std::vector<std::vector<cv::DMatch> > matches;

    // Find correspondences by NNDR (Nearest Neighbor Distance Ratio)
    // Check if this descriptor matches with those of the objects
    std::vector<int> objectKeypointIndex, sceneKeypointIndex; // Used for homography

    if (IS_BRUTEFORCE_MATCHER) {

        if (Controller::MODE_DEBUG) {
            cout << "BruteForce NORM_L2 matching.." << endl;
        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("Matcher", (string) "BF");
        }

        // knnMatch
        matcher->knnMatch(activeObjectPattern->descriptors, sceneFrame.descriptors, matches, K_GROUPS);

        try {

            for (unsigned int i = 0; i < matches.size(); ++i) {

                if (Controller::MODE_DEBUG) {
                    //cout << "DistanceIndex=" << i << "; distance1=" << matches.at(i).at(0).queryIdx << "; distance2=" <<
                    //matches.at(i).at(0).distance << ";" << endl;
                }

                if (matches.at(i).at(0).distance <= NN_DISTANCE_RATIO * matches.at(i).at(1).distance) {

                    unsigned int queryId = abs(matches.at(i).at(0).queryIdx);
                    goodTrainKeypoints.push_back(activeObjectPattern->keypoints.at(queryId).pt);
                    objectKeypointIndex.push_back(matches.at(i).at(0).queryIdx);

                    unsigned int trainId = abs(matches.at(i).at(0).trainIdx);
                    goodSceneKeypoints.push_back(sceneFrame.keypoints.at(trainId).pt);
                    sceneKeypointIndex.push_back(matches.at(i).at(0).trainIdx);
                }
            }

        } catch (std::out_of_range &exception) {
            // fix this error:
            // terminate called after throwing an instance of 'std::out_of_range'
            // what():  vector::_M_range_check
            if (Controller::MODE_DEBUG) {
                cvError(0, "Analyzer", "Matches out of range!", __FILE__, __LINE__);
                cout << exception.what() << endl;
            }
        }

    } else {

        // Create Flann KDTree index
        cv::flann::Index flannIndex(sceneFrame.descriptors, cv::flann::KDTreeIndexParams(),
                                    cvflann::FLANN_DIST_EUCLIDEAN);

        if (Controller::MODE_DEBUG) {
            cout << "Flann KDTree created.." << endl;
        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("Matcher", (string) "Flann");
        }

        resultIndex = cv::Mat(activeObjectPattern->descriptors.rows, K_GROUPS, CV_32SC1); // Results index
        distanceIndex = cv::Mat(activeObjectPattern->descriptors.rows, K_GROUPS,
                                CV_32FC1); // Distance resultIndex are CV_32FC1

        // search (nearest neighbor)
        flannIndex.knnSearch(activeObjectPattern->descriptors, resultIndex, distanceIndex, K_GROUPS,
                             cv::flann::SearchParams());

        try {

            for (int i = 0; i < activeObjectPattern->descriptors.rows; ++i) {

                if (Controller::MODE_DEBUG) {
                    //cout << "DistanceIndex=" << i << "; distance1=" << distanceIndex.at<float>(i, 0) << "; distance2=" <<
                    //distanceIndex.at<float>(i, 1) << ";" << endl;
                }

                // TODO check if needed!
                if (isnan(distanceIndex.at<float>(i, 0)) || isnan(distanceIndex.at<float>(i, 1))) {
                    continue;
                }

                if (distanceIndex.at<float>(i, 0) <= NN_DISTANCE_RATIO * distanceIndex.at<float>(i, 1)) {
                    goodTrainKeypoints.push_back(activeObjectPattern->keypoints.at(i).pt);
                    objectKeypointIndex.push_back(i);

                    unsigned int resultIndexId = abs(resultIndex.at<int>(i, 0));
                    goodSceneKeypoints.push_back(sceneFrame.keypoints.at(resultIndexId).pt);
                    sceneKeypointIndex.push_back(resultIndex.at<int>(i, 0));
                }
            }

        } catch (std::out_of_range &exception) {
            // fix this error:
            // terminate called after throwing an instance of 'std::out_of_range'
            // what():  vector::_M_range_check
            if (Controller::MODE_DEBUG) {
                cvError(0, "Analyzer", "Matches out of range!", __FILE__, __LINE__);
                cout << exception.what() << endl;
            }
        }
    }

    if (Controller::MODE_DEBUG) {
        cout << "matchingDescriptors=" << timer->getMillis() << endl;
    }

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("MatchingDescriptors(ms)", (double) timer->getMillis());
    }

}

bool Analyzer::process(SceneFrame &sceneFrame) {

    // calc processing time for this method
    if (Controller::MODE_STATISTICS) {
        clock->restart();
    }

    bool enoughInliers = false;

    if (isInitialized) {

        if (Controller::MODE_STATISTICS) {
            timer->restart();
        }

        // analyze features and descriptors for frame
        analyzeSceneFrame(sceneFrame);

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("AnalyzeSceneFrame(ms)", (double) timer->getMillis());
        }

        std::vector<cv::Point2f> goodTrainKeypoints;
        std::vector<cv::Point2f> goodSceneKeypoints;

        // check preconditions
        // there have to be descriptors! (catch black screens etc)
        if (activeObjectPattern->descriptors.rows <= 0) {
            return false;
        }
        if (sceneFrame.descriptors.rows <= 0) {
            return false;
        }

        if (activeObjectPattern->descriptors.type() == CV_8U) {
            matchBinaryDescriptors(sceneFrame, goodTrainKeypoints, goodSceneKeypoints);
        }

        if (activeObjectPattern->descriptors.type() == CV_32F) {
            matchFloatDescriptors(sceneFrame, goodTrainKeypoints, goodSceneKeypoints);
        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("GoodTrainKeypoints", (int) goodTrainKeypoints.size());
            Controller::statistics("GoodSceneKeypoints", (int) goodSceneKeypoints.size());
        }

        if (goodSceneKeypoints.size() != goodTrainKeypoints.size()) {
            if (Controller::MODE_DEBUG) {
                cvError(0, "Analyzer", "goodScene and goodObject keypoints size NOT equal!", __FILE__, __LINE__);
            }
            // not matched!
            return false;
        }

        int inliers = 0;

        // NOTE: minimum 4 points needed to calc homography
        if (goodSceneKeypoints.size() >= MINIMUM_MATCHES) {
            if (goodSceneKeypoints.size() == goodTrainKeypoints.size()) {

                if (Controller::MODE_STATISTICS) {
                    timer->restart();
                }

                // calculate inliers
                inliers = calcInliers(sceneFrame, goodTrainKeypoints, goodSceneKeypoints);

                if (Controller::MODE_STATISTICS) {
                    Controller::statistics("InliersCalc(ms)", (double) timer->getMillis());
                }

            } else {

                if (Controller::MODE_DEBUG) {
                    cout << "GoodSceneKeypoints != GoodTranKeypoints" << endl;
                }

            }
        } else {

            if (Controller::MODE_DEBUG) {
                cout << "Not enough keypoint matches (" << goodTrainKeypoints.size() << "/" << MINIMUM_MATCHES <<
                ") for homography.." << endl;
            }

        }

        enoughInliers = (inliers >= MINIMUM_INLIERS);

        // find perspective and draw rectangle
        if (enoughInliers) {

            if (Controller::MODE_STATISTICS) {
                timer->restart();
            }

            // calulate perspective transformation
            perspectiveTransform(activeObjectPattern->points2d, sceneFrame.objectPosition, sceneFrame.homography);

            if (Controller::MODE_STATISTICS) {
                Controller::statistics("PerspectiveTransform(ms)", (double) timer->getMillis());
            }

            if (Controller::MODE_USE_WINDOWS) {

                // drawing contours
                Drawer::drawContour(sceneFrame.gray, sceneFrame.objectPosition, cv::Scalar(0, 255, 0));

                //-- Show detected matches
//        cv::Mat Drawer::drawMatchesWindow(cv::Mat query, cv::Mat pattern, const std::vector<cv::KeyPoint> &queryKp,
//                                           const std::vector<cv::KeyPoint> &trainKp, std::vector<cv::DMatch> matches,
//                                           int maxMatchesDrawn) {

                // open custom windows
				string window_name = DETECTOR + "-" + EXTRACTOR + "-" + MATCHER;
				cv::namedWindow(window_name, 0); //resizable window;
	            cv::resizeWindow(window_name,800,800);
				imshow(window_name, sceneFrame.gray);
            }

        } else {

            if (Controller::MODE_DEBUG) {
                cout << "Not enough inliers (" << inliers << "/" << MINIMUM_INLIERS << ") calculating perspective" <<
                endl;
            }

        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("AnalyzerProcess(ms)", (double) clock->getMillis());
        }

    } else {
        if (Controller::MODE_DEBUG) {
            cvError(0, "Analyzer", "Analyzer not initialized!", __FILE__, __LINE__);
        }
    }

    // basic rule to devide if object was found or not
    bool objectFound = false;
    if (enoughInliers) {
        objectFound = isRectangle(sceneFrame.objectPosition);
    }

    return objectFound;
}

bool Analyzer::isRectangle(vector<cv::Point2f> &rectanglePoints) {

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

int Analyzer::calcInliers(SceneFrame &sceneFrame, std::vector<cv::Point2f> &goodTrainKeypoints,
                          std::vector<cv::Point2f> &goodSceneKeypoints) {

    std::vector<uchar> outlierMask;  // Used for homography

    sceneFrame.homography = findHomography(goodTrainKeypoints,
                                           goodSceneKeypoints,
                                           cv::RANSAC,
                                           RANSAC_REPROJECTION_THRESHOLD,
                                           outlierMask);

    int inliers = 0, outliers = 0;
    for (unsigned int k = 0; k < goodTrainKeypoints.size(); ++k) {
        if (outlierMask.at(k)) {
            ++inliers;
        }
        else {
            ++outliers;
        }
    }

    if (Controller::MODE_DEBUG) {
        cout << "calcInliers: Inliers=" << inliers << "; Outliers=" << outliers << ";" << endl;
    }

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("Inliers", (int) inliers);
        Controller::statistics("Outliers", (int) outliers);
    }

    return inliers;
}

void Analyzer::match(SceneFrame &query) {
    assert(!query.descriptors.empty());
//    query.matches.clear();
    matcher->match(query.descriptors, query.matches);
}

void Analyzer::train(const cv::Mat &descriptors) {

    // clear old training data
    matcher->clear();

    // TODO register pattern to list and extends descriptor vector to hold multiple patterns?
    // create a descriptor vector to store descriptor data
    std::vector<cv::Mat> descriptorList(1);
    descriptorList[0] = descriptors.clone();

    // promote descriptor list to matcher instance
    matcher->add(descriptorList);

    // train matcher
    matcher->train();
}

// TODO überarbeiten
bool Analyzer::refineMatches
        (SceneFrame &query, ObjectPattern &pattern) {

    const int minNumberMatchesAllowed = 8;

    // TODO make matching working..
    cout << "RefineMatches [START]: SceneFrame Keypoints: " << std::setw(4) << query.keypoints.size() <<
    "; SceneFrame Matches: " << std::setw(4) <<
    query.matches.size() << endl;

    std::vector<cv::KeyPoint> trainKeypoints = pattern.keypoints;

    if (query.matches.size() < minNumberMatchesAllowed)
        return false;

    // Prepare data for cv::findHomography
    std::vector<cv::Point2f> srcPoints(query.matches.size());
    std::vector<cv::Point2f> dstPoints(query.matches.size());

    for (size_t i = 0; i < query.matches.size(); i++) {

        cout << pattern.keypoints[i].response << endl;

    }

    for (size_t i = 0; i < query.matches.size(); i++) {

        srcPoints[i] = pattern.keypoints[query.matches[i].trainIdx].pt;
        dstPoints[i] = query.keypoints[query.matches[i].queryIdx].pt;
    }

    cout << srcPoints.size() << "::" << dstPoints.size() << endl;

    // Find homography matrix and get inliers mask
    std::vector<unsigned char> inliersMask(srcPoints.size());
    cv::Mat homography = cv::findHomography(srcPoints,
                                            dstPoints,
                                            CV_FM_RANSAC,
                                            3.0,
                                            inliersMask);

    // TODO update homo here
    //trackerObject.homography = &homography;

    std::vector<cv::DMatch> inliers;
    for (size_t i = 0; i < inliersMask.size(); i++) {
        if (inliersMask[i]) {
            inliers.push_back(query.matches[i]);
        }
    }

    query.matches.swap(inliers);

    cout << "RefineMatches [ END ]: SceneFrame Keypoints: " << std::setw(4) << query.keypoints.size() <<
    "; SceneFrame Matches: " << std::setw(4) <<
    query.matches.size() << endl;

    return query.matches.size() > minNumberMatchesAllowed;

}

bool Analyzer::initialize() {
    if (!isInitialized) {

        // delete activeObjectPattern
        delete activeObjectPattern;
        activeObjectPattern = 0;

        // clear detector/extractor instances if needed
        if (Analyzer::detector || Analyzer::extractor) {
            Analyzer::releaseOpenCV();
        }

        // init detector/extractor
        if (!Analyzer::DETECTOR.empty() && !Analyzer::EXTRACTOR.empty() && !Analyzer::MATCHER.empty()) {
            Analyzer::initDetector(DETECTOR);
            Analyzer::initExtractor(EXTRACTOR);
            Analyzer::initMatcher(MATCHER);
        } else {
            return false;
        }

        // done
        isInitialized = true;
    }
    return isInitialized;
}

bool Analyzer::releaseOpenCV() {
    Analyzer::detector.release();
    Analyzer::extractor.release();
    Analyzer::matcher.release();
    return true;
}

bool Analyzer::createObjectPattern(cv::Mat &gray) {

    bool returnThis = false;

    if (!gray.empty()) {
        delete activeObjectPattern;
        activeObjectPattern = new ObjectPattern(gray);
        if (activeObjectPattern && isInitialized) {
            returnThis = analyze(activeObjectPattern->image, activeObjectPattern->keypoints,
                                 activeObjectPattern->descriptors);
        }
    } else {
        if (Controller::MODE_DEBUG) {
            cvError(0, "Analyzer::createObjectPattern", "Image empty!", __FILE__, __LINE__);
        }
    }

    return returnThis;
}

bool Analyzer::missingObjectPattern() {
    bool returnThis = true;
    if (!activeObjectPattern) {
        cv::Mat objectImage = cv::imread(Controller::STORAGE_PATH + Controller::DEFAULT_OBJECT_IMAGE,
                                         CV_LOAD_IMAGE_GRAYSCALE);
        createObjectPattern(objectImage);
    } else {
        returnThis = false;
    }
    return returnThis;
}

#endif
