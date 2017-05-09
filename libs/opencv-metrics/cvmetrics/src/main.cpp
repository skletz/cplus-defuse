/** Signature Matching Distance (Version 1.0) **********
* ******************************************************
* Input/Output (Demo of Signature Matching Distance):
*	-arg1: Feature Signatures 1
*	-arg2: Feature Signatures 2
*	-arg3: Type of Minkowski Distance <INT> (1 = L1; 2 = L2)
*	-arg4: Direction <INT> (0 = bidirectional; 1 = asymmetric to query; 2 = asymmetric to database)
*	-arg5: Lambda <FLOAT> (is only used if bidirectional is chosen)
*	
*	Example: main.exe fs1.yml fs2.yml 1 0 1.0
*
* Theory:
*	-Beecks, C., Kirchhoff, S., & Seidl, T. (2013). Signature matching distance for content-based image retrieval. 
*	 Proceedings of the 3rd ACM Conference on International Conference on Multimedia Retrieval - ICMR ’13, 41.
*	-Beecks, C., Schoeffmann, K., Lux, M., Uysal, M. S., & Seidl, T. (2015). Endoscopic Video Retrieval: 
*	 A Signature-based Approach for Linking Endoscopic Images with Video Segments. 
*	 IEEE Proceedings of the International Symposium on Multimedia, 1–6.
* 
* @author skletz
* @version 2.0 25/04/17
*
**/

#include <iostream>
#include <string>
#include <opencv2/core/persistence.hpp>
#include <opencv2/core/core.hpp>
#include "similarity/smd.hpp"

using namespace defuse;

int main(int argc, char **argv)
{
	std::cout << "Signature Matching Distance Demo" << std::endl;


	if (argc < 3)
	{
		std::cout << "Demo of Signature Matching Distance - Too few arguments:" << std::endl;
		std::cout << "\t arg1: Feature Signatures 1" << std::endl;
		std::cout << "\t arg2: Feature Signatures 2" << std::endl;
		std::cout << "\t arg3: Type of Minkowski Distance <INT> (1 = L1; 2 = L2)" << std::endl;
		std::cout << "\t arg4: Direction <INT> (0 = bidirectional; 1 = asymmetric query; 2 = asymmetric db)" << std::endl;
		std::cout << "\t arg5: Lambda <FLOAT>" << std::endl;

		std::getchar();

		return EXIT_FAILURE;
	}

	std::string featureFile1 = argv[1];
	std::string featureFile2 = argv[2];

	cv::Mat features1, features2;

	//Read in the features file
	cv::FileStorage fileStorage;
	fileStorage.open(featureFile1, cv::FileStorage::READ);
	if (!fileStorage.isOpened())
	{
		std::cout << "YAML File could not be opened: " << featureFile1 << std::endl;
		std::getchar();
		return  EXIT_FAILURE;
	}
		
	fileStorage["Features"] >> features1;
	fileStorage.release();

	fileStorage.open(featureFile2, cv::FileStorage::READ);
	if (!fileStorage.isOpened())
	{
		std::cout << "YAML File could not be opened: " << featureFile2 << std::endl;
		std::getchar();
		return  EXIT_FAILURE;
	}

	fileStorage["Features"] >> features2;
	fileStorage.release();

	//Processes the SMD parameters
	int gddistanceparam = std::atoi(argv[3]);
	int direction = std::atoi(argv[4]);
	float lambda = std::stof(argv[5]);

	//Initializes ground distance
	Minkowski* gddistance = new Minkowski(gddistanceparam);

	SMD* smd = new SMD(gddistance, direction, lambda);

	float distance = smd->compute(features1, features2);

	std::cout << "Features 1 = " << features1 << std::endl;
	std::cout << std::endl;
	std::cout << "Features 2 = " << features2 << std::endl;
	std::cout << std::endl;
	std::cout << "Distance between 1 and 2: " << distance << std::endl;

	delete gddistance;
	delete smd;

	return EXIT_SUCCESS;
}
