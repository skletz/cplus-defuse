#include "dfs2xtractor.hpp"
#include "dfs2parameter.hpp"
#include <cpluslogger.h>
#include "featuresignatures.hpp"

#define _USE_MATH_DEFINES 
#include <math.h>

defuse::DFS2Xtractor::DFS2Xtractor(Parameter* _parameter)
{
	int samples = dynamic_cast<DFS2Parameter *>(_parameter)->staticparamter.samplepoints;

	Directory samplepointdir(dynamic_cast<DFS2Parameter *>(_parameter)->staticparamter.samplepointsfile);
	SamplePoints* samplePoints = new SamplePoints(samples);
	File sampleFile(samplepointdir.getPath(), samplePoints->getSamplePointFileName());

	bool status;
	if (!cplusutil::FileIO::isValidPathToFile(sampleFile.getFile()))
	{
		status = samplePoints->serialize(sampleFile.getFile());
		LOG_INFO("Samplepoint File: " << sampleFile.getFilename() << " does not exists. A new one will be created.");
	}
	else
	{
		status = samplePoints->deserialize(sampleFile.getFile());
		int desirelizedSize = samplePoints->getPoints().size();
		if (samples != desirelizedSize)
		{
			LOG_FATAL("Fatal error: Sample count: " << samples << " do not match with desirelized samplepoints: " << desirelizedSize);
		}
	}

	mSamplepoints = samplePoints;
	mMaxCluster = dynamic_cast<DFS2Parameter *>(_parameter)->staticparamter.initialCentroids;
	mMaxFrames = dynamic_cast<DFS2Parameter *>(_parameter)->frames;
	mTPCTSignatures = new tpct::TPCTSignatures();

	mPCTSignatures = pct::PCTSignatures::create(mSamplepoints->getPoints(), mSamplepoints->getSampleCnt(), mMaxCluster);
}

defuse::Features* defuse::DFS2Xtractor::xtract(VideoBase* _videobase)
{
	cv::VideoCapture video(_videobase->mFile->getFile());

	if (!video.isOpened())
	{
		LOG_FATAL("Fatal error: Video File does not exists." << _videobase->mFile->getFile());
	}

	unsigned int framecount = video.get(CV_CAP_PROP_FRAME_COUNT);

	int SAMPLEX = 8, SAMPLEY = 8;

	const float L_COLOR_RANGE = 100;
	const float A_COLOR_RANGE = 127;
	const float B_COLOR_RANGE = 127;


	std::vector<cv::Point2f> initPoints;
	std::vector<cv::Point2f> corners;
	std::vector<cv::Point2f> prevCorners;

	//every mMaxFrames'th frame is used
	cv::Mat gray, prevGray, image, frame;


	cv::Mat tsamples, backupSamples;
	std::vector<cv::Mat> temporalsamples;
	for (int iFrame = 0; iFrame < framecount; iFrame = iFrame + mMaxFrames)
	{
		cv::Mat samples;
		samples.create(mSamplepoints->getPoints().size(), as_integer(IDX::DIMS), CV_32F);
		video.set(CV_CAP_PROP_POS_FRAMES, iFrame);
		video.grab();
		video.retrieve(frame);

		if (frame.empty())
			continue;

		float maxWH = MAX(frame.cols, frame.rows);

		int samplecountPerFrame = mSamplepoints->getPoints().size();
		initPoints.clear();
		initPoints.resize(samplecountPerFrame);

		float result = std::sqrt(samplecountPerFrame);
		for (int i = 0; i < int(result); i++)
		{
			for (int j = 0; j < int(result); j++)
			{
				initPoints[i*result + j] = cv::Point2f(float(i / result), float(j / result));
			}
		}

		corners.clear();
		for (std::size_t iSample = 0; iSample < initPoints.size(); iSample++)
		{
			std::size_t x = std::size_t(initPoints[iSample].x * (frame.cols - 1) + 0.5);
			std::size_t y = std::size_t(initPoints[iSample].y * (frame.rows - 1) + 0.5);

			corners.push_back(cv::Point(x, y));

			samples.at<float>(iSample, as_integer(IDX::X)) = float(double(x) / double(frame.cols));	// x, y normalized
			samples.at<float>(iSample, as_integer(IDX::Y)) = float(double(y) / double(frame.rows));

			float t = float(double(iFrame) / double(framecount));;
			samples.at<float>(iSample, as_integer(IDX::T)) = t; //t normalized

			cv::Mat rgbPixel(frame, cv::Rect(x, y, 1, 1));			// get Lab pixel color
			cv::Mat labPixel;										// placeholder -> TODO: optimize
			rgbPixel.convertTo(rgbPixel, CV_32FC3, 1.0 / 255);		//scale it down
			cv::cvtColor(rgbPixel, labPixel, cv::COLOR_BGR2Lab);
			cv::Vec3f labColor = labPixel.at<cv::Vec3f>(0, 0);		// end

			samples.at<float>(iSample, as_integer(IDX::L)) = float(std::floor(labColor[0] + 0.5) / float(L_COLOR_RANGE));	// Lab color normalized
			samples.at<float>(iSample, as_integer(IDX::A)) = float(std::floor(labColor[1] + 0.5 + A_COLOR_RANGE) / float((A_COLOR_RANGE * 2) + 1));
			samples.at<float>(iSample, as_integer(IDX::B)) = float(std::floor(labColor[2] + 0.5 + B_COLOR_RANGE) / float((B_COLOR_RANGE * 2) + 1));

			samples.at<float>(iSample, as_integer(IDX::MD)) = 0;
			samples.at<float>(iSample, as_integer(IDX::MI)) = 0;
		}

		frame.copyTo(image);
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

		std::vector<cv::Point2f> cornersCopy = std::vector<cv::Point2f>(corners);
		if (iFrame > 0)
		{
			std::vector<uchar> status;
			std::vector<float> error;

			//try{
			cv::calcOpticalFlowPyrLK(prevGray, gray, prevCorners, corners, status, error);
			//}
			//catch (std::exception& e)
			//{
			//	LOG_ERROR("An exception occurred during calcOpticalFlowPyrLK: " << e.what());
			//	LOG_ERROR("Optical Flow ended with framenumber " << iFrame << " of" << framecount);
			//	LOG_ERROR("Optical Flow ended with minute " << (iFrame / 25 / 60) << " of" << (framecount / 25 / 60));
			//	break;
			//}

			for (int iStatus = 0; iStatus < status.size(); iStatus++)
			{
				if (status[iStatus] == 0)
				{
					continue;
				}

				cv::Point2f pA = prevCorners[iStatus];
				cv::Point2f pB = corners[iStatus];

				float mvX = pA.x - pB.x;
				float mvY = pA.y - pB.y;

				float powDist = pow(mvX, 2.0) + pow(mvY, 2.0);
				float len = 0;
				if (powDist > 0)
					len = float(sqrt(powDist));

				int direction;

				mvX = -mvX;
				/* IMPORTANT NOTE: as Y is already vice-versa-scale,
				we cannot invert it (e.g. mvX=10, mvY=10 means we
				have prediction 45 degree left up (and not right up
				as in usual mathematics coordinates!) */

				float angle = 0;
				if ((mvX < 0.000001 && mvY < 0.000001) || len < 0.000001)
				{
					direction = 0;
				}
				else
				{
					if (mvX == 0)
					{
						if (mvY >= 0)
						{
							direction = 4;
						}
						else
						{
							direction = 10;
						}

					}
					else
					{
						//TODO: CHECK CODE FOR MVY == 0 ??? (no difference for +/- mvX??)


						angle = (float)(atan(mvY / mvX) * 180 / M_PI); //angle in degree

																	   /* quadrants:
																	   *  2 | 1
																	   *  --+--
																	   *  3 | 4
																	   */

						direction = 0;
						if (mvX >= 0 && mvY >= 0) //1st quandrant
						{
							if (angle >= 0 && angle < 15)
								direction = 1;
							else if (angle >= 15 && angle < 45)
								direction = 2;
							else if (angle >= 45 && angle < 75)
								direction = 3;
							else
								direction = 4;
						}
						else if (mvX < 0 && mvY >= 0) //2nd quandrant
						{
							if (angle <= 0 && angle > -15)
								direction = 7;
							else if (angle <= -15 && angle > -45)
								direction = 6;
							else if (angle <= -45 && angle > -75)
								direction = 5;
							else
								direction = 4;
						}
						else if (mvX < 0 && mvY < 0) //3rd quandrant
						{
							if (angle >= 0 && angle < 15)
								direction = 7;
							else if (angle >= 15 && angle < 45)
								direction = 8;
							else if (angle >= 45 && angle < 75)
								direction = 9;
							else
								direction = 10;
						}
						else //4th quadrant
						{
							if (angle <= 0 && angle > -15)
								direction = 1;
							else if (angle <= -15 && angle > -45)
								direction = 12;
							else if (angle <= -45 && angle > -75)
								direction = 11;
							else
								direction = 10;
						}
					}
				}

				samples.at<float>(iStatus, as_integer(IDX::MD)) = direction / double(13);
				samples.at<float>(iStatus, as_integer(IDX::MI)) = len;

				if (samples.at<float>(iStatus, as_integer(IDX::MD)) < 0 || samples.at<float>(iStatus, as_integer(IDX::MD)) > 1)
				{
					LOG_FATAL("X not normelized" << samples.at<float>(iStatus, as_integer(IDX::X)));
				}

			} //end status
		}

		try
		{
			if (iFrame == 0)
			{
				backupSamples.release();
				samples.copyTo(backupSamples);
			}
			else
			{
				cv::vconcat(samples, backupSamples, backupSamples);
				tsamples.release();
				backupSamples.copyTo(tsamples);
			}

		}
		catch (std::exception& e) {
			LOG_ERROR("An exception occurred during copyTo and cconcat of samples: " << e.what());
			LOG_ERROR("Sampling ended with framenumber " << iFrame << " of" << framecount);
			LOG_ERROR("Sampling ended with minute " << (iFrame / 25 / 60) << " of" << (framecount / 25 / 60));
			break;
		}

		//temporalsamples.push_back(samples);
		prevGray = gray.clone();
		prevCorners = std::vector<cv::Point2f>(cornersCopy);

	}

	LOG_INFO("Temporal sampling finished; start to cluster");

	//tsamples.release();
	//cv::vconcat(temporalsamples, tsamples);

	// Prepare initial centroids.
	cv::Mat clusters;
	// make seeds from the first samples 
	tsamples(cv::Rect(0, 0, tsamples.cols, mMaxCluster)).copyTo(clusters);
	// set initial weight to 1
	clusters(cv::Rect(as_integer(IDX::WEIGHT), 0, 1, clusters.rows)) = 1;

	//prepare for iterating

	joinCloseClusters(clusters);
	dropLightPoints(clusters);


	for (std::size_t iteration = 0; iteration < mPCTSignatures->getIterationCount(); iteration++)
	{
		// Prepare space for new centroid values.
		cv::Mat tmpCentroids(clusters.size(), clusters.type());
		tmpCentroids = 0;

		// Clear weights for new iteration.
		clusters(cv::Rect(as_integer(IDX::WEIGHT), 0, 1, clusters.rows)) = 0;

		// Compute affiliation of points and sum new coordinates for centroids.
		for (std::size_t iSample = 0; iSample < tsamples.rows; iSample++)
		{
			std::size_t iClosest = findClosestCluster(clusters, tsamples, iSample);
			for (std::size_t iDimension = 0; iDimension < as_integer(IDX::DIMS) - 1; iDimension++)
			{
				tmpCentroids.at<float>(iClosest, iDimension) += tsamples.at<float>(iSample, iDimension);
			}
			clusters.at<float>(iClosest, as_integer(IDX::WEIGHT))++;
		}

		// Compute average from tmp coordinates and throw away too small clusters.
		std::size_t lastIdx = 0;
		for (std::size_t i = 0; (int)i < tmpCentroids.rows; ++i)
		{
			// Skip clusters that are too small (large-enough clusters are packed right away)
			if (clusters.at<float>(i, as_integer(IDX::WEIGHT)) >(iteration + 1) * mPCTSignatures->getClusterMinSize())
			{
				for (std::size_t d = 0; d < as_integer(IDX::DIMS) - 1; d++)
				{
					clusters.at<float>(lastIdx, d) = tmpCentroids.at<float>(i, d) / clusters.at<float>(i, as_integer(IDX::WEIGHT));
				}
				// weights must be compacted as well
				clusters.at<float>(lastIdx, as_integer(IDX::WEIGHT)) = clusters.at<float>(i, as_integer(IDX::WEIGHT));
				lastIdx++;
			}
		}

		// Crop the arrays if some centroids were dropped.
		clusters.resize(lastIdx);
		if (clusters.rows == 0)
		{
			break;
		}

		// Finally join clusters with too close centroids.
		joinCloseClusters(clusters);
		dropLightPoints(clusters);

		//if (iteration % 2 == 0)
		//std::cout << "\t\t=>Interation Nr: " << iteration << " of " << ITERATION_COUNT << std::endl;
	}

	// The result must not be empty!
	if (clusters.rows == 0)
	{
		singleClusterFallback(tsamples, clusters);
	}

	cropClusters(clusters);

	normalizeWeights(clusters);


	cv::Mat signatures;
	signatures.create(clusters.rows, as_integer(IDX::DIMS), clusters.type());
	clusters.copyTo(signatures);

	LOG_INFO("Temporal clustering finished.");

	FeatureSignatures* fs = new FeatureSignatures(_videobase->mVideoID, _videobase->mClazz, _videobase->mStartFrameNumber, framecount);
	fs->mVectors = signatures;
	video.release();
	return fs;
}

void defuse::DFS2Xtractor::computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const
{
}

void defuse::DFS2Xtractor::joinCloseClusters(cv::Mat clusters)
{
	for (int i = 0; i < clusters.rows - 1; i++)
	{
		if (clusters.at<float>(i, as_integer(IDX::WEIGHT)) == 0)
		{
			continue;
		}

		for (int j = i + 1; j < clusters.rows; j++)
		{
			if (clusters.at<float>(j, as_integer(IDX::WEIGHT)) > 0 && computeL2(clusters, i, clusters, j) <= mPCTSignatures->getJoiningDistance())
			{
				clusters.at<float>(i, as_integer(IDX::WEIGHT)) = 0;
				break;
			}
		}
	}
}

void defuse::DFS2Xtractor::dropLightPoints(cv::Mat& clusters)
{
	std::size_t frontIdx = 0;

	// Skip leading continuous part of weighted-enough points.
	while (frontIdx < clusters.rows && clusters.at<float>(frontIdx, as_integer(IDX::WEIGHT)) > mPCTSignatures->getDropThreshold())
	{
		++frontIdx;
	}

	// Mark first emptied position and advance front index.
	std::size_t tailIdx = frontIdx++;

	while (frontIdx < clusters.rows)
	{
		if (clusters.at<float>(frontIdx, as_integer(IDX::WEIGHT)) > mPCTSignatures->getDropThreshold())
		{
			// Current (front) item is not dropped -> copy it to the tail.
			clusters.row(frontIdx).copyTo(clusters.row(tailIdx));
			++tailIdx;	// grow the tail
		}
		++frontIdx;
	}
	clusters.resize(tailIdx);
}

std::size_t defuse::DFS2Xtractor::findClosestCluster(cv::Mat& clusters, cv::Mat& points, std::size_t pointIdx)
{
	std::size_t iClosest = 0;
	float minDistance = computeL2(clusters, 0, points, pointIdx);
	for (std::size_t iCluster = 1; iCluster < clusters.rows; iCluster++)
	{
		float distance = computeL2(clusters, iCluster, points, pointIdx);
		if (distance < minDistance)
		{
			iClosest = iCluster;
			minDistance = distance;
		}
	}
	return iClosest;
}

void defuse::DFS2Xtractor::normalizeWeights(cv::Mat& clusters)
{
	// get max weight
	float maxWeight = clusters.at<float>(0, as_integer(IDX::WEIGHT));
	for (int i = 1; i < clusters.rows; i++)
	{
		if (clusters.at<float>(i, as_integer(IDX::WEIGHT)) > maxWeight)
		{
			maxWeight = clusters.at<float>(i, as_integer(IDX::WEIGHT));
		}
	}

	// normalize weight
	float weightNormalizer = 1 / maxWeight;
	for (int i = 0; i < clusters.rows; i++)
	{
		clusters.at<float>(i, as_integer(IDX::WEIGHT)) = clusters.at<float>(i, as_integer(IDX::WEIGHT)) * weightNormalizer;
	}
}

void defuse::DFS2Xtractor::singleClusterFallback(const cv::Mat& points, cv::Mat& clusters)
{
	if (clusters.rows != 0)
	{
		return;
	}

	// Initialize clusters.
	clusters.resize(1);
	clusters.at<float>(0, as_integer(IDX::WEIGHT)) = static_cast<float>(points.rows);

	// Sum all points.
	for (std::size_t i = 0; i < points.rows; ++i)
	{
		for (std::size_t d = 0; d < as_integer(IDX::DIMS) - 1; d++)
		{
			clusters.at<float>(0, d) += points.at<float>(i, d);
		}
	}

	// Divide centroid by number of points -> compute average in each dimension.
	for (std::size_t i = 0; i < points.rows; ++i)
	{
		for (std::size_t d = 0; d < as_integer(IDX::DIMS) - 1; d++)
		{
			clusters.at<float>(0, d) = clusters.at<float>(0, d) / clusters.at<float>(0, as_integer(IDX::WEIGHT));
		}
	}
}

void defuse::DFS2Xtractor::cropClusters(cv::Mat& clusters)
{
	if (clusters.rows > mPCTSignatures->getMaxClustersCount())
	{
		cv::Mat duplicate;						// save original clusters
		clusters.copyTo(duplicate);

		cv::Mat sortedIdx;						// sort using weight column
		sortIdx(clusters(cv::Rect(as_integer(IDX::DIMS) - 1, 0, 1, clusters.rows)), sortedIdx, cv::SORT_EVERY_COLUMN + cv::SORT_DESCENDING);

		clusters.resize(mPCTSignatures->getMaxClustersCount());		// crop to max clusters
		for (int i = 0; i < mPCTSignatures->getMaxClustersCount(); i++)
		{									// copy sorted rows
			duplicate.row(sortedIdx.at<int>(i, 0)).copyTo(clusters.row(i));
		}
	}
}

float defuse::DFS2Xtractor::computeL2(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2)
{
	double result = 0.0;

	//without weights - 1!
	for (int i = 0; i < _f1.cols - 1; i++)
	{
		double distance = _f1.at<float>(idx1, i) - _f2.at<float>(idx2, i);
		result += distance * distance;
	}

	result = std::sqrt(result);
	return result;
}
