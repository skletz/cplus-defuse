#include "resultbase.hpp"

void defuse::ResultBase::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "VideoFileName" << mVideoFileName
		<< "VideoID" << static_cast<int>(mVideoID)
		<< "ShotID" << static_cast<int>(mShotID)
		<< "QueryID" << static_cast<int>(mQueryID)
		<< "Distance" << static_cast<float>(mDistance)
		<< "Matches" << static_cast<float>(mPrecision)
		<< "}";
}

void defuse::ResultBase::read(const cv::FileNode& node)
{
	mVideoFileName = static_cast<std::string>(node["VideoFileName"]);
	mVideoID = static_cast<int>(node["VideoID"]);
	mShotID = static_cast<int>(node["ShotID"]);
	mQueryID = static_cast<int>(node["QueryID"]);
	mDistance = static_cast<float>(node["Distance"]);
	mPrecision = static_cast<float>(node["Matches"]);

}
