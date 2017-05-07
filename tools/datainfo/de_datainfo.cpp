/** DeValuation (Version 2.0) ***************************
* ******************************************************
*       _    _      ()_()
*      | |  | |    |(o o)
*   ___| | _| | ooO--`o'--Ooo
*  / __| |/ / |/ _ \ __|_  /
*  \__ \   <| |  __/ |_ / /
*  |___/_|\_\_|\___|\__/___|
*
* ******************************************************
* @author skletz
* @version 1.0 21/03/17
*
**/

#include <cpluslogger.hpp>
#include <cplusutil.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <boost/filesystem.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>
#include "opencv2/imgcodecs/imgcodecs.hpp"

namespace boostfs = boost::filesystem;

typedef struct
{
	int value;
	std::string name;
	std::string description;

} Group;

typedef struct
{
	int id;
	int start;
	int end;
	Group clazz;

} Segment;

struct Video
{
	int id;
	std::string file;
	std::string name;
	std::vector<Segment> segments;

	int framecount;
	int fps;

	int frameheight;
	int framewidth;

	bool operator==(const Video& cmpObj) const {
		return id == cmpObj.id;
	}
};

static int id = 1;
static int getNextSegmentID()
{
	return id++;
}

/**
Generate segment files for each video entrie.

Created on: March 21, 2016
Author: Sabrina Kletz
*/
int main(int argc, char **argv)
{
	LOG_INFO("************************************************");
	LOG_INFO("DeDatainfo-v1.0");
	LOG_INFO("************************************************\n");

	std::string file;
	std::string conf;
	std::string videodir;
	std::string directory;
	bool splitVideoFiles;

	if (argc < 4)
	{
		std::cout << "Wrong usage!!!" << std::endl;
		std::cout << "Command line arguments: [1] Input-File [2] Output-Directory\n" << std::endl;
		std::cout << "Input-File contains lines, like:" << std::endl;
		std::cout << "\t/path/to/file/1.mp4\t\t1\t1131\t\t26" << std::endl;
		std::cout << "\t(Video-File)\t\t(Startframe)\t(Endframe)\t(Class)" << std::endl;

		//Default 
		char *clinput[] = { argv[0],
			".\\data\\classes_with_default.csv",
			".\\data\\segments",
			".\\data\\classes.conf.txt",
			"C:\\multimedia\\HORST\\videos",
			"true", NULL };

		std::cout << "Use default input for demonstration: " << std::endl;
		file = clinput[1];
		directory = clinput[2];
		conf = clinput[3];
		videodir = clinput[4];
		splitVideoFiles = (clinput[5] == "true" ? true : false);

	}
	else
	{
		file = argv[0];
		directory = argv[1];
		conf = argv[2];
		videodir = argv[3];
	}



	std::cout << "------------------------------->" << std::endl;
	std::cout << "Input File: " << file << std::endl;
	std::cout << "Output Directory: " << directory << std::endl;
	std::cout << "Config File: " << conf << std::endl;
	std::cout << "Video Directory: " << videodir << std::endl;
	std::cout << "Split video files: " << splitVideoFiles << std::endl;
	std::cout << "------------------------------->" << std::endl;


	boostfs::path inputfile{ file };
	boostfs::path outputdirectory{ directory };
	boostfs::path conffile{ conf };
	boostfs::path videodirectoy{ videodir };

	std::string filename = inputfile.filename().string();
	std::string videoname = inputfile.filename().stem().string();

	std::ifstream fstreaminputfile(inputfile.c_str());
	std::ifstream fstreamconffile(conffile.c_str());

	std::string line;
	std::string partial;

	std::vector<Video> videos;
	Segment segment;

	if (!fstreaminputfile.is_open())
	{
		std::cout << "Input-File does not exits. " << std::endl;
		std::cout << "Program aborted. Press any key to continue ... " << std::endl;
		getchar();
		return EXIT_FAILURE;
	}

	if (!fstreamconffile.is_open())
	{
		std::cout << "Config-File does not exits. " << std::endl;
		std::cout << "Program aborted. Press any key to continue ... " << std::endl;
		getchar();
		return EXIT_FAILURE;
	}

	std::map<int, Group> classes;

	//parse config file - each line describes one class
	while (getline(fstreamconffile, line))
	{
		std::vector<std::string> tokens;
		std::istringstream iss(line);
		std::string token;

		while (std::getline(iss, token, '\t'))
		{
			tokens.push_back(token);
		}

		std::string name = tokens.at(0);
		int clazznr = std::atoi(tokens.at(1).c_str());
		std::string describtion = tokens.at(2);

		Group clazz;
		clazz.name = name;
		clazz.value = clazznr;
		clazz.description = describtion;
		classes[clazznr] = clazz;
	}


	//parse the segment file
	//each line contain a segment of a video
	while (getline(fstreaminputfile, line))
	{
		std::vector<std::string> tokens;
		std::istringstream iss(line);
		std::string token;

		while (std::getline(iss, token, '\t'))
		{
			tokens.push_back(token);
		}

		boostfs::path videofile{ tokens.at(0) };

		Video video;

		video.file = videofile.filename().string();
		video.name = videofile.filename().stem().string();

		video.id = std::atoi(video.name.c_str());


		segment.start = std::atoi(tokens.at(1).c_str());
		segment.end = std::atoi(tokens.at(2).c_str());
		//segment.clazz = std::atoi(tokens.at(3).c_str());
		int group = std::atoi(tokens.at(3).c_str());
		segment.clazz = classes[group];

		//add the segment to the video
		if (!(std::find(videos.begin(), videos.end(), video) != videos.end()))
		{
			video.segments.push_back(segment);
			videos.push_back(video);
		}
		else
		{
			for (int i = 0; i < videos.size(); i++)
			{
				if (video.id == videos.at(i).id)
				{
					videos.at(i).segments.push_back(segment);
				}
			}
		}


		tokens.clear();
	}

	cpluslogger::Logger::get()->filelogging(true);
	cpluslogger::Logger::get()->logfile("stats.log");

	std::sort(videos.begin(), videos.end(), [](const Video& lhs, const Video& rhs)
	{
		return lhs.id < rhs.id;
	});

	int segmentcnt = 0;


	LOG_INFO("##############################################################");

	for (const auto i : videos)
	{
		segmentcnt += i.segments.size();
		//LOG_INFO(i.id << "\t" << i.file << "\t" << i.name << "\t" << i.segments.size() << "\t" << (i.framecount / double(i.fps)));
	}

	std::vector<std::pair<int, Segment>> segments;

	int seconds = 0;
	int segmentcntExclNonAnnotated = 0;

	double segmentlengths = 0;
	double segmentlengthMin = DBL_MAX;
	double segmentlengthMax = -1.0;

	LOG_INFO("ID" << "\t" << "File" << "\t" << "Name" << "\t" << "#Segments" << "#Length");
	for (int i = 0; i < videos.size(); i++)
	{
		cv::VideoCapture* cap = new cv::VideoCapture();
		boostfs::path p = videodirectoy / boostfs::path{ videos.at(i).file };
		cap->open(p.string());

		if (!cap->isOpened())
		{
			std::cout << "Video-File does not exits. " << std::endl;
			std::cout << "Program aborted. Press any key to continue ... " << std::endl;
			getchar();
			return EXIT_FAILURE;
		}

		videos.at(i).framecount = cap->get(CV_CAP_PROP_FRAME_COUNT);
		videos.at(i).fps = cap->get(CV_CAP_PROP_FPS);
		videos.at(i).framewidth = cap->get(CV_CAP_PROP_FRAME_WIDTH);
		videos.at(i).frameheight = cap->get(CV_CAP_PROP_FRAME_HEIGHT);








		seconds += (videos.at(i).framecount / double(videos.at(i).fps));
		LOG_INFO(videos.at(i).id << "\t" << videos.at(i).file << "\t" << videos.at(i).name << "\t" << videos.at(i).segments.size() << "\t" << (videos.at(i).framecount / double(videos.at(i).fps)));

		for (int j = 0; j < videos.at(i).segments.size(); j++)
		{
			std::pair<int, Segment> pair;

			pair.first = videos.at(i).segments.at(j).clazz.value;
			pair.second = videos.at(i).segments.at(j);

			segments.push_back(pair);

			if (videos.at(i).segments.at(j).clazz.value != 26)
				segmentcntExclNonAnnotated++;

			double tmp = (videos.at(i).segments.at(j).end - videos.at(i).segments.at(j).start);
			tmp = tmp / double(videos.at(i).fps);

			if (tmp > 7000)
			{
				LOG_INFO("##############################################################");
			}

			if (tmp < segmentlengthMin)
			{
				segmentlengthMin = tmp;
			}

			if (tmp > segmentlengthMax)
			{
				segmentlengthMax = tmp;
			}

			segmentlengths += (tmp / double(videos.at(i).fps));

			if (splitVideoFiles)
			{
				boostfs::path clazzPath(videodir);
				clazzPath = clazzPath / "examples_per_class";
				clazzPath = clazzPath / std::to_string(pair.first);

				if (!boostfs::exists(clazzPath))
					boostfs::create_directories(clazzPath);

				//int ex = static_cast<int>(cap->get(CV_CAP_PROP_FOURCC)); //get codec int
				//char EXT[] = { ex & 0XFF, (ex & 0XFF00) >> 8, (ex & 0XFF0000) >> 16, (ex & 0XFF000000) >> 24, 0 };

				std::string::size_type pAt = videos.at(i).file.find_last_of('.');

				videos.at(i).segments.at(j).id = getNextSegmentID();

				std::stringstream filename;
				filename << videos.at(i).name << "_" << videos.at(i).segments.at(j).id << "_" << videos.at(i).segments.at(j).clazz.value << ".avi";


				cv::VideoWriter *outputVideo = new cv::VideoWriter();
				cv::Size S = cv::Size(
					int(cap->get(CV_CAP_PROP_FRAME_WIDTH)),    //Acquire input size
					int(cap->get(CV_CAP_PROP_FRAME_HEIGHT))
				);

				std::string outputfile = (clazzPath / filename.str()).string();
				outputVideo->open(outputfile, CV_FOURCC('M', 'J', 'P', 'G'), cap->get(CV_CAP_PROP_FPS), S, true);

				if (!outputVideo->isOpened())
					LOG_ERROR("Video File: " << outputfile << " cannot be created!");


				cv::Mat src;

				//bool replicateimages = false;
				//if (videos.at(i).segments.at(j).end - videos.at(i).segments.at(j).start < cap->get(CV_CAP_PROP_FPS))
				//{
				//	replicateimages = true;

				//}

				for (int f = videos.at(i).segments.at(j).start; f < videos.at(i).segments.at(j).end; f++)
				{

					cap->set(CV_CAP_PROP_POS_FRAMES, f);
					cap->retrieve(src);
					outputVideo->write(src);

				}

				//if (replicateimages)
				//{
				//	
				//	int frames = cap->get(CV_CAP_PROP_FPS) - (videos.at(i).segments.at(j).end - videos.at(i).segments.at(j).start);

				//	for (int f = 0; f < frames*2; f++)
				//	{
				//		cap->set(CV_CAP_PROP_POS_FRAMES, videos.at(i).segments.at(j).end);
				//		cap->retrieve(src);
				//		outputVideo->write(src);
				//	}
				//}

				outputVideo->release();
				delete outputVideo;
			}
		}

		cap->release();
		delete cap;

	}

	std::map<int, std::vector<Segment>> segmentsGroupedByClazz;

	for (auto it = segments.begin(); it != segments.end(); ++it) {
		segmentsGroupedByClazz[(*it).first].push_back((*it).second);
	}

	int averageSegmentsInclNonAnnotated = 0;
	int averageSegments = 0;

	LOG_INFO("##############################################################");
	LOG_INFO("Class" << "\t" << "Number");
	for (auto it = segmentsGroupedByClazz.begin(); it != segmentsGroupedByClazz.end(); ++it)
	{
		int group = (*it).first;
		averageSegmentsInclNonAnnotated += std::vector<Segment>((*it).second).size();

		if (group != 26)
			averageSegments = averageSegmentsInclNonAnnotated;

		LOG_INFO((*it).first << "\t" << std::vector<Segment>((*it).second).size());
	}

	LOG_INFO("##############################################################");
	LOG_INFO("#Videos \t" << videos.size());
	LOG_INFO("#Segments incl. non annotated \t" << segmentcnt);
	LOG_INFO("#Segments excl. non annotated \t" << segmentcntExclNonAnnotated);
	LOG_INFO("#Segments per group incl. non annotated \t" << (averageSegmentsInclNonAnnotated / double(segmentsGroupedByClazz.size())));
	LOG_INFO("#Segments per group excl. non annotated \t" << (averageSegments / double(segmentsGroupedByClazz.size() - 1)));
	LOG_INFO("##############################################################");
	LOG_INFO("#Seconds \t" << seconds);
	LOG_INFO("#Minutes \t" << seconds / double(60));
	LOG_INFO("#Hours \t" << seconds / 60 / double(60));
	LOG_INFO("##############################################################");
	LOG_INFO("Avg length of Videos (s) \t" << (seconds / double(videos.size())));
	LOG_INFO("Avg length of Videos (m) \t" << (seconds / videos.size() / double(60)));
	LOG_INFO("Avg length of Videos (h) \t" << (seconds / videos.size() / 60 / double(60)));
	LOG_INFO("##############################################################");
	LOG_INFO("Avg length of Segments (s) \t" << (segmentlengths / double(segmentcnt)));
	LOG_INFO("Avg length of Segments (m) \t" << (segmentlengths / segmentcnt / double(60)));
	LOG_INFO("Avg length of Segments (h) \t" << (segmentlengths / segmentcnt / 60 / double(60)));
	LOG_INFO("Min length of Segments (s) \t" << segmentlengthMin);
	LOG_INFO("Max length of Segments (s) \t" << segmentlengthMax);

	for (int i = 0; i < videos.size(); i++)
	{
		boostfs::path file{ outputdirectory.c_str() };


		file = file / (videos.at(i).name + ".seg");

		Video element = videos.at(i);
		std::ofstream ofs(file.string());

		boostfs::path indexfile{ outputdirectory };
		indexfile = indexfile / "segments.idx";

		std::ofstream indexstream(indexfile.string());

		for (int j = 0; j < videos.at(i).segments.size(); j++)
		{
			Segment segment = element.segments.at(j);
			ofs << segment.start;
			ofs << "\t";
			ofs << segment.end;
			ofs << "\t";
			ofs << -1;
			ofs << "\t";
			ofs << segment.clazz.value;
			ofs << std::endl;


			indexstream << videos.at(i).id;
			indexstream << "\t";
			indexstream << element.segments.at(j).start;
			indexstream << "\t";
			indexstream << element.segments.at(j).end;
			indexstream << "\t";
			indexstream << std::endl;

		}

		ofs.close();
		indexstream.close();
	}

	if (boostfs::create_directory(outputdirectory))
	{
		std::cout << "Segment-Files were stored in '" << outputdirectory << std::endl;
	}


	return EXIT_SUCCESS;
}

