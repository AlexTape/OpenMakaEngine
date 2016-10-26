#pragma once
#include <opencv2/core/core.hpp>

class Geometry
{
private:
	Geometry(void);
	virtual ~Geometry(void);
public:

	enum RECTANGLE_MODE {MAX, MIN, FIT};

	static bool isRectangle(std::vector<cv::Point2f>& rectanglePoints);
	static std::vector<cv::Point_<float>> rescale(std::vector<cv::Point_<float>>& points2d);
	static CvRect fitRectangle(std::vector<cv::Point_<float>> rectangle, RECTANGLE_MODE mode = MAX);
	static cv::Mat cutRoi(cv::Mat& image, CvRect rect);
	static cv::Mat getRoi(cv::Mat& image, CvRect rect);
};

