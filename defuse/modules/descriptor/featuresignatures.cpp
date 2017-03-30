#include "featuresignatures.hpp"

defuse::FeatureSignatures::FeatureSignatures(std::string _mVideoFileName, int _videoID, int _clazz, unsigned _startFrameNr, unsigned _frameCount)
	:mVideoFileName(_mVideoFileName), mVideoID(_videoID), mClazz(_clazz), mStartFrameNr(_startFrameNr), mFrameCount(_frameCount)
{
	mVectors = cv::Mat();
}


void defuse::FeatureSignatures::write(cv::FileStorage& fs) const
{
	fs << "{"
		//<< "VideoFileName" << mVideoFileName
		<< "VideoID" << static_cast<int>(mVideoID)
		<< "StartFrameNr" << static_cast<int>(mStartFrameNr)
		<< "FrameCount" << static_cast<int>(mFrameCount)
		<< "Clazz" << static_cast<int>(mClazz)
		<< "Features" << mVectors
		<< "}";
}

void defuse::FeatureSignatures::read(const cv::FileNode& node)
{
	mVideoFileName = static_cast<std::string>(node["VideoFileName"]);
	mVideoID = static_cast<int>(node["VideoID"]);
	mStartFrameNr = static_cast<int>(node["StartFrameNr"]);
	mFrameCount = static_cast<int>(node["FrameCount"]);
	mClazz = static_cast<int>(node["Clazz"]);
	node["Features"] >> mVectors;
}
