#ifndef _DEFUSE_CNNFEATURES_HPP_
#define _DEFUSE_CNNFEATURES_HPP_


#ifdef __cplusplus

#include <cplusutil.h>
#include <cpluslogger.h>
#include <opencv2/opencv.hpp>
#include "../../features.hpp"

namespace defuse {

	class CNNFeatures : public Features
	{

	public:
		std::string mVideoFileName;

		int mVideoID;

		int mClazz; //Need for evaluations, is the same clazz like in segments

		unsigned int mStartFrameNr;

		unsigned int mFrameCount;

		cv::Mat mVectors;

		CNNFeatures(int _videoID, int _clazz, unsigned int _startFrameNr, unsigned int _frameCount);

		CNNFeatures();;

		~CNNFeatures() {};

		void write(cv::FileStorage& fs) const;
		void read(const cv::FileNode& node);

		void readFloatArray(const std::string file);

	};


}
#endif
#endif