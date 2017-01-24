#ifndef OPENMAKAENGINE_CONTROLLER_H
#define OPENMAKAENGINE_CONTROLLER_H

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Recognition/Analyzer.h"
#include "Recognition/ObjectPattern.h"
#include "Recognition/SceneFrame.h"
#include "Tracker/Tracker.h"
#include "Helper/Timer.h"
#include "Helper/Statistics.h"

#define ControllerTAG "OpenMaka::Controller"

namespace om
{
	class Controller
	{
	private:
		static Controller* inst_;

		Controller(void);

		static bool MODE_OBJECT_DETECTION;
		static bool MODE_TRACKING;
		static bool MODE_OPENGL;

		bool STATE_OBJECT_FOUND;
		bool STATE_TRACKING_OBJECT;
		bool STATE_DISPLAY_OPENGL;

		static om::Analyzer* analyzer; // Analyzer object
		static om::Tracker* tracker; // Tracker object
		static om::Statistics* stats; // Statistics object

	public:

		virtual ~Controller(void);

		static Controller* getInstance();

		int initialize(cv::Mat& frame, std::string storagePath);

		int displayFunction(cv::Mat& mRgbaFrame, cv::Mat& mGrayFrame, std::string imageName = "nill");

		void glResize(int height, int width);

		int setDetector(std::string type);

		int setExtractor(std::string type);

		int setMatcher(std::string type);

		void isModeObjectDetection(bool isActive);

		void isModeTracking(bool isActive);

		void isModeOpenGL(bool isActive);

		void glRender();

		bool createObjectPattern(cv::Mat& rgb, cv::Mat& gray);

		bool configure(std::string detector, std::string extractor, std::string matcher);
		
		int test(std::vector<std::basic_string<char>> images, int test, int quantifier);

		om::Timer* clock;

		om::Timer* timer;
		om::SceneFrame* sceneFrame;

		static bool isInitialized;

		static cv::Size FRAME_SIZE;
		static int MAX_IMAGE_SIZE;
		static int IMAGE_SCALE;
		static std::string STATISTICS_FILE;

		static std::string STORAGE_PATH;
		static std::string CONFIG_FILE;
		static std::string DEFAULT_OBJECT_IMAGE;

		static void statistics(std::string key, int value);

		static void statistics(std::string key, double value);

		static void statistics(std::string key, bool value);

		static void statistics(std::string key, long unsigned int value);

		static void statistics(std::string key, std::string value);

		static bool MODE_STATISTICS;
		static bool MODE_DEBUG;
		static bool MODE_USE_WINDOWS;
		static bool MODE_SAVE_RESULT_FRAMES;

		void isModeDebug(bool isActive);

		void isModeStatistics(bool isActive);


		typedef struct Configuration
		{
			std::string detector;
			std::string extractor;
			std::string matcher;
		} Configuration;
	};
};

#endif
