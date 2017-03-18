#include "motionhistogram.hpp"

defuse::MotionHistogram::MotionHistogram(int _videoID, int _clazz, unsigned _startFrameNr, unsigned _frameCount)
	:mVideoID(_videoID), mClazz(_clazz), mStartFrameNr(_startFrameNr), mFrameCount(_frameCount)
{
	mVectors = cv::Mat();
}

void defuse::MotionHistogram::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "VideoID" << static_cast<int>(mVideoID)
		<< "StartFrameNr" << static_cast<int>(mStartFrameNr)
		<< "FrameCount" << static_cast<int>(mFrameCount)
		<< "Clazz" << static_cast<int>(mClazz)
		<< "Features" << mVectors
		<< "}";
}

void defuse::MotionHistogram::read(const cv::FileNode& node)
{
	mVideoID = static_cast<int>(node["VideoID"]);
	mStartFrameNr = static_cast<int>(node["StartFrameNr"]);
	mFrameCount = static_cast<int>(node["FrameCount"]);
	mClazz = static_cast<int>(node["Clazz"]);
	node["Features"] >> mVectors;
}
