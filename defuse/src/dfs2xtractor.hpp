#ifndef _DEFUSE_DFS2XTRACTOR_HPP_
#define _DEFUSE_DFS2XTRACTOR_HPP_
#ifdef __cplusplus

#include <cplusutil.hpp>
#include "features.hpp"
#include "parameter.hpp"
#include "videobase.hpp"
#include "xtractor.hpp"
#include "samplepoints.hpp"

#include <cvpctsig.h>
#include <cvtfsig.h>
#include <opencv2/opencv.hpp>

namespace defuse {
	namespace tpct = analysis::tpct_signatures;
	namespace pct = cv::xfeatures2d;

	class DFS2Xtractor : public Xtractor
	{
	public:
		enum  class IDX { X = 0, Y, L, A, B, C, E, MD, WEIGHT, DIMS };

		cv::Ptr<tpct::TPCTSignatures> mTPCTSignatures;

		cv::Ptr<pct::PCTSignatures> mPCTSignatures;

		SamplePoints* mSamplepoints;

		int mMaxCluster;

		int mMaxFrames;

		int mFrameSelection;

		int mVariant;


		float maxLenght = 3.0; //= height / 3.0
		int mFilterLength = 5;

		int mIterations = 5;

		//Contrast, Entropy - Grayscale Bitmap
		//TODO make grayscale bits and window radius changable via parameter settings
		int mGrayscaleBits = 5;
		int mWindowRadius = 4;
		std::vector<double> mWeights;
		std::vector<double> mTranslations;

		DFS2Xtractor(Parameter* _parameter);

		Features* xtract(VideoBase* _videobase) override;

		void computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const;

		void getSamples(cv::Mat& frame, std::vector<cv::Point2f> initPoints, cv::Mat& samples) const;
		void getMotionDirection(std::vector<uchar>& statusVector, std::vector<float>& errorVector, std::vector<cv::Point2f>& prevPoints, std::vector<cv::Point2f>& currPoints, int height, int width, cv::Mat& out) const;
		void getTemporalSamples(cv::Mat& samples, cv::Mat& signatures) const;


		void joinCloseClusters(cv::Mat clusters) const;
		void dropLightPoints(cv::Mat& clusters) const;
		int findClosestCluster(cv::Mat &clusters, cv::Mat &points, int pointIdx) const;
		void normalizeWeights(cv::Mat &clusters) const;
		void singleClusterFallback(const cv::Mat &points, cv::Mat &clusters) const;
		void cropClusters(cv::Mat &clusters) const;

		void draw2DSampleSignature(const cv::InputArray _source, const cv::InputArray _signature, cv::OutputArray& _result) const;
		double square(int a) const;
		void drawLine(cv::Mat& image, cv::Point p, cv::Point q, int scale) const;

		void draw2DFSSignature(const cv::InputArray _source, const cv::InputArray _signature, cv::OutputArray& _result) const;
		void drawSignatureMotionDirection(const cv::InputArray& _tempSig, cv::OutputArray _result, int _width, int _height) const;

		float computeL2(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2) const;
	};
}
#endif
#endif