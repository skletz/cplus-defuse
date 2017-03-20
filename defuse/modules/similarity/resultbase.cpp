#include "resultbase.hpp"

void defuse::ResultBase::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "VideoID" << static_cast<int>(mVideoID)
		<< "StartFrameNr" << static_cast<int>(mStartFrameNumber)
		<< "FrameCount" << static_cast<int>(mFrameCount)
		<< "Clazz" << static_cast<int>(mClazz)
		<< "Distance" << static_cast<float>(mDistance)
		<< "Precision" << static_cast<float>(mPrecision)
		<< "}";
}

void defuse::ResultBase::read(const cv::FileNode& node)
{
	mVideoID = static_cast<int>(node["VideoID"]);
	mStartFrameNumber = static_cast<int>(node["StartFrameNr"]);
	mFrameCount = static_cast<int>(node["FrameCount"]);
	mClazz = static_cast<int>(node["Clazz"]);
	mDistance = static_cast<float>(node["Distance"]);
	mPrecision = static_cast<float>(node["Precision"]);

}
