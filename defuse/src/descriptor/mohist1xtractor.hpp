#ifndef _DEFUSE_MOHIST1XTRACTOR_HPP_
#define _DEFUSE_MOHIST1XTRACTOR_HPP_
#ifdef __cplusplus

#include <cplusutil.hpp>
#include "featsig/include/signatures.h"
#include "dfeatsig_v1//src/tpct_signatures.hpp"
#include "../features.hpp"
#include "../parameter.hpp"
#include "../videobase.hpp"
#include "../xtractor.hpp"
#include "util/samplepoints.hpp"

#include <opencv2/opencv.hpp>

namespace defuse {
	namespace tpct = analysis::tpct_signatures;
	namespace pct = cv::xfeatures2d;

	class MoHist1Xtractor : public Xtractor
	{
	public:

		int mSampleX;
		int mSampleY;
		bool mFixSampling;

		SamplePoints* mSamplepoints;

		int mDistribution;

		int mMaxFrames;

		int mFrameSelection;

		int mVariant;

		float maxLenght = 3.0; //= height / 3.0
		int mFilterLength = 5;

		std::string imgOutputPath;
		std::string videoImgOutputpath = "";
		std::string currentVideoFile = "";

		MoHist1Xtractor(Parameter* _parameter);

		Features* xtract(VideoBase* _videobase) override;

		void computeMotionHistogram(cv::VideoCapture& _video, cv::OutputArray _histogram);

		void extractMotionHistogram(std::vector<uchar> status, std::vector<float> error, std::vector<cv::Point2f> prevCorner, std::vector<cv::Point2f> corner, int width, int height, cv::Mat& motionHist) const;

		void drawLine(cv::Mat& image, cv::Point p, cv::Point q, int scale);

		double square(int a);
		
		void drawMotionHistogram(cv::Mat& motionHist, bool showLength, cv::Mat& histImage);
	};

}
#endif
#endif