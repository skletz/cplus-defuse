#include "featuresignatures.hpp"

defuse::FeatureSignatures::FeatureSignatures()
{
	mVideoFileName = "";
	mVectors = cv::Mat();
}

defuse::FeatureSignatures::FeatureSignatures(std::string _filename)
	: Features(_filename)
{
	mVectors = cv::Mat();
}

defuse::FeatureSignatures::FeatureSignatures(std::string _filename, cv::Mat _features) 
	: Features(_filename, _features)
{

}

void defuse::FeatureSignatures::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "VideoFileName" << mVideoFileName
		<< "Features" << mVectors
		<< "}";
}

void defuse::FeatureSignatures::read(const cv::FileNode& node)
{
	mVideoFileName = static_cast<std::string>(node["VideoFileName"]);
	node["Features"] >> mVectors;
}
