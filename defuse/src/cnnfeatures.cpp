#include "cnnfeatures.hpp"

defuse::CNNFeatures::CNNFeatures(int _videoID, int _clazz, unsigned _startFrameNr, unsigned _frameCount)
	:mVideoID(_videoID), mClazz(_clazz), mStartFrameNr(_startFrameNr), mFrameCount(_frameCount)
{
	mVectors = cv::Mat();
}

defuse::CNNFeatures::CNNFeatures()
{
	mVectors = cv::Mat();
}

void defuse::CNNFeatures::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "VideoFileName" << mVideoFileName
		<< "VideoID" << static_cast<int>(mVideoID)
		<< "StartFrameNr" << static_cast<int>(mStartFrameNr)
		<< "FrameCount" << static_cast<int>(mFrameCount)
		<< "Clazz" << static_cast<int>(mClazz)
		<< "Features" << mVectors
		<< "}";
}

void defuse::CNNFeatures::read(const cv::FileNode& node)
{
	mVideoFileName = static_cast<std::string>(node["VideoFileName"]);
	mVideoID = static_cast<int>(node["VideoID"]);
	mStartFrameNr = static_cast<int>(node["StartFrameNr"]);
	mFrameCount = static_cast<int>(node["FrameCount"]);
	mClazz = static_cast<int>(node["Clazz"]);
	node["Features"] >> mVectors;
}

void defuse::CNNFeatures::readFloatArray(const std::string file)
{
	File f(file);
	std::string filename = f.getFilename();

	//Filename: 1.mp4_2151.mp4-400-1.csv
	//1.mp4_2151.jpg.csv
	std::vector<std::string> tmp = cplusutil::String::split(filename, '.');

	std::string tmp1 = tmp.at(0);
	std::string tmp2 = tmp.at(1);
	std::string tmp3 = tmp.at(2);

	mVideoFileName = f.getFilename();

	mVideoID = cplusutil::String::extractIntFromString(tmp1);

	tmp = cplusutil::String::split(tmp2, '_');
	mStartFrameNr = cplusutil::String::extractIntFromString(tmp.at(1));


	mClazz = cplusutil::String::extractIntFromString(tmp3);

	std::ifstream stream(file);
	std::vector<float> floats;

	if (!stream) {
		LOG_INFO("Cannot open file.");
	}

	float value = 0.0;
	std::stringstream strStream;
	//while (!stream.eof())
	//{
		

		strStream << stream.rdbuf();

		std::vector<std::string> values = cplusutil::String::split(strStream.str(), ';');

		//stream >> value;

		for(int i = 0; i < values.size(); i++)
		{
			floats.push_back(std::stof(values.at(i)));
		}

		
	//}

	//float* floatarray = &floats[0];

	//mVectors = cv::Mat(1, floats.size(), CV_32F, 0);

	//for (int i = 0; i < floats.size(); i++)
	//{
	//mVectors.at<float>(i, 0) = floats.at(i);
	//}

	mVectors = cv::Mat(1, floats.size(), CV_32F);
	memcpy(mVectors.data, floats.data(), floats.size() * sizeof(float));

	//cv::resize(mVectors, mVectors, cv::Size(floats.size(),1));
	//LOG_INFO("CNN Feature Values desirelized.");
}
