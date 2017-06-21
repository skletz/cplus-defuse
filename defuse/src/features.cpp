#include "features.hpp"
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

defuse::Features::Features(std::string _filename)
{
	mVideoFileName = _filename;
	mExtractionTime = 0.0;
}

defuse::Features::Features(std::string _filename, cv::Mat _features)
{
	mVideoFileName = _filename;
	mVectors = _features;
	mExtractionTime = 0.0;
}

defuse::Features::Features(cv::Mat _features)
{
	mVideoFileName = "";
	mVectors = _features;
	mExtractionTime = 0.0;
}

defuse::Features::Features()
{
	mVideoFileName = "";

	mVectors = cv::Mat();

	mExtractionTime = 0.0;
}

void defuse::Features::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "VideoFileName" << mVideoFileName
		<< "Features" << mVectors
		<< "}";
}

void defuse::Features::read(const cv::FileNode& node)
{
	mVideoFileName = static_cast<std::string>(node["VideoFileName"]);
	node["Features"] >> mVectors;
}

void defuse::Features::writeBinary(std::string _file) const
{
	std::ofstream outstream(_file, std::ios_base::out);
	outstream.close();
	outstream.open(_file, std::ios::binary | std::ios::out);

	boost::archive::binary_oarchive oa(outstream);

	int cols, rows, type;
	cols = mVectors.cols; rows = mVectors.rows; type = mVectors.type();
	bool continuous = mVectors.isContinuous();

	oa & cols & rows & type & continuous;

	if (continuous) {
		const unsigned int data_size = rows * cols * mVectors.elemSize();
		oa & boost::serialization::make_array(mVectors.ptr(), data_size);
	}
	else {
		const unsigned int row_size = cols*mVectors.elemSize();
		for (int i = 0; i < rows; i++) {
			oa & boost::serialization::make_array(mVectors.ptr(i), row_size);
		}
	}
}

void defuse::Features::readBinary(std::string _file)
{
	std::ifstream instream(_file, std::ios::binary | std::ios::in);

	boost::archive::binary_iarchive ia(instream);

	cv::Mat vectors;
	int cols, rows, type;
	bool continuous;

	ia & cols & rows & type & continuous;

	vectors.create(rows, cols, type);

	const unsigned int row_size = cols*vectors.elemSize();
	for (int i = 0; i < rows; i++) {
		ia & boost::serialization::make_array(vectors.ptr(i), row_size);
	}

	mVectors = vectors;
}