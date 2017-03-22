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
* Purpose: Extract a specific descriptor from a set of videos or fram a specific video file
* Input/Output:
*	-arg1: Name of the dataset
*	-arg2: Path to a directory of extracted features ..\data\features\SFS1\SFS1_1000_random_10_5_2_0.01_0
*	-arg3: Path to the output directory ..\data\maps
*	-arg4: Type of Descriptor <INT>
*		0 = static feature signatures (variant 1)
*		1 = dynamic feature signatures (variant 1) //trajectories
*		2 = dynamic feature signatures (variant 2) //optical-flow
*		3 = motion histogram (variant 1)
*		4 = cnn (fc7 layer)
*	-arg5: Type of Distance <INT>
*		0 = minkowski distance
*		1 = signature matching distance
*		2 = signature quadratic form distance
*		3 = mahalanobis distance
*	-arg6: Evaluation Method
*		0 = mean average precision
*		1 = precision/recall@k
*		2 = random
*	-arg7: Path to CSV File AP Values ..\\data\\log-evaluation-maps.csv
*	-args > 8 depends on the type of the choosen distance, evaluation method
*	***********************************************************
*	TYPE: 0 = minkowski distance
*	***********************************************************
*	-args8: parameter of mk distance <INT>
*		1 = manahtten distance
*		2 = euclidian distance
*	***********************************************************
*	TYPE: 1 = signature matching distance
*	***********************************************************
*	-args8: Ground Distance <INT>
*		1 = manahtten distance
*		2 = euclidian distance
*	-arg9: Matching Strategy
*		0 = neares neighbor
*	-arg10: Direction
*		0 = bidirectional
*		1 = assymmetrisch to query
*		2 = assymetrisch to database
*	-arg11: Cost Function
*		0 = weighted distance
*	-arg12: Lambda <FLOAT>, is only used if bidirectional is choosen
*	***********************************************************
*	TYPE: 2 = signature quadratic form distance
*	***********************************************************
*	-args8: Ground Distance <INT>
*		1 = manahtten distance
*		2 = euclidian distance
*	-arg9: Similairty function
*		0 = heuristics
*	-arg10: Alpha <FLOAT>
*	***********************************************************
*	TYPE: 2 = mahalanobis distance
*	***********************************************************
* @author skletz
* @version 2.0 13/03/17
*
**/

#include <future>
#include <cpluslogger.h>
#include <cplusutil.h>
#include <defuse.hpp>
#include <random>
#include <iomanip>
#include <boost/format.hpp>
using namespace defuse;

static int distancemethod = 0;
static int xtractionmethod = 0;
static int evaluationmethod = 0;
static std::string distancemethodname = "signature matching distance";
static std::string distancemethodnameshort = "SMD1";

static std::string evlautionmethodname = "mean average precision";
static std::string evaluationmethodnameshort = "MAP";

static Parameter* paramter = new SFS1Parameter();

static std::string evalsettings = "";
static std::string xtractsettings = "";
static std::string featurepath = "..\\data\\features\\SFS1\\SFS1_1000_random_10_5_2_0.01_0";
static std::string outputpath = "..\\data\\maps\\";

static Directory* featuredir = nullptr;
static Directory* outputdir = nullptr;
static File* csvMAPValues = nullptr; 

static std::string dataset = "data_test-v1_0";
static bool ranodmazieTest = false;
static std::string csvFileMAPPath = "";
static bool appendValues = false;

//Used for directory as well as logfile name
std::string modelname = "";
//Output of the average precision values for statistical evaluation
std::string apcsvfile = "";

std::pair<EvaluatedQuery*, std::vector<ResultBase*>> compareRandom(
	Features& _query, std::vector<Features*> _model, Distance* measure, bool executiontime);

std::pair<EvaluatedQuery*, std::vector<ResultBase*>> compare(
	Features& _query, std::vector<Features*> _model, Distance* measure, bool executiontime);

bool serialize(std::pair<EvaluatedQuery*, std::vector<ResultBase*>> _results, Directory* _director);

bool writeAveragePrecisionValue(std::pair<EvaluatedQuery*, std::vector<ResultBase*>> _results, File* _file);

bool addValuesMAPToCSV(File* _file, std::vector<std::pair<int, float>> class_values, float average, std::string model);

Features* deserialize(std::string _file);

void evaluteResults(std::pair<EvaluatedQuery*, std::vector<ResultBase*>> result, int group);

double evaluateMeanAveragePrecision(std::map<int, std::vector<Features*>> groups, std::vector<Features*> model, Distance* distance, File* apstats, std::vector<std::pair<int, float>>& class_values);

std::mutex m;
std::unique_lock<std::mutex> f() {
	std::unique_lock<std::mutex> guard(m);
	return std::move(guard);
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		LOG_ERROR("valuation: too few arguments!");
	}
	else
	{
		dataset = argv[1];
		featurepath = argv[2];
		outputpath = argv[3];
		xtractionmethod = std::atoi(argv[4]);
		distancemethod = std::atoi(argv[5]);
		evaluationmethod = std::atoi(argv[6]);
		csvFileMAPPath = argv[7];

		if (distancemethod == 0)
		{
			distancemethodname = "Minkowski Distance";
			distancemethodnameshort = "MD";

			paramter = new MinkowskiParamter();
			static_cast<MinkowskiParamter *>(paramter)->distance = std::atoi(argv[8]);

			ranodmazieTest = std::atoi(argv[9]);

		}
		else if (distancemethod == 1)
		{
			distancemethodname = "Signature Matching Distance";
			distancemethodnameshort = "SMD";

			paramter = new SMDParamter();

			static_cast<SMDParamter *>(paramter)->grounddistance.distance = std::atoi(argv[8]);
			static_cast<SMDParamter *>(paramter)->matching = std::atoi(argv[9]);
			static_cast<SMDParamter *>(paramter)->direction = std::atoi(argv[10]);
			static_cast<SMDParamter *>(paramter)->cost = std::atoi(argv[11]);
			static_cast<SMDParamter *>(paramter)->lambda = std::stof(argv[12]);

			ranodmazieTest = std::atoi(argv[13]);

		}
		else if (distancemethod == 2)
		{
			distancemethodname = "Signature Quadratic Form Distance";
			distancemethodnameshort = "SQFD";

			paramter = new SQFDParamter();
			static_cast<SQFDParamter *>(paramter)->grounddistance.distance = std::atoi(argv[8]);
			static_cast<SQFDParamter *>(paramter)->similarity = std::atoi(argv[9]);
			static_cast<SQFDParamter *>(paramter)->alpha = std::stof(argv[10]);

			ranodmazieTest = std::atoi(argv[11]);

		}else if(distancemethod == 3)
		{
			distancemethodname = "Mahalanobis distance";
			distancemethodnameshort = "MHD";

			ranodmazieTest = std::atoi(argv[8]);
		}

		evalsettings = paramter->getFilename();
	}


	//Init output u. feature directory
	featuredir = new Directory(featurepath);
	outputdir = new Directory(outputpath);
	csvMAPValues = new File(csvFileMAPPath);

	//Logfile, init with current timestamp
	xtractsettings = featuredir->directories.at(featuredir->directories.size() - 2);
	std::string std2 = featuredir->directories.at(featuredir->directories.size() - 1);

	time_t now = time(0);
	static char name[20]; //is also the model name
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", localtime(&now));

	std::stringstream logfile;
	logfile << name << "_" << xtractsettings << "_" << std2 << "_" << evalsettings << "_Evaluation" << ".log";
	Directory workingdir(".");
	File log(workingdir.getPath(), logfile.str());
	log.addDirectoryToPath("logs");

	cpluslogger::Logger::get()->logfile(log.getFile());
	cpluslogger::Logger::get()->filelogging(true);
	cpluslogger::Logger::get()->perfmonitoring(true);

	LOG_INFO("************************************************");
	LOG_INFO("DeValuation-v2.0");
	LOG_INFO("Descriptor Type: " + xtractsettings);
	LOG_INFO("Distance Measure: " + distancemethodname);
	LOG_INFO("Evaluation Method: " + evaluationmethod);
	LOG_INFO("Input Directory Path: " << featurepath);
	LOG_INFO("Output Directory: " << outputpath);
	LOG_INFO("Logging: " << log.getFile().c_str());
	LOG_INFO(paramter->get());
	LOG_INFO("************************************************\n");

	//size_t start = cv::getTickCount();
	modelname = name;
	std::vector<std::string> files = cplusutil::FileIO::getFileListFromDirectory(featuredir->getPath());
	int numFeatures = 0;
	LOG_PERFMON(PINTERIM, "Number of Features\t" << files.size());

	std::vector<Features*> model;
	std::vector<std::pair<int, Features*>> queries;
	for (int iFile = 0; iFile < files.size(); iFile++)
	{
		std::string file = files.at(iFile);

		Features* features = deserialize(file);
		numFeatures++;
		if (features == nullptr)
		{
			LOG_ERROR("Fatal Error: File " << file << "cannot be deserialized.");
			LOG_ERROR("Evaluation is aborted!");
			return EXIT_FAILURE;
		}

		//Group features by Class Attribute
		std::pair<int, Features*> pair;
		pair.first = features->mClazz;
		pair.second = features;
		queries.push_back(pair);
		model.push_back(features);
	}

	//Group features by Class Attribute
	std::map<int, std::vector<Features*>> groups;

	for (auto it = queries.begin(); it != queries.end(); ++it) {
		groups[(*it).first].push_back((*it).second);
	}
	LOG_INFO("Number of classes found: " << groups.size());

	//Directory structure
	std::stringstream dir;
	dir << xtractsettings << "_" << std2 << "_" << evalsettings;
	outputdir->addDirectory(dir.str());
	LOG_DEBUG("Output Directory is initialized: " << dir.str());
	
	//Get the descriptor method for the exact position of the WEIGHTS
	int skipDim = -1;
	int idxWeight = -1;
	if (xtractionmethod == 0)
	{
		idxWeight = Xtractor::as_integer(SFS1Xtractor::IDX::WEIGHT);
		skipDim = -1;
	}
	else if (xtractionmethod == 1)
	{
		idxWeight = Xtractor::as_integer(DFS1Xtractor::IDX::WEIGHT);
		skipDim = -1;
	}
	else if (xtractionmethod == 2)
	{
		idxWeight = Xtractor::as_integer(DFS2Xtractor::IDX::WEIGHT);
		skipDim = -1;
	}
	else if (xtractionmethod == 3)
	{
		//MoHist has no weights
		idxWeight = -1;
		skipDim = -1;
	}
	else if (xtractionmethod == 4)
	{
		idxWeight = -1;
		LOG_INFO("Extraction method 4 is not implemented.")
	}

	//Initialize distance method for the processing of the parameters
	Distance *distance = nullptr;
	if (distancemethod == 0)
	{
		distance = new Minkowski(paramter);
	}
	else if (distancemethod == 1)
	{
		distance = new SMD(paramter, idxWeight, skipDim);
	}
	else if (distancemethod == 2)
	{
		distance = new SQFD(paramter, idxWeight, skipDim);
	}else if(distancemethod == 3)
	{
		distance = new Mahalanobis();
	}

	//Output of the average precision values for statistical evaluation
	//Distinguish between random ap values and calculated
	std::stringstream tmp;
	if (ranodmazieTest) {
		std::string n = "RANDOM";
		tmp << name << n << "_" << "Aps";
		modelname += ("_" + n);
	}
	else 
	{
		tmp << name << "_" << dir.str() << "_" << "Aps";
		modelname += ("_" + dir.str());
	}
	
	apcsvfile = tmp.str();

	//Init outputs
	Directory outputdirParent(outputpath);
	File* apstats = new File(outputdirParent.getPath(), apcsvfile, ".csv");

	std::vector<std::pair<int, float>> class_values;

	double map = evaluateMeanAveragePrecision(groups, model, distance, apstats, class_values);
	map = map / float(groups.size());

	size_t end = cv::getTickCount();
	//double elapsedTime = (end - start) / (cv::getTickFrequency() * 1.0f);
	LOG_PERFMON(PRESULT, "Mean of Mean Average Precision:" << "\tSize\t" << numFeatures << "\tModel\t" << xtractsettings << "\t" << evalsettings << "\t" "\tMAP\t" << map);
	//LOG_PERFMON(PTIME, "Execution Time of Evaluation (ms): \t" << elapsedTime * 1000.0);

	addValuesMAPToCSV(csvMAPValues, class_values, map, modelname);

	delete distance;
	delete paramter;
	delete featuredir;
	delete outputdir;
	delete csvMAPValues;
	delete apstats;

}

double evaluateMeanAveragePrecision(std::map<int, std::vector<Features*>> groups, std::vector<Features*> model, Distance* distance, File* apstats, std::vector<std::pair<int, float>>& class_values)
{
	//Output
	std::vector<std::pair<int, float>> tmp_values;

	double map = 0;
	//Parallel processing
	std::vector<std::future<std::pair<EvaluatedQuery*, std::vector<ResultBase*>>>> pendingFutures;

	//Only used if randomization is true
	std::vector<std::pair<EvaluatedQuery*, std::vector<ResultBase*>>> randomResults;

	//evalaute the mean average precision value for each group
	for (auto it = groups.begin(); it != groups.end(); ++it)
	{
		bool printexecutiontime = false; //multi-threaded, measure one value for the first entry of each group 
		std::vector<Features*> group = (*it).second;
		float mapPerGroup = 0;

		for (int iGroup = 0; iGroup < group.size(); iGroup++)
		{
			if (iGroup == 0)
				printexecutiontime = true;
			else
				printexecutiontime = false;

			Features element = *group.at(iGroup);
			if (!ranodmazieTest)
			{
				//pendingFutures.push_back(std::async(std::launch::async, &compare, element, model, distance, printexecutiontime));
				compare(element, model, distance, printexecutiontime);
			}
			else
			{
				randomResults.push_back(compareRandom(element, model, distance, printexecutiontime));
				//mapPerGroup += compareRandom(element, model, distance, printexecutiontime).first->mAPValue;
			}

		}

		int currentGroup = (*it).first;

		if (ranodmazieTest)
		{

			for (int iRandomResult = 0; iRandomResult < randomResults.size(); iRandomResult++)
			{
				mapPerGroup += randomResults.at(iRandomResult).first->mAPValue;
				writeAveragePrecisionValue(randomResults.at(iRandomResult), apstats);
			}

			LOG_INFO("Random Test Finished for group ... " << currentGroup);
			mapPerGroup = mapPerGroup / float(group.size());
			LOG_PERFMON(PINTERIM, "Mean Average Precision per Group: \t" << currentGroup << "\t" << mapPerGroup);

			tmp_values.push_back(std::make_pair(currentGroup, mapPerGroup));

			randomResults.clear();
			map = map + mapPerGroup;

		}
		else
		{
			std::unique_lock<std::mutex> guard(f());
			LOG_INFO("Group\t" << currentGroup << "\tSize:\t" << group.size());
			guard.unlock();
			for (int i = 0; i < pendingFutures.size(); i++)
			{
				std::pair<EvaluatedQuery*, std::vector<ResultBase*>> results = pendingFutures.at(i).get();
				mapPerGroup = mapPerGroup + results.first->mAPValue;

				serialize(results, outputdir);

				writeAveragePrecisionValue(results, apstats);
			}
			pendingFutures.clear();

			mapPerGroup = mapPerGroup / float(group.size());

			map = map + mapPerGroup;



			LOG_PERFMON(PINTERIM, "Mean Average Precision per Group: \t" << currentGroup << "\t" << mapPerGroup);
			tmp_values.push_back(std::make_pair(currentGroup, mapPerGroup));

		}

	}

	class_values.swap(tmp_values);
	return map;
}

std::pair<EvaluatedQuery*, std::vector<ResultBase*>> compareRandom(
	Features& _query, std::vector<Features*> _model, Distance* measure, bool executiontime)
{
	std::vector<ResultBase*> results;
	results.reserve(_model.size());

	float distance = 0.0;
	for (int iElem = 0; iElem < _model.size(); iElem++)
	{
		Features* element = _model.at(iElem);

		ResultBase* result = new ResultBase();
		result->mVideoID = element->mVideoID;
		result->mStartFrameNumber = element->mStartFrameNr;
		result->mFrameCount = element->mFrameCount;
		result->mClazz = element->mClazz;
		result->mDistance = distance;

		results.push_back(result);
	}

	srand(time(0));
	std::random_shuffle(results.begin(), results.end(), [](int n) { return rand() % n; });

	auto engine = std::default_random_engine{};
	std::shuffle(results.begin(), results.end(), engine);


	//for (int k = 0; k < results.size(); k++) {
	//	int r = k + rand() % (results.size() - k); // careful here!
	//	cv::swap(results[k], results[r]);
	//}

	int relevant = 0;

	float precisionAsSum = 0;

	float precision;

	int length = results.size();

	for (int iResult = 1; iResult < length + 1; iResult++)
	{

		ResultBase* rankedresult = results.at(iResult - 1);

		if (rankedresult->mClazz == _query.mClazz)
		{
			relevant++;
			precisionAsSum += (static_cast<float>(relevant) / static_cast<float>(iResult));
		}

		precision = ((static_cast<float>(relevant) / static_cast<float>(iResult)));

		rankedresult->mPrecision = precision;
	}

	float ap = (precisionAsSum / static_cast<float>(relevant));

	EvaluatedQuery* evalQuery = new EvaluatedQuery();
	evalQuery->mClazz = _query.mClazz;
	evalQuery->mAPValue = ap;
	evalQuery->mFrameCount = _query.mFrameCount;
	evalQuery->mStartFrameNumber = _query.mStartFrameNr;
	evalQuery->mVideoID = _query.mVideoID;

	std::pair<EvaluatedQuery*, std::vector<ResultBase*>> apresults;
	apresults.first = evalQuery;
	apresults.second = results;

	return apresults;

}



std::pair<EvaluatedQuery*, std::vector<ResultBase*>> compare(
	Features& _query, std::vector<Features*> _model, Distance* measure, bool executiontime)
{
	std::vector<ResultBase*> results;
	results.reserve(_model.size());

	float distance = 0.0;
	for (int iElem = 0; iElem < _model.size(); iElem++)
	{
		Features* element = _model.at(iElem);


		size_t e1_start = cv::getTickCount();
		distance = measure->compute(_query, *element);

		if (distance < 0)
		{
			LOG_ERROR("Error distance is small than zero " << distance);
		}

		size_t e1_end = cv::getTickCount();
		float elapsed_sec = (e1_end - e1_start / cv::getTickFrequency());

		std::unique_lock<std::mutex> guard(f());
		LOG_PERFMON(PTIME, "Computation-Time: Searching \t" << xtractsettings << "\t" << evalsettings << "\t" << elapsed_sec);
		guard.unlock();


		ResultBase* result = new ResultBase();
		result->mVideoID = element->mVideoID;
		result->mStartFrameNumber = element->mStartFrameNr;
		result->mFrameCount = element->mFrameCount;
		result->mClazz = element->mClazz;
		result->mDistance = distance;

		results.push_back(result);
	}

	std::sort(results.begin(), results.end(), [](const ResultBase* s1, const ResultBase* s2)
	{
		return (s1->mDistance < s2->mDistance);
	});

	size_t start = cv::getTickCount();
	int relevant = 0;

	float precisionAsSum = 0;

	float precision;

	int length = results.size();

	for (int iResult = 1; iResult < length + 1; iResult++)
	{

		ResultBase* rankedresult = results.at(iResult - 1);

		if (rankedresult->mClazz == _query.mClazz)
		{
			relevant++;
			precisionAsSum += (static_cast<float>(relevant) / static_cast<float>(iResult));
		}

		precision = ((static_cast<float>(relevant) / static_cast<float>(iResult)));

		rankedresult->mPrecision = precision;
	}

	float ap = (precisionAsSum / static_cast<float>(relevant));

	EvaluatedQuery* evalQuery = new EvaluatedQuery();
	evalQuery->mClazz = _query.mClazz;
	evalQuery->mAPValue = ap;
	evalQuery->mFrameCount = _query.mFrameCount;
	evalQuery->mStartFrameNumber = _query.mStartFrameNr;
	evalQuery->mVideoID = _query.mVideoID;

	std::pair<EvaluatedQuery*, std::vector<ResultBase*>> apresults;
	apresults.first = evalQuery;
	apresults.second = results;
	size_t end = cv::getTickCount();
	double elapsedTime = (end - start) / (cv::getTickFrequency() * 1.0f);

	return apresults;
}

bool serialize(std::pair<EvaluatedQuery*, std::vector<ResultBase*>> _results, Directory* _director)
{
	std::stringstream file;
	file << _results.first->mVideoID << "_" << _results.first->mStartFrameNumber;

	File* output = new File(_director->getPath(), file.str(), ".yml");
	cv::FileStorage fs(output->getFile(), cv::FileStorage::WRITE);

	if (!fs.isOpened()) {
		LOG_FATAL("Fatal Error in deserialization of Model: Invalid feature file " << file.str());
		return false;
	}

	fs << "Dataset" << dataset;
	fs << "Model" << xtractsettings;
	fs << "Distance" << evalsettings;
	fs << "VideoID" << static_cast<int>(_results.first->mVideoID);
	fs << "StartFrameNr" << static_cast<int>(_results.first->mStartFrameNumber);
	fs << "FrameCount" << static_cast<int>(_results.first->mFrameCount);
	fs << "Clazz" << static_cast<int>(_results.first->mClazz);
	fs << "AP" << static_cast<float>(_results.first->mAPValue);
	fs << "ResultCount" << static_cast<int>(_results.second.size());
	fs << "Data" << "[";
	for (int i = 0; i < _results.second.size(); i++)
	{
		_results.second.at(i)->write(fs);
	}

	fs << "]";
	fs.release();

	delete output;
	return true;
	return true;

}

bool writeAveragePrecisionValue(std::pair<EvaluatedQuery*, std::vector<ResultBase*>> _results, File* _file)
{

	std::ofstream outfile;

	outfile.open(_file->getFile(), std::ios_base::app);

	std::stringstream ss1, ss2;
	ss1 << static_cast<int>(_results.first->mVideoID) << ";" << static_cast<int>(_results.first->mStartFrameNumber) << ";" << static_cast<int>(_results.first->mClazz);
	ss2 << static_cast<int>(_results.first->mVideoID) << "_" << static_cast<int>(_results.first->mStartFrameNumber) << "_" << static_cast<int>(_results.first->mClazz);

	outfile << ss2.str() << ";" << ss1.str() << ";" << static_cast<float>(_results.first->mAPValue);
	outfile << std::endl;

	outfile.close();

	return true;
}

bool addValuesMAPToCSV(File* _file, std::vector<std::pair<int, float>> class_values, float average, std::string model)
{
	std::ifstream csvfileIn;
	csvfileIn.open(_file->getFile().c_str(), std::ios_base::in);

	if (!csvfileIn.is_open())
	{
		LOG_ERROR("Cannot open CSV File! :" << _file->getFile());
	}

	std::string line;

	csvfileIn >> line;
	csvfileIn.close();

	std::ofstream csvfileOut;
	csvfileOut.open(_file->getFile().c_str(), std::ios_base::app);

	std::vector<std::string> elems = cplusutil::String::split(line, ';');

	std::vector<std::string> newLine(elems);

	newLine.at(0) = model + ";";
	for (int i = 1; i < elems.size(); i++)
	{
		int elem = std::atoi(elems.at(i).c_str());

		for (int j = 0; j < class_values.size(); j++)
		{
			if (class_values.at(j).first == elem)
			{
				std::string value = std::to_string(class_values.at(j).second) + ";";
				newLine.at(i) = value;
				break;
			}

		}

	}
	newLine.push_back(std::to_string(average));

	for (int i = 0; i < newLine.size(); i++)
	{
		csvfileOut << newLine.at(i);
	}

	csvfileOut << std::endl;
	csvfileOut.close();
	return true;
}

Features* deserialize(std::string _file)
{
	cv::FileStorage fileStorage(_file, cv::FileStorage::READ);

	if (!fileStorage.isOpened()) {
		LOG_FATAL("Fatal Error in deserialization of Model: Invalid feature file " << _file);
		return nullptr;
	}

	cv::FileNode node = fileStorage["Data"];
	cv::FileNodeIterator it = node.begin(), it_end = node.end();

	Features* features = nullptr;

	for (; it != it_end; ++it)
	{
		if (xtractionmethod < 3)
		{
			features = new FeatureSignatures();
		}
		else if (xtractionmethod == 3)
		{
			features = new MotionHistogram();
		}

		(*features).read(*it);
	}

	fileStorage.release();
	return features;
}