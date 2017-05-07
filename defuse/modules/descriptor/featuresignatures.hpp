#ifndef _DEFUSE_FEATURESIGNATURES_HPP_
#define _DEFUSE_FEATURESIGNATURES_HPP_

#ifdef __cplusplus

#include <cplusutil.hpp>
#include <cpluslogger.hpp>
#include <opencv2/opencv.hpp>
#include "../features.hpp"

namespace defuse {

	class FeatureSignatures : public Features
	{

	public:

		/**
		*	cv::Mat featureData of FeatureSignatures inheritent from AbstractFeature
		*
		*	| X       	| Y       	| L       	| A       	| B       	| CONTRAST 	| ENTROPY 	| WEIGHT  	|
		*	|---------	|---------	|---------	|---------	|---------	|----------	|---------	|---------	|
		*	| 0.0-1.0 	| 0.0-1.0 	| 0.0-1.0 	| 0.0-1.0 	| 0.0-1.0 	| 0.0-1.0  	| 0.0-1.0 	| 0.0-1.0 	|
		*
		*/

		enum IDX_BASE { X, Y, L, A, B, CONTRAST, ENTROPY, WEIGHT };

		std::string mVideoFileName;

		int mVideoID;

		int mClazz; //Need for evaluations, is the same clazz like in segments

		unsigned int mStartFrameNr;

		unsigned int mFrameCount;

		cv::Mat mVectors;

		FeatureSignatures(std::string mVideoFileName, int _videoID, int _clazz, unsigned int _startFrameNr, unsigned int _frameCount);

		FeatureSignatures() {};

		~FeatureSignatures() {};

		void write(cv::FileStorage& fs) const;
		void read(const cv::FileNode& node);
	};


	static void write(cv::FileStorage& fs, const std::string&, const std::vector<FeatureSignatures>& x)
	{
		fs << "[";
		for (unsigned int i = 0; i < x.size(); i++)
		{
			x.at(i).write(fs);
		}
		fs << "]";
	}

	static void write(cv::FileStorage& fs, const std::string&, const FeatureSignatures& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, FeatureSignatures& x, const FeatureSignatures& default_value)
	{
		if (node.empty())
			LOG_ERROR("AbstractFeature OpenCV read; default Value!");

		x.read(node);
	}
}
#endif
#endif