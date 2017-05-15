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
#include<boost/tokenizer.hpp>

static std::string prog = "MBSXtraction";
static std::string indexFile = "";
static std::string msbDirectory = "";
using boost::property_tree::ptree;

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
		("index-file", po::value<std::string>(&indexFile), "index file (XML)")
		("msb-directory", po::value<std::string>(&msbDirectory), "directory containing master shot boundary files")
		;

	po::positional_options_description p;
	p.add("index-file", -1);

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

	if (args.count("index-file"))
	{
		LOG_INFO( "Index files is: " << args["index-file"].as< std::string >());
	}


	if (args.count("msb-directory"))
	{
		LOG_INFO("MSB Directory is: " << args["msb-directory"].as< std::string >());
	}

}

VideoFileList readIndex (std::istream &is)
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

int main(int argc, char const *argv[]) {

	processProgramOptions(argc, argv);

	File* index = new File(indexFile);
	Directory* msb = new Directory(msbDirectory);

	std::ifstream is(index->getFile());
	if(!is.is_open())
	{
		exit(EXIT_FAILURE);
	}

	VideoFileList videolist = readIndex(is);
	is.close();

	int maxFiles = videolist.size();
	for(int iFile = 0; iFile < maxFiles; iFile++)
	{
		File videomsb(videolist.at(iFile).filename);
		videomsb.setPath(msb->getPath());
		videomsb.setFileExtension(".msb");

		is.open(videomsb.getFile());
		if (!is.is_open())
		{
			LOG_ERROR("MSB File for video : " << videolist.at(iFile).filename << " cannot be found with: " << videomsb.getFile());
		}else
		{
			MasterShotBoundaries shots = readMSB(is);
			if(shots.size() == 0)
			{
				LOG_INFO(videolist.at(iFile).filename << "has no shots");
			}
			videolist.at(iFile).msbs = shots;
		}
		is.close();

		cplusutil::Terminal::showProgress("Read in MSB Files: ", iFile, maxFiles);
	}
	

	delete index;
	delete msb;
	return EXIT_SUCCESS;
}
