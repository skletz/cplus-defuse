#ifndef _DEFUSE_DFS1XTRACTOR_HPP_
#define _DEFUSE_DFS1XTRACTOR_HPP_
#ifdef __cplusplus

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

	class DFS1Xtractor : public Xtractor
	{
	public:
		enum class IDX { X = 0, Y, L, A, B, C, E, WEIGHT, DX_IDX, DY_IDX, DIMS };

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