/** DeDataconstruction (Version 1.0) ***************************
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
* @version 1.0 22/03/17
*
**/

#include <cpluslogger.h>
#include <cplusutil.h>
#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>


static std::string input = "..\\data-v1_1";
static std::string input2 = "..\\data-v1_1";
static std::string extension = ".mp4";
static std::string output = "..\\data-v1_2";
static int maxChunksize = 10;
static std::string dirs = "1,2,3,4,5,8,14,23,24,25";
static int filenameType = 1;

//static int skipDir = 311;
//static boolean different = false;
//static int useDir = 311;

std::vector<std::string> getRecursiveAllFilesFromDirectory(const std::string& path, std::string fileExtension)
{
	std::vector<std::string> mFileList;
	if (!path.empty())
	{
		namespace fs = boost::filesystem;

		fs::path apk_path(path);
		fs::recursive_directory_iterator end;

		for (fs::recursive_directory_iterator i(apk_path); i != end; ++i)
		{
			const fs::path cp = (*i);

			if (fs::is_regular_file(cp) && i->path().extension() == fileExtension)
			{
				mFileList.push_back(cp.string());
			}
		}
	}
	return mFileList;
}

void initFileLogging()
{
	time_t now = time(0);
	static char name[20];
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", localtime(&now));

	std::stringstream logfile;
	logfile << name << "_" << "dataconstruction" << ".log";
	Directory workingdir(".");
	File log(workingdir.getPath(), logfile.str());
	log.addDirectoryToPath("logs");
	cpluslogger::Logger::get()->logfile(log.getFile());
	cpluslogger::Logger::get()->filelogging(true);
	cpluslogger::Logger::get()->perfmonitoring(true);
}

void processFilenameType1(std::vector<std::string> entries, Directory tempdir3)
{
	for (int iEntry = 0; iEntry < entries.size(); iEntry++)
	{
		File file(entries.at(iEntry));
		file.setFileExtension(extension);
		File copiedfile(entries.at(iEntry));
		std::string chunk = file.getFilename();

		std::vector<std::string> file_framenr_chunknr = cplusutil::String::split(chunk, '-');
		//if the list is empty, the original file were copied, length was smaller than maxChunksize
		if (file_framenr_chunknr.size() == 1)
		{

			LOG_INFO("Current Filename " << chunk);
			std::stringstream ss;
			ss << chunk << "-0001";
			LOG_INFO("New Filename " << ss.str());

			copiedfile.setFilename(ss.str());
			copiedfile.setPath(tempdir3.getPath());
			copiedfile.setFileExtension(extension);

		}
		else
		{
			copiedfile.setFilename(chunk);
			copiedfile.setPath(tempdir3.getPath());
			copiedfile.setFileExtension(extension);

		}

		std::string path = tempdir3.getPath();

		LOG_INFO("Copy from  " << file.getFile());
		LOG_INFO("Copy to  " << copiedfile.getFile());
		boost::filesystem::copy_file(file.getFile(), copiedfile.getFile());
	}
}

void processFilenameType2(std::vector<std::string> entries, Directory tempdir3)
{
	for (int iEntry = 0; iEntry < entries.size(); iEntry++)
	{
		File file(entries.at(iEntry));
		file.setFileExtension(extension);
		File copiedfile(entries.at(iEntry));
		std::string chunk = file.getFilename();

		std::stringstream ss;
		if (chunk.find("case") != std::string::npos) {
			std::vector<std::string> parts = cplusutil::String::split(chunk, '_');
			//first and last entry are the class id, extension, framenumber
			for (int i = 1; i < parts.size() - 2; i++)
			{
				bool hasInteger = false;
				int integer = cplusutil::String::extractIntegerPart(parts.at(i), hasInteger);

				if (hasInteger)
					ss << "0" << integer;
			}

			std::vector<std::string> subparts = cplusutil::String::split(parts.at(6), '.');

			ss << subparts.at(0) << subparts.at(subparts.size() - 1) << "_";
			subparts = cplusutil::String::split(parts.at(parts.size() - 1), '.');
			ss << subparts.at(0) << "-0001";
			//ss <<file.getFileExtension();

			LOG_INFO("Current Filename " << chunk);
			LOG_INFO("New Filename " << ss.str());

		}else
		{
			std::vector<std::string> parts = cplusutil::String::split(chunk, '_');
			std::vector<std::string> subparts = cplusutil::String::split(parts.at(1), '.');

			
			ss << subparts.at(0);

			std::string substring = subparts.at(1).substr(3, subparts.at(1).find_last_of(".") - 1);
			ss << substring << file.getFileExtension();
			subparts = cplusutil::String::split(parts.at(parts.size()-1), '.');
			ss << "_" << subparts.at(0) << "-0001"; // << file.getFileExtension();
			LOG_INFO("Current Filename " << chunk);
			LOG_INFO("New Filename " << ss.str());

		}

		copiedfile.setFilename(ss.str());
		copiedfile.setPath(tempdir3.getPath());
		copiedfile.setFileExtension(extension);
		boost::filesystem::copy_file(file.getFile(), copiedfile.getFile());
	}
}

//Assuming that all video files are in subdirectories ..//chunks//ch_10//videoname//chunks.mp4
int main(int argc, char **argv)
{
	//Convert the output from chunk extractor into the right input format
	//*******************************************************************
	//Input: I:\SQUASH\datasets\data_mtap-v1_1\chunks\orig .mp4 I:\SQUASH\datasets\data_mtap-v1_2\orig\311 2 311 2
	//*******************************************************************
	//input = argv[1];
	//extension = argv[2];
	//output = argv[3];
	//maxChunksize = std::atoi(argv[4]);
	//dirs = argv[5];
	//filenameType = std::atoi(argv[6]); //currently 2 completely different filename types are implemented


	//Directory* inputdir = new Directory(input);
	//Directory* outputdir = new Directory(output);

	//LOG_INFO("Searching ...");
	//std::vector<std::string> entries;
	////entries = getRecursiveAllFilesFromDirectory(inputdir->getPath(), extension);
	//LOG_INFO("Found: " << entries.size() << " entries with " << extension << " in " << input);

	//std::vector<std::string> directories = cplusutil::FileIO::getFileListFromDirectory(inputdir->getPath());

	//Directory tempdir1(*outputdir);
	//tempdir1.addDirectory("chunks");

	//Directory tempdir2(tempdir1);
	//tempdir2.addDirectory("ch_" + std::to_string(maxChunksize));

	//std::vector<std::string> processDirs = cplusutil::String::split(dirs, ',');

	//for (int iDirectory = 0; iDirectory < directories.size(); iDirectory++)
	//{
	//	//the directory is a directory which contains the class number
	//	Directory iDir(directories.at(iDirectory));

	//	int clazz;
	//	{
	//		std::istringstream reader(iDir.mDirName);
	//		reader >> clazz;
	//	}

	//	entries = getRecursiveAllFilesFromDirectory(directories.at(iDirectory), extension);

	//	Directory tempdir3(tempdir2);
	//	tempdir3.addDirectory(std::to_string(clazz));

	//	if (std::find(processDirs.begin(), processDirs.end(), iDir.mDirName) != processDirs.end())
	//	{
	//		if(filenameType == 1)
	//		{
	//			processFilenameType1(entries, tempdir3);
	//		}else
	//		{
	//			processFilenameType2(entries, tempdir3);
	//		}
	//	}
	//}

	//Convert the output from cnn extractor into the right input format
	//*******************************************************************

	input = argv[1];
	extension = argv[2];
	output = argv[3];
	input2 = argv[4];

	Directory inputdir(input);
	Directory outputdir(output);
	Directory inputdir2(input2);

	std::vector<std::string> entries = cplusutil::FileIO::getFileListFromDirectory(inputdir.getPath());

	for (int iEntry = 0; iEntry < entries.size(); iEntry++)
	{
		std::string entry = entries.at(iEntry);

		boost::filesystem::path isClipFileDir{ entry };
		if (boost::filesystem::is_directory(isClipFileDir))
		{
			std::vector<std::string> files = cplusutil::FileIO::getFileListFromDirectory(entry);

			for (int iFile = 0; iFile < files.size(); iFile++)
			{
				File file(files.at(iFile));
				std::string filename = file.getFilename();
				std::string parentpath = files.at(iFile).substr(0, files.at(iFile).find_last_of("/\\"));
				Directory parentdirectory(parentpath);

				int clazz;
				{
					std::istringstream reader(parentdirectory.mDirName);
					reader >> clazz;
				}

				File copyToFile(files.at(iFile));
				//File copyFromFile(files.at(iFile));
				file.setPath(inputdir2.getPath());

				if(file.getFileExtension() == extension)
				{
					std::string tmpfilename = file.getFilename().substr(0, file.getFilename().find_last_of("."));
					tmpfilename += ".";
					tmpfilename += std::to_string(clazz);
					copyToFile.setFilename(tmpfilename);
					
					copyToFile.setPath(outputdir.getPath());
					copyToFile.setFileExtension(extension);

					LOG_INFO("Copy from  " << file.getFile());
					LOG_INFO("Copy to  " << copyToFile.getFile());
					boost::filesystem::copy_file(file.getFile(), copyToFile.getFile());

				}

			}
		}

	}


}
	