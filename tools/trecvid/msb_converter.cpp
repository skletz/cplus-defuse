/** MSBXtraction (Version 1.0) *************************
* ******************************************************
*       _    _      ()_()
*      | |  | |    |(o o)
*   ___| | _| | ooO--`o'--Ooo
*  / __| |/ / |/ _ \ __|_  /
*  \__ \   <| |  __/ |_ / /
*  |___/_|\_\_|\___|\__/___|
*
* ******************************************************
* Purpose: This tool is created for the data in TRECVid 2017 in order to extract the shots of the dataset using the master shot boundaries (msb).
* The boundaries are given via .cvs and the video files including its video id is given via one .xml file including all information of all video files.
* Therfore the overview file (.xml) is readed in and according to its information each msb file is read in and an
* Input/Output:

* @author skletz
* @version 1.0 12/05/17
*
**/

#include <cpluslogger.hpp>
#include <cplusutil.hpp>
#include <iostream>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <opencv2/opencv.hpp>

static std::string prog = "MSBConverter";
static std::string collectionFile = "";
static std::string msbDirectory = "";
static std::string outputDirectory = "";
using boost::property_tree::ptree;

int countFive = 0;
int countTen = 0;
int countTwenty = 0;
int countThirty = 0;
int sumShots = 0;
int counts[30] = { 0 };;



using namespace boost;
namespace po = program_options;

struct MasterShotBoundary
{
	unsigned int start;
	unsigned int end;
};

typedef std::vector<MasterShotBoundary> MasterShotBoundaries;

struct VideoFile
{
	int id;
	std::string filename;
	std::string use;
	std::string source;
	std::string filetype;
	MasterShotBoundaries msbs;
};

typedef std::vector<VideoFile> VideoFileList;

void showHelp(const po::options_description& desc)
{
	LOG_INFO(desc);
	exit(EXIT_SUCCESS);
}

void processProgramOptions(const int argc, const char *const argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Show brief usage message")
		("collection-file", po::value<std::string>(&collectionFile), "index file (XML)")
		("msb-directory", po::value<std::string>(&msbDirectory), "directory containing master shot boundary files")
		("output-directory", po::value<std::string>(&outputDirectory), "output directory")
		;

	po::positional_options_description p;
	p.add("collection-file", -1);

	if (argc < 2)
	{
		LOG_ERROR("For the execution of " << prog << " there are too few command line arguments!");
		showHelp(desc);
	}

	po::variables_map args;

	try
	{
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), args);
	}
	catch(po::error const& e)
	{
		LOG_ERROR(e.what());
		showHelp(desc);
		exit(EXIT_FAILURE);
	}

	po::notify(args);

	if (args.count("help")) {
		showHelp(desc);
	}

	if (args.count("collection-file"))
	{
		LOG_INFO( "Index files is: " << args["collection-file"].as< std::string >());
	}


	if (args.count("msb-directory"))
	{
		LOG_INFO("MSB Directory is: " << args["msb-directory"].as< std::string >());
	}

	if (args.count("output-directory"))
	{
		LOG_INFO("Output Directory is: " << args["output-directory"].as< std::string >());
	}

}

VideoFileList readCollection (std::istream &is)
{
	// populate tree structure pt
	ptree pt;
	read_xml(is, pt);

	// traverse pt
	VideoFileList ans;
	int counter = 0;
	int max = pt.get_child("VideoFileList").count("VideoFile");
	BOOST_FOREACH(ptree::value_type const& v, pt.get_child("VideoFileList")) {

		if (v.first == "VideoFile") {
			VideoFile f;
			f.id = v.second.get<int>("id");
			f.filename = v.second.get<std::string>("filename");
			f.use = v.second.get<std::string>("use");
			f.source = v.second.get<std::string>("source");
			f.filetype = v.second.get<std::string>("filetype");
			ans.push_back(f);
		}

		counter++;
		cplusutil::Terminal::showProgress("Read in Video Files: ", counter, max);

	}
	return ans;
}

MasterShotBoundaries readMSB(std::istream &is)
{
	std::string line;

	MasterShotBoundaries shots;

	int skipLines = 2;
	while (getline(is, line))
	{
		bool isInt = (line.find_first_not_of("0123456789") == std::string::npos);

		if (skipLines != 0)
		{
			skipLines--;
			continue;
		}

		std::vector<std::string> range;
		cplusutil::String::split(line, ' ', range);

		if(range.size() != 2)
		{
			LOG_ERROR("Shot boundaries are in the wrong format (only 2 entries with whitespace separated): " << line);
			continue;
		}

		MasterShotBoundary shot;
		shot.start = std::atoi(range.at(0).c_str());
		shot.end = std::atoi(range.at(1).c_str());

		shots.push_back(shot);
	}

	return shots;
}

void writeIndex(std::fstream &fs, VideoFile &videoFile)
{
	//cv::VideoCapture video("");

	for (int iShot = 0; iShot < videoFile.msbs.size(); iShot++)
	{
		int diff = videoFile.msbs.at(iShot).end - videoFile.msbs.at(iShot).start;

		if (diff < 30)
		{
			counts[diff]++;
		}

		if(diff == 0)
		{
			LOG_ERROR("One Frame Shot: ID\t" << videoFile.id << "; Source\t" << videoFile.source << "Shot Start: \t" << videoFile.msbs.at(iShot).start);
		}

		sumShots++;

		fs << videoFile.msbs.at(iShot).start;
		fs << ",";
		fs << videoFile.msbs.at(iShot).end;
		fs << '\n';
	}

	fs.flush();
}

VideoFileList processMSBoundaries(File* _collection, Directory* _msbs)
{
		std::ifstream is(_collection->getFile());
	if (!is.is_open())
	{
		exit(EXIT_FAILURE);
	}

	VideoFileList videolist = readCollection(is);
	is.close();

	int maxFiles = videolist.size();
	for (int iFile = 0; iFile < maxFiles; iFile++)
	{
		File videomsb(videolist.at(iFile).filename);
		videomsb.setPath(_msbs->getPath());
		videomsb.setFileExtension(".msb");

		is.open(videomsb.getFile());
		if (!is.is_open())
		{
			LOG_ERROR("MSB File for video : " << videolist.at(iFile).filename << " cannot be found with: " << videomsb.getFile());
		}
		else
		{
			MasterShotBoundaries shots = readMSB(is);
			if (shots.size() == 0)
			{
				LOG_INFO(videolist.at(iFile).filename << "has no shots");
			}

			videolist.at(iFile).msbs = shots;
		}
		is.close();

		cplusutil::Terminal::showProgress("Read MSB Files: ", iFile, maxFiles);
	}
	return videolist;
}

void processMSBIndex(VideoFileList& _videoFileList, Directory* _index)
{
	std::string idxFileName = "";
	std::fstream fs;
	File* idxFile;

	int maxFiles = _videoFileList.size();
	for (int iFile = 0; iFile < maxFiles; iFile++)
	{
		idxFileName = std::to_string(_videoFileList.at(iFile).id) + ".csv";
		idxFile = new File(idxFileName);
		idxFile->setPath(_index->getPath());

		fs.open(idxFile->getFile(), std::fstream::out);

		if (!fs.is_open())
		{
			LOG_ERROR("Index File for video : " << _videoFileList.at(iFile).filename << " cannot be created with: " << idxFile->getFile());
		}
		else
		{
			writeIndex(fs, _videoFileList.at(iFile));
		}

		cplusutil::Terminal::showProgress("Write Index File: ", iFile, maxFiles);
		fs.close();
		delete idxFile;

	}
}

int main(int argc, char const *argv[]) {

	processProgramOptions(argc, argv);

	std::stringstream logfile;
	logfile  << argv[0] << ".log";
	Directory workingdir(".");
	File log(workingdir.getPath(), logfile.str());
	cpluslogger::Logger::get()->logfile(log.getFile());
	cpluslogger::Logger::get()->filelogging(true);

	File* collection = new File(collectionFile);
	Directory* msb = new Directory(msbDirectory);
	Directory* index = new Directory(outputDirectory);


	VideoFileList videoFileList = processMSBoundaries(collection, msb);
	processMSBIndex(videoFileList, index);


	delete collection;
	delete msb;
	float mult = 100.0f / float(sumShots);

	float lessThan30Frames = 0;
	int totalCounts = 0;
	for(int iCount = 0; iCount < 30; iCount++)
	{
		LOG_INFO("Average frames per shot:"
			<< "\t" << iCount+1
			<< "\t" << "Frames"
			<< "\t" << counts[iCount]
			<< "\t" << counts[iCount]*mult
			<< "\t" << "%");
		lessThan30Frames += (counts[iCount] * mult);
		totalCounts += counts[iCount];
	}

	LOG_INFO("Average sum with less than 30 frames per shot:"
		<< "\t" << "<=30"
		<< "\t\t" << lessThan30Frames
		<< "\t" << "%");

	LOG_INFO("Sum of shots less than 30 frames per shot:"
		<< "\t" << "<=30"
		<< "\t"
		<< "\t" << totalCounts);
	LOG_INFO("Total sum of shots:"
		<< "\t\t"
		<< "\t" << sumShots);

	return EXIT_SUCCESS;
}
