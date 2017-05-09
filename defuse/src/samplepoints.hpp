#ifndef _DEFUSE_SAMPLEPOINTS_HPP_
#define _DEFUSE_SAMPLEPOINTS_HPP_
#ifdef __cplusplus

#include <cplusutil.hpp>

#include <boost/unordered_map.hpp>
#include <opencv2/core.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>

namespace defuse {
	namespace fs = boost::filesystem;
	namespace as = boost::assign;

	class SamplePoints
	{

	public:
		enum Distribution { RANDOM, REGULAR };

	private:
		Distribution mDistribution;
		int mSampleCnt;

		std::vector<cv::Point2f> mPoints;
		fs::path mSamplePointDirPath;
		std::string mSamplePointFileName;

	public:

		SamplePoints(int samplesCnt = 10);

		void initSamplePoints(Distribution distribution, int samplesCnt);

		int getSampleCnt();

		std::vector<cv::Point2f> getPoints();

		Distribution getDistribution();

		std::string getSamplePointFileName();

		bool serialize(std::string destinationFile);
		bool deserialize(std::string sourceFile);

		//OpenCV YAML (De)-Serialization
		void write(cv::FileStorage& fs) const;
		void read(const cv::FileNode& node);
	};

	const boost::unordered_map<SamplePoints::Distribution, const char*> distributionToString = as::map_list_of
	(SamplePoints::RANDOM, "random")
	(SamplePoints::REGULAR, "regular");

	static void write(cv::FileStorage& fs, const std::string&, const SamplePoints& x)
	{
            x.write(fs);
	}

	static void read(const cv::FileNode& node, SamplePoints& x, const SamplePoints& default_value = SamplePoints())
	{
		if (node.empty())
			x = default_value;
		else
                    x.read(node);

			
	}
}
#endif
#endif