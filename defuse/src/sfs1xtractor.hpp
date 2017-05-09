#ifndef _DEFUSE_SFS1XTRACTOR_HPP_
#define _DEFUSE_SFS1XTRACTOR_HPP_
#ifdef __cplusplus

#include "videobase.hpp"
#include "xtractor.hpp"
#include "features.hpp"
#include "parameter.hpp"

#include "samplepoints.hpp"
#include <cvpctsig.h>
#include <opencv2/opencv.hpp>

namespace defuse {
	namespace pct = cv::xfeatures2d;

	class SFS1Xtractor : public Xtractor
	{
	public:

		enum class IDX { X = 0, Y, L, A, B, C, E, WEIGHT, DIMS };

		cv::Ptr<pct::PCTSignatures> mPCTSignatures;

		SamplePoints* mSamplepoints;

		int mMaxCluster;

		int mKeyFrame;
                
		SFS1Xtractor(Parameter* _parameter);

		Features* xtract(VideoBase* _videobase) override;

		void computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const;

	};

}
#endif
#endif