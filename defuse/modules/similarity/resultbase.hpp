#ifndef _DEFUSE_RESULTBASE_HPP_
#define _DEFUSE_RESULTBASE_HPP_
#ifdef __cplusplus

#include <cpluslogger.h>
#include <opencv2/core/persistence.hpp>
namespace defuse {

	class ResultBase
	{
	public:

		int mVideoID;

		int mClazz;

		unsigned int mStartFrameNumber;

		unsigned int mFrameCount;

		float mDistance;

		float mPrecision;

		ResultBase() {};

		~ResultBase() {};

		void write(cv::FileStorage& fs) const;
		void read(const cv::FileNode& node);
	};

	static void write(cv::FileStorage& fs, const std::string&, const std::vector<ResultBase>& x)
	{
		fs << "[";
		for (unsigned int i = 0; i < x.size(); i++)
		{
			x.at(i).write(fs);
		}
		fs << "]";
	}

	static void write(cv::FileStorage& fs, const std::string&, const ResultBase& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, ResultBase& x, const ResultBase& default_value)
	{
		if (node.empty())
			LOG_ERROR("AbstractFeature OpenCV read; default Value!");

		x.read(node);
	}

}
#endif
#endif