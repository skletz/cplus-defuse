#include "samplepoints.hpp"

#include <cpluslogger.hpp>

defuse::SamplePoints::SamplePoints(int _samplesCnt, Distribution _distribution)
{
	mSampleCnt = _samplesCnt;
	initSamplePoints(_distribution, mSampleCnt);
	//std::stringstream ss;
	//ss << "samplepoints_" << distributionToString.at(mDistribution) << "_" << mSampleCnt << ".yml";
	mSamplePointDirPath = cplusutil::FileIO::getCurrentWorkingDirectory();
	std::string sampleCnt = cplusutil::String::toStirng(mSampleCnt);
	mSamplePointFileName = cplusutil::String::concatCStrings("_", 3, "samplepoints", defuse::distributionToString.at(mDistribution), sampleCnt.c_str());
	mSamplePointFileName += ".yml";
}

void defuse::SamplePoints::initSamplePoints(Distribution distribution, int samplesCnt)
{
	mSampleCnt = samplesCnt;
	mDistribution = distribution;

	cv::RNG random;
	random.state = cv::getTickCount();
	mPoints.clear();
	mPoints.resize(samplesCnt);

	switch (distribution)
	{
	case RANDOM:
		for (int i = 0; i < samplesCnt; i++)
		{
			mPoints[i] = (cv::Point2f(random.uniform((float)0.0, (float)1.0), random.uniform((float)0.0, (float)1.0)));
		}
		break;
		//ADDED option for regular choosen keypoints
	case REGULAR:
	{
		float result = sqrt(samplesCnt);
		for (int i = 0; i < (int)result; i++)
		{
			for (int j = 0; j < (int)result; j++)
			{
				mPoints[i*result + j] = cv::Point2f((float)(i / result), (float)(j / result));
			}

		}
		break;
	}

	default:
		LOG_ERROR("Generation of this samplepoint distribution is not implemented!");
		break;
	}
}

int defuse::SamplePoints::getSampleCnt()
{
	return mSampleCnt;
}

std::vector<cv::Point2f> defuse::SamplePoints::getPoints()
{
	return mPoints;
}

defuse::SamplePoints::Distribution defuse::SamplePoints::getDistribution()
{
	return mDistribution;
}

std::string defuse::SamplePoints::getSamplePointFileName()
{
	return mSamplePointFileName;
}

bool defuse::SamplePoints::serialize(std::string destinationFile)
{
	cv::FileStorage fs(destinationFile, cv::FileStorage::WRITE);
	if (!fs.isOpened()) {
		LOG_ERROR("Fatal Error in serialization of SamplePoints: Could not generate File " << destinationFile);
		return false;
	}

	fs << "SamplePoints" << *this;
	fs.release();
	return true;
}

bool defuse::SamplePoints::deserialize(std::string sourceFile)
{
	cv::FileStorage fs(sourceFile, cv::FileStorage::READ);
	if (!fs.isOpened()) {
		LOG_FATAL("Fatal Error in deserialization of SamplePoints: Invalid file " << sourceFile);
		return false;
	}

	if (fs["SamplePoints"].empty())
	{
		LOG_FATAL("Fatal Error in deserialization of SamplePoints: File is empty " << sourceFile);
		return false;
	}

	mSamplePointDirPath = cplusutil::FileIO::getParentDirPath(sourceFile);
	mSamplePointFileName = mSamplePointDirPath.filename().string();
	fs["SamplePoints"] >> *this;
	fs.release();
	return true;
}

void defuse::SamplePoints::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "Distribution" << mDistribution
		<< "SamplesCnt" << static_cast<int>(mSampleCnt)
		<< "SamplePoints" << mPoints;
	fs << "}";

}

void defuse::SamplePoints::read(const cv::FileNode& node)
{
	int distributionEnum;
	node["Distribution"] >> distributionEnum;
	mDistribution = static_cast<Distribution>(distributionEnum);
	node["SamplesCnt"] >> mSampleCnt;
	node["SamplePoints"] >> mPoints;
}
