/*
* PCT Signatures is an implementation of feature signatures.
* It was implemented with the focus to contribute to the
* OpenCV's extra modules. This module is a preliminary version
* which was received in 2015. In more detail, this module
* implements PCT (position-color-texture) signature extractor
* and SQFD (Signature Quadratic Form Distance).
*
* @author Gregor Kovalcik, Martin Krulis, Jakub Lokoc
* @repository https://github.com/GregorKovalcik/opencv_contrib
* @version 1.0 19/10/15
*/
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <string>
#include "pct_signatures.hpp"


using namespace std;
using namespace cv;
using namespace xfeatures2d;

#define DO_BENCHMARK 0


/** @brief

	Example of the PCTSignatures algorithm.

	The program has 2 modes:
	- single argument mode, where the program computes and visualizes the image signature
	- multiple argument mode, where the program compares the first image to the others
	  using signatures and signature quadratic form distance (SQFD)

	Enable the benchmark by defining DO_BENCHMARK preprocessor macro.
	This will duplicate the first image imageCount times
	and measures time of computing signatures and SQFD

*/
int main(int argc, char** argv)
{
	if (argc < 2)										// Check arguments
	{
		cout << "Example of the PCTSignatures algorithm." << endl;
		cout << "Usage: ./pct_signatures ImageToProcessAndDisplay" << endl;
		cout << "or:    ./pct_signatures ReferenceImage [ImagesToCompareWithTheReferenceImage]" << endl;
		cout << "The program has 2 modes:" << endl;
		cout << "- single argument: program computes and visualizes the image signature" << endl;
		cout << "- multiple argument: program compares the first image to the others" << endl;
		cout << "  using signatures and signature quadratic form distance (SQFD)" << endl;
		return -1;
	}

	Mat source;
	source = imread(argv[1], CV_LOAD_IMAGE_COLOR);		// Read the file

	if (!source.data)									// Check for invalid input
	{
		cout << "Could not open or find the image: " << argv[1] << std::endl;
		return -1;
	}


	// create the algorithm
	Mat signature, result;
	int initSampleCount = 5000;
	int initSeedCount = 70;
	vector<Point2f> initPoints;
	PCTSignatures::generateInitPoints(initPoints, initSampleCount, PCTSignatures::PointDistribution::RANDOM);
	Ptr<PCTSignatures> pctSignatures = PCTSignatures::create(initPoints, initSampleCount, initSeedCount);

	// compute the first image
	size_t start = getTickCount();
	pctSignatures->computeSignature(source, signature);
	size_t end = getTickCount();
	cout << "signature computed in " << (end - start) / (getTickFrequency() * 1.0f) << " seconds." << endl;

	vector<Mat> images;
	vector<Mat> signatures;
	vector<float> distances;


#ifdef DO_BENCHMARK // benchmark
	cout << "Benchmark ..." << endl;
	int imageCount = 100;
	for (int i = 0; i < imageCount; i++)
	{
		images.push_back(source);
	}
	cout << "computing " << imageCount << " signatures: " << endl;
	start = getTickCount();
	pctSignatures->computeSignatures(images, signatures);
	end = getTickCount();
	cout << "signatures time: " << (end - start) / (getTickFrequency() * 1.0f) << endl;

	cout << "computing sqfd: " << endl;
	start = getTickCount();
	PCTSignatures::computeQuadraticFormDistances(signature, signatures, distances);
	end = getTickCount();
	cout << "sqfd time: " << (end - start) / (getTickFrequency() * 1.0f) << endl;
	return 0;
#endif



	// single image -> compute and visualize
	if (argc == 2)
	{
		cout << "Single image: Compute and visualize" << endl;
		PCTSignatures::drawSignature(source, signature, result);
		namedWindow("Source", WINDOW_AUTOSIZE);		// Create windows for display.
		namedWindow("Result", WINDOW_AUTOSIZE);
		imshow("Source", source);					// Show our images inside them.
		imshow("Result", result);
		waitKey(0);		// Wait for user input
	}

	// multiple images -> compare to the first one
	// example: ../data/image0.jpg ../data/image1.jpg ../data/image2.jpg ../data/image3.jpg
	else
	{
		cout << "Multiple image: Compare to the first one" << endl;
		for (int i = 2; i < argc; i++)
		{
			Mat img = imread(argv[i], CV_LOAD_IMAGE_COLOR);
			if (!source.data)									// Check for invalid input
			{
				cout << "Could not open or find the image: " << argv[1] << std::endl;
				return -1;
			}
			images.push_back(img);
		}

		pctSignatures->computeSignatures(images, signatures);
		PCTSignatures::computeQuadraticFormDistances(signature, signatures, distances);

		for (int i = 0; i < distances.size(); i++)
		{
			cout << "Image: " << argv[i + 2] << ", similarity: " << distances[i] << endl;
		}
	}

	return 0;
}
