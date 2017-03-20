/** DeXtraction (Version 2.0) ***************************
 * ******************************************************
 *       _    _      ()_()     
 *      | |  | |    |(o o)             
 *   ___| | _| | ooO--`o'--Ooo
 *  / __| |/ / |/ _ \ __|_  /  
 *  \__ \   <| |  __/ |_ / /   
 *  |___/_|\_\_|\___|\__/___|  
 *                                                   
 * ******************************************************
 * Purpose: Extract a specific descriptor from a set of videos or fram a specific video file
 * Input/Output:
 *	-arg1: Name of the dataset
 *	-arg2: Path to a directory of video segments ..\\data\\segments OR to a specific video file ..\\data\\segments\\1\\test.mp4
 *	-arg3: Type of Input
 *		0 = Path to a specific video file
 *		1 = Path to a directory of video files*		
 *	-arg4: Path to the output directory ..\\data\\features
 *	-arg5: Type of Descriptor <INT>
 *		0 = static feature signatures (variant 1)
 *		1 = dynamic feature signatures (variant 1) //trajectories
 *		2 = dynamic feature signatures (variant 2) //optical-flow
 *		3 = motion histogram (variant 1) //direction only
 *		3 = motion histogram (variant 2) //direction incl. length
 *		4 = cnn (fc7 layer)
 *	-args > 5 depends on the type of the choosen descriptor 
 *		***********************************************************
 *		TYPE: 0 = static feature signatures variant 1
 *		***********************************************************
 *		-arg6: Number of Samplepoints
 *		-arg7: Distribution
 *			0 = RANDOM
 *			1 = REGULAR
 *			2 = GAUSSIAN
 *		-arg8: Number of initial centroids <INT>
 *		-arg9: Path to a directory of samplepoint files, if distribution is RANDOM ..\\data\\samplepoints
 *		-arg10: Number of iterations adjustable clustering - Default: 5 <INT>
 *		-arg11: Minimal weight adjustable clustering - Default: 2 <INT>
 *		-arg12: Minimal distance adjustable clustering - Default: 0.01 <FLOAT>
 *		-arg13: Keyframe selection
 *			0 = use middle frame of each segment
 *			1 = use first frame of each segment
 *			2 = use last frame of each segment
  *		***********************************************************
 *		TYPE: 1 = dynamic feature signatures variant 1 using trajectories
 *		***********************************************************
 *		-arg6: Number of Samplepoints
 *		-arg7: Distribution
 *			0 = RANDOM
 *			1 = REGULAR
 *			2 = GAUSSIAN
 *		-arg8: Number of initial centroids <INT>
 *		-arg9: Path to a directory of samplepoint files, if distribution is RANDOM ..\\data\\samplepoints
 *		-arg10: Number of iterations adjustable clustering - Default: 5 <INT>
 *		-arg11: Minimal weight adjustable clustering - Default: 2 <INT>
 *		-arg12: Minimal distance adjustable clustering - Default: 0.01 <FLOAT>
 *		-arg13: Frame selection
 *			0 = use fix number of frames per segment
 *			1 = use fix number per second
 *			2 = use all frames
 *		-arg14: Number of frames per segment or second, if keyframe selection type is 1,2 (fix number)			
 *		***********************************************************
 *		TYPE: 2 = dynamic feature signatures variant 2 using optical-flow
 *		***********************************************************
 *		-arg6: Number of Samplepoints
 *		-arg7: Distribution
 *			0 = RANDOM
 *			1 = REGULAR
 *			2 = GAUSSIAN
 *		-arg8: Number of initial centroids <INT>
 *		-arg9: Path to a directory of samplepoint files, if distribution is RANDOM ..\\data\\samplepoints
 *		-arg10: Number of iterations adjustable clustering - Default: 5 <INT>
 *		-arg11: Minimal weight adjustable clustering - Default: 2 <INT>
 *		-arg12: Minimal distance adjustable clustering - Default: 0.01 <FLOAT>
 *		-arg13: Frame selection
 *			0 = use fix number of frames per segment
 *			1 = use fix number per second
 *			2 = use all frames
 *		-arg14: Number of frames per segment or second, if keyframe selection type is 1,2 (fix number)
 *		***********************************************************
 *		TYPE: 3 = motion histogram variant 1
 *		***********************************************************
 *		-arg6: Number of samplex
 *		-arg7: Number of sampley
  *		-arg8: Frame selection
 *			0 = use fix number of frames per segment
 *			1 = use fix number per second
 *			2 = use all frames
 *		-arg9: Number of frames per segment or second, if keyframe selection type is 1,2 (fix number)		
 *		
 * @author skletz
 * @version 2.0 13/03/17
 * 
**/

#include <future>
#include <cpluslogger.h>
#include <cplusutil.h>
#include <defuse.hpp>

using namespace defuse;

//Default Settings
static int descriptortype = 0;
static std::string descriptorname = "static feature signatures variant 1";
static std::string descriptorshort = "SFS1";

static Parameter* paramter = new SFS1Parameter();
static std::string extractionsettings = ""; //is generated at runtime to use it for filenames, depends on the given settings

static std::string videopath = "..\\data\\segments\\";
static std::string outputpath = "..\\data\\features\\";
static std::string samplepointspath = "..\\data\\samplepoints\\";

static std::string dataset = "data_test-v1_0";
static boolean videopathIsDirectory = true;

static Directory* clipsdir = nullptr;
static File* videoFile = nullptr; //In the case of the input is only one video file
static Directory* outputdir = nullptr;

void processVideoDirectory();
void processVideoFile();

Features* xtract(VideoBase* _videobase, Parameter* _parameter);
bool serialize(Features* _features, File* _file);
Features* extractVideo(Directory* dir, File* videofile, Parameter* paramter);

static bool display = false;

int main(int argc, char **argv)
{

	if (argc < 3)
	{
		LOG_ERROR("xtraction: too few arguments!");
	}
	else
	{
		dataset = argv[1];
		videopathIsDirectory = std::atoi(argv[2]);
		videopath = argv[3];
		outputpath = argv[4];
		descriptortype = std::atoi(argv[5]);

		if (descriptortype == 0) //Default static feature signatures variant 1
		{
			static_cast<SFS1Parameter *>(paramter)->samplepoints = std::atoi(argv[6]);
			static_cast<SFS1Parameter *>(paramter)->distribution = std::atoi(argv[7]);
			static_cast<SFS1Parameter *>(paramter)->initialCentroids = std::atoi(argv[8]);
			static_cast<SFS1Parameter *>(paramter)->samplepointsfile = argv[9];
			static_cast<SFS1Parameter *>(paramter)->iterations = std::atoi(argv[10]);
			static_cast<SFS1Parameter *>(paramter)->minimalWeight = std::atoi(argv[11]);
			static_cast<SFS1Parameter *>(paramter)->minimalDistance = std::stof(argv[12]);
			static_cast<SFS1Parameter *>(paramter)->keyframeSelection = std::atoi(argv[13]);
			display = std::atoi(argv[14]);

		}
		else if (descriptortype == 1)
		{
			descriptorname = "dynamic feature signatures variant 1 using trajectories";
			descriptorshort = "DFS1";

			paramter = new DFS1Parameter();
			static_cast<DFS1Parameter *>(paramter)->staticparamter.samplepoints = std::atoi(argv[6]);
			static_cast<DFS1Parameter *>(paramter)->staticparamter.distribution = std::atoi(argv[7]);
			static_cast<DFS1Parameter *>(paramter)->staticparamter.initialCentroids = std::atoi(argv[8]);
			static_cast<DFS1Parameter *>(paramter)->staticparamter.samplepointsfile = argv[9];
			static_cast<DFS1Parameter *>(paramter)->staticparamter.iterations = std::atoi(argv[10]);
			static_cast<DFS1Parameter *>(paramter)->staticparamter.minimalWeight = std::atoi(argv[11]);
			static_cast<DFS1Parameter *>(paramter)->staticparamter.minimalDistance = std::stof(argv[12]);

			static_cast<DFS1Parameter *>(paramter)->frameSelection = std::atoi(argv[13]);
			static_cast<DFS1Parameter *>(paramter)->frames = std::atoi(argv[14]);
			display = std::atoi(argv[15]);
		}
		else if (descriptortype == 2)
		{
			descriptorname = "dynamic feature signatures variant 2 using optical-flow";
			descriptorshort = "DFS2";

			paramter = new DFS2Parameter();
			static_cast<DFS2Parameter *>(paramter)->staticparamter.samplepoints = std::atoi(argv[6]);
			static_cast<DFS2Parameter *>(paramter)->staticparamter.distribution = std::atoi(argv[7]);
			static_cast<DFS2Parameter *>(paramter)->staticparamter.initialCentroids = std::atoi(argv[8]);
			static_cast<DFS2Parameter *>(paramter)->staticparamter.samplepointsfile = argv[9];
			static_cast<DFS2Parameter *>(paramter)->staticparamter.iterations = std::atoi(argv[10]);
			static_cast<DFS2Parameter *>(paramter)->staticparamter.minimalWeight = std::atoi(argv[11]);
			static_cast<DFS2Parameter *>(paramter)->staticparamter.minimalDistance = std::stof(argv[12]);

			static_cast<DFS2Parameter *>(paramter)->frameSelection = std::atoi(argv[13]);
			static_cast<DFS2Parameter *>(paramter)->frames = std::atoi(argv[14]);
			display = std::atoi(argv[14]);
		}
		else if (descriptortype == 3)
		{
			descriptorname = "motion histogram variant 1";
			descriptorshort = "MoHist1";

			paramter = new MoHist1Parameter();
			static_cast<MoHist1Parameter *>(paramter)->samplex = std::atoi(argv[6]);
			static_cast<MoHist1Parameter *>(paramter)->sampley = std::atoi(argv[7]);
			
			static_cast<MoHist1Parameter *>(paramter)->frameSelection = std::atoi(argv[8]);
			static_cast<MoHist1Parameter *>(paramter)->frames = std::atoi(argv[9]);
			display = std::atoi(argv[10]);
		}

		//generate filename depending on the descriptor
		extractionsettings = paramter->getFilename();

	}

	
	//Init output directory
	outputdir = new Directory(outputpath);

	//Logfile, init with curren timestampt
	time_t now = time(0);
	static char name[20];
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", localtime(&now));

	std::stringstream logfile;
	logfile << name << "_" << extractionsettings << "_Extraction" << ".log";
	Directory workingdir(".");
	File log(workingdir.getPath(), logfile.str());
	log.addDirectoryToPath("logs");

	LOG_INFO("************************************************");
	LOG_INFO("DeXtraction-v2.0");
	LOG_INFO("Used Feature: " + descriptorname + "\n");
	LOG_INFO("Input Directory: " << videopath);
	LOG_INFO("Output Directory: " << outputpath);
	LOG_INFO("Logging: " << log.getFile().c_str());
	LOG_INFO(paramter->get());
	LOG_INFO("************************************************\n");


	if (videopathIsDirectory)
	{
		clipsdir = new Directory(videopath);
		cpluslogger::Logger::get()->logfile(log.getFile());
		cpluslogger::Logger::get()->filelogging(true);
		cpluslogger::Logger::get()->perfmonitoring(true);
		processVideoDirectory();

	}
	else
	{
		videoFile = new File(videopath);
		clipsdir = new Directory(videoFile->getFile().substr(0, videoFile->getFile().find_last_of("/\\")));
		processVideoFile();
	}


	delete paramter;
	delete outputdir;

	if (videoFile != nullptr) delete videoFile; //if the input is a directory, videoFile will never be initialized
	delete clipsdir;

	return EXIT_SUCCESS;
}

void processVideoDirectory()
{
	std::vector<std::string> dirs = cplusutil::FileIO::getFileListFromDirectory(clipsdir->getPath());
	int numClips = 0;
	LOG_PERFMON(PINTERIM, "Number of Categories\t" << dirs.size());
	for (int iDir = 0; iDir < dirs.size(); iDir++)
	{
		std::string dir = dirs.at(iDir);

		std::vector<std::string> clippathes = cplusutil::FileIO::getFileListFromDirectory(dir);

		std::vector<std::future<Features*>> pendingFutures;

		for (int iClipFile = 0; iClipFile < clippathes.size(); iClipFile++)
		{
			numClips += static_cast<int>(clippathes.size());

			File* file = new File(clippathes.at(iClipFile));
			std::string filename = file->getFilename();
			std::string parentpath = clippathes.at(iClipFile).substr(0, clippathes.at(iClipFile).find_last_of("/\\"));
			Directory parentdirectory(parentpath);

			std::vector<std::string> parts;
			cplusutil::String::split(filename, '_', parts);

			std::vector<std::string> videoparts;
			cplusutil::String::split(parts.at(0), '.', videoparts);

			int videoID = cplusutil::String::extractIntFromString(videoparts.at(0));

			unsigned int startFrameNr;
			{
				std::istringstream reader(parts.at(1));
				reader >> startFrameNr;
			}

			int clazz;
			{
				std::istringstream reader(parentdirectory.mDirName);
				reader >> clazz;
			}

			VideoBase* videoclip = new VideoBase(file, videoID, startFrameNr, clazz);
			//async Insufficient Memory exception - Solution - Change from 32-bit to 64-bit
			//pendingFutures.push_back(std::async(std::launch::async, &xtract, videoclip, paramter));
			xtract(videoclip, paramter);

		}

		LOG_DEBUG("Directoy\t" << dirs.at(iDir) << "\tSize:\t" << clippathes.size());
		size_t start = cv::getTickCount();
		LOG_DEBUG("Waiting of pending values ...");
		for (int i = 0; i < pendingFutures.size(); i++)
		{
			Features* features = pendingFutures.at(i).get();

			std::stringstream stream;
			if (descriptortype < 3)
			{
				stream << static_cast<FeatureSignatures *>(features)->mVideoID << "_" << static_cast<FeatureSignatures *>(features)->mStartFrameNr;
			}
			else if (descriptortype == 3)
			{
				stream << static_cast<MotionHistogram *>(features)->mVideoID << "_" << static_cast<MotionHistogram *>(features)->mStartFrameNr;
			}
			else
			{
				LOG_FATAL("Extraction Method not implemented " << descriptortype);
			}


			File* output = new File(outputdir->getPath(), stream.str(), ".yml");
			output->addDirectoryToPath(descriptorshort);
			output->addDirectoryToPath(extractionsettings);

			serialize(features, output);

			delete output;
			delete features;
		}




		LOG_DEBUG("End Calculation ..." << "\tDirectoy\t" << dirs.at(iDir) << "\tSize:\t" << clippathes.size());
		size_t end = cv::getTickCount();
		double elapsedTime = (end - start) / (cv::getTickFrequency() * 1.0f);

		LOG_PERFMON(PINTERIM, "Execution Time (FeatureSignatures of Segmetns of Category + Serialization): ""\tCategory\t" << dirs.at(iDir) << "\tSegment Count\t" << pendingFutures.size() << "\tElapsed Time\t" << elapsedTime);
	}

	LOG_PERFMON(PINTERIM, "Number of all Segments\t" << numClips);

	getchar();
}

void processVideoFile()
{
	Features* features = extractVideo(clipsdir, videoFile, paramter);
	std::stringstream stream;
	if (descriptortype < 3)
	{
		stream << static_cast<FeatureSignatures *>(features)->mVideoID << "_" << static_cast<FeatureSignatures *>(features)->mStartFrameNr;
	}
	else if (descriptortype == 3)
	{
		stream << static_cast<MotionHistogram *>(features)->mVideoID << "_" << static_cast<MotionHistogram *>(features)->mStartFrameNr;
	}
	else
	{
		LOG_FATAL("Extraction Method not implemented " << descriptortype);
	}


	File* output = new File(outputdir->getPath(), stream.str(), ".yml");
	output->addDirectoryToPath(descriptorshort);
	output->addDirectoryToPath(extractionsettings);

	serialize(features, output);

	delete output;
	delete features;
}

Features* xtract(VideoBase* _videobase, Parameter* _parameter)
{
	Xtractor* xtractor = nullptr;

	if (descriptortype == 0)
	{
		xtractor = new SFS1Xtractor(_parameter);
	}
	else if (descriptortype == 1)
	{
		xtractor = new DFS1Xtractor(_parameter);
	}
	else if (descriptortype == 2)
	{
		xtractor = new DFS2Xtractor(_parameter);
	}
	else if (descriptortype == 3)
	{
		xtractor = new MoHist1Xtractor(_parameter);
	}
	else
	{
		LOG_FATAL("Extraction Method not implemented " << descriptortype);
	}
	xtractor->display = display;

	Features* features = xtractor->xtract(_videobase);

	delete xtractor;
	return features;
}

bool serialize(Features* _features, File* _file)
{
	int featurecount = 1;
	cv::FileStorage fs(_file->getFile(), cv::FileStorage::WRITE);

	if (!fs.isOpened()) {
		LOG_ERROR("Error in serialization of Video: Invalid file " << "");
		return false;
	}

	fs << "Dataset" << dataset;
	fs << "Model" << extractionsettings;
	fs << "FeatureCount" << static_cast<int>(featurecount);
	fs << "Data" << "[";


	if (descriptortype < 3)
	{
		static_cast<FeatureSignatures *>(_features)->write(fs);
	}
	else if (descriptortype == 3)
	{
		static_cast<MotionHistogram *>(_features)->write(fs);
	}
	else
	{
		LOG_FATAL("Extraction Method not implemented " << descriptortype);
	}


	fs << "]";
	fs.release();

	return true;
}

Features* extractVideo(Directory* dir, File* videofile, Parameter* paramter)
{
	File* file = videofile;
	std::string filename = file->getFilename();

	std::vector<std::string> parts;
	cplusutil::String::split(videofile->getFilename(), '_', parts);

	std::vector<std::string> videoparts;
	cplusutil::String::split(parts.at(0), '.', videoparts);

	int videoID = cplusutil::String::extractIntFromString(videoparts.at(0));

	unsigned int startFrameNr;
	{
		std::istringstream reader(parts.at(1));
		reader >> startFrameNr;
	}

	int clazz;
	{
		std::istringstream reader(dir->mDirName);
		reader >> clazz;
	}

	VideoBase* videoclip = new VideoBase(file, videoID, startFrameNr, clazz);

	Features* feature = xtract(videoclip, paramter);

	return feature;
}
