#ifndef _DEFUSE_FEATURES_HPP_
#define _DEFUSE_FEATURES_HPP_
#ifdef __cplusplus

#include <cpluslogger.h>
#include <vector>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/persistence.hpp>

namespace defuse {

	class Features
	{
	public:

		std::string mVideoFileName;

		int mVideoID;

		int mClazz;

		std::vector<int> mFeatureFrameNrs;

		int mStartFrameNr;

		int mFrameCount;

		cv::Mat mVectors;

		float mExtractionTime;

		void write(cv::FileStorage& fs) const;
		void read(const cv::FileNode& node);


		Features() {};
		~Features() {};

		Features(std::string mVideoFileName, int _videoID, int _clazz, int _startFrameNr, int _frameCount);
	};



	static void write(cv::FileStorage& fs, const std::string&, const std::vector<Features>& x)
	{
		fs << "[";
		for (unsigned int i = 0; i < x.size(); i++)
		{
			x.at(i).write(fs);
		}
		fs << "]";
	}

	static void write(cv::FileStorage& fs, const std::string&, const Features& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, Features& x, const Features& default_value)
	{
		if (node.empty())
			LOG_ERROR("AbstractFeature OpenCV read; default Value!");

		x.read(node);
	}
}
#endif
#endif