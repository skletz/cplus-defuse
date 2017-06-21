#ifndef _DEFUSE_FEATURES_HPP_
#define _DEFUSE_FEATURES_HPP_
#include <cpluslogger.hpp>
#include <vector>
#include <opencv2/core/core.hpp>

namespace defuse {

	class Features
	{
	public:

		Features();

		Features(std::string _filename);

		Features(std::string _filename, cv::Mat _features);

		Features(cv::Mat _features);

		~Features() {};

		std::string mVideoFileName;

		cv::Mat mVectors;

		float mExtractionTime;

		void write(cv::FileStorage& fs) const;

		void read(const cv::FileNode& node);

		void writeBinary(std::string _file) const;

		void readBinary(std::string _file);

	};

	static void write(cv::FileStorage& fs, const std::string&, const std::vector<Features>& x)
	{
		fs << "[";
		for (unsigned int i = 0; i < x.size(); i++)
		{
			x.at(i).write(fs);
		}
		fs << "]";
	}

	static void write(cv::FileStorage& fs, const std::string&, const Features& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, Features& x, const Features& default_value)
	{
		if (node.empty())
			LOG_ERROR("AbstractFeature OpenCV read; default Value!");

		x.read(node);
	}

	
}

#endif //_DEFUSE_FEATURES_HPP_
