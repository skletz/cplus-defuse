#ifndef _DEFUSE_DFS2XTRACTOR_HPP_
#define _DEFUSE_DFS2XTRACTOR_HPP_
#ifdef __cplusplus

#include <cplusutil.h>
#include "featsig/include/signatures.h"
#include "dfeatsig_v1//src/tpct_signatures.hpp"
#include "../features.hpp"
#include "../parameter.hpp"
#include "../../videobase.hpp"
#include "../xtractor.hpp"
#include "util/samplepoints.hpp"

#include <opencv2/opencv.hpp>

namespace defuse {
	namespace tpct = analysis::tpct_signatures;
	namespace pct = cv::xfeatures2d;

	class DFS2Xtractor : public Xtractor
	{
	public:
		enum  class IDX { X = 0, Y, T, L, A, B, MD, MI, WEIGHT, DIMS };

		cv::Ptr<tpct::TPCTSignatures> mTPCTSignatures;

		cv::Ptr<pct::PCTSignatures> mPCTSignatures;

		SamplePoints* mSamplepoints;

		int mMaxCluster;

		int mMaxFrames;

		DFS2Xtractor(Parameter* _parameter);

		Features* xtract(VideoBase* _videobase) override;

		void computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const;

		void joinCloseClusters(cv::Mat clusters);
		void dropLightPoints(cv::Mat& clusters);
		std::size_t findClosestCluster(cv::Mat &clusters, cv::Mat &points, std::size_t pointIdx);
		void normalizeWeights(cv::Mat &clusters);
		void singleClusterFallback(const cv::Mat &points, cv::Mat &clusters);
		void cropClusters(cv::Mat &clusters);

		float computeL2(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2);
	};
}
#endif
#endif