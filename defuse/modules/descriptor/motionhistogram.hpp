#ifndef _DEFUSE_MOTIONHISTOGRAM_HPP_
#define _DEFUSE_MOTIONHISTOGRAM_HPP_
#ifdef __cplusplus

#include <cplusutil.h>
#include <cpluslogger.h>
#include <opencv2/opencv.hpp>
#include "../features.hpp"

namespace defuse {

	class MotionHistogram : public Features
	{
		/*
		bin 0: none            (ZERO)
		bin 1: right           (R)          -15 - 15 degree (345 - 15)
		bin 2: right up_       (RU_)        15 - 45 degree
		bin 3: right up        (RU)         45 - 75 degree
		bin 4: up              (U)          75 - 105 degree
		bin 5: left up         (LU)         105 - 135 degree
		bin 6: left up_        (LU_)        135 - 165 degree
		bin 7: left            (L)          165 - 195 degree
		bin 8: left down       (LD)         195 - 225 degree
		bin 9: left down_      (LD_)        225 - 255 degree
		bin 10: down           (D)          255 - 285 degree
		bin 11: right down_    (RD_)        285 - 315 degree
		bin 12: right down     (RD)         315 - 345 degree
		*/

	public:

		enum IDX { ZERO, R, RU_L, RU, U, LU, LU_, L, LD, LD_, D, RD, RD_ };

		int mVideoID;

		int mClazz; //Need for evaluations, is the same clazz like in segments

		unsigned int mStartFrameNr;

		unsigned int mFrameCount;

		cv::Mat mVectors;

		MotionHistogram(int _videoID, int _clazz, unsigned int _startFrameNr, unsigned int _frameCount);

		MotionHistogram() {};

		~MotionHistogram() {};

		void write(cv::FileStorage& fs) const;
		void read(const cv::FileNode& node);
	};

	static void write(cv::FileStorage& fs, const std::string&, const std::vector<MotionHistogram>& x)
	{
		fs << "[";
		for (unsigned int i = 0; i < x.size(); i++)
		{
			x.at(i).write(fs);
		}
		fs << "]";
	}

	static void write(cv::FileStorage& fs, const std::string&, const MotionHistogram& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, MotionHistogram& x, const MotionHistogram& default_value)
	{
		if (node.empty())
			LOG_ERROR("AbstractFeature OpenCV read; default Value!");

		x.read(node);
	}
}
#endif
#endif