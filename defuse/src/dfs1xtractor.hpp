#ifndef _DEFUSE_DFS1XTRACTOR_HPP_
#define _DEFUSE_DFS1XTRACTOR_HPP_
#ifdef __cplusplus

#include "features.hpp"
#include "parameter.hpp"
#include "videobase.hpp"
#include "xtractor.hpp"
#include "samplepoints.hpp"

#include <opencv2/opencv.hpp>

namespace defuse {
	namespace tpct = analysis::tpct_signatures;
	namespace pct = cv::xfeatures2d;

	class DFS1Xtractor : public Xtractor
	{
	public:
		enum class IDX { X1 = 0, Y2, L, A, B, C, E, WEIGHT, X1_IDX, X2_IDX, DIMS };

		cv::Ptr<tpct::TPCTSignatures> mTPCTSignatures;

		cv::Ptr<pct::PCTSignatures> mPCTSignatures;

		SamplePoints* mSamplepoints;

		int mMaxCluster;

		int mMaxFrames;

		int mFrameSelection;

		DFS1Xtractor(Parameter* _parameter);

		Features* xtract(VideoBase* _videobase) override;


		void computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const;
	};

}
#endif
#endif