#include "dfs2xtractor.hpp"
#include "dfs2parameter.hpp"
#include <cpluslogger.h>
#include "featuresignatures.hpp"

#define _USE_MATH_DEFINES 
#include <math.h>
#include <filesystem>

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

	mFrameSelection = dynamic_cast<DFS2Parameter *>(_parameter)->frameSelection;

	mTPCTSignatures = new tpct::TPCTSignatures();

	mPCTSignatures = pct::PCTSignatures::create(mSamplepoints->getPoints(), mSamplepoints->getSampleCnt(), mMaxCluster);
	//mPCTSignatures = pct::PCTSignatures::create(samples, mSamplepoints->getSampleCnt(), cv::xfeatures2d::PCTSignatures::PointDistribution::REGULAR);

	for (int i = 0; i < as_integer(IDX::DIMS); i++)
	{
		mWeights.push_back(1.0);
		mTranslations.push_back(0.0);
	}

	mVariant = 0;

	maxLenght = 2.0;
	mFilterLength = 30;

	mIterations = dynamic_cast<DFS2Parameter *>(_parameter)->staticparamter.iterations;
}

defuse::Features* defuse::DFS2Xtractor::xtract(VideoBase* _videobase)
{
	cv::VideoCapture video(_videobase->mFile->getFile());

	if (!video.isOpened())
	{
		LOG_FATAL("Fatal error: Video File does not exists." << _videobase->mFile->getFile());
	}

	size_t start = cv::getTickCount();
	cv::Mat signatures;
	computeSignatures(video, signatures);
	size_t end = cv::getTickCount();

	double elapsedTime = (end - start) / (cv::getTickFrequency() * 1.0f);
	double framecount = video.get(CV_CAP_PROP_FRAME_COUNT);
	//LOG_PERFMON(PINTERIM, "Execution Time (One Signature per Video): " << "\tFrame Count\t" << framecount << "\tElapsed Time\t" << elapsedTime);

	FeatureSignatures* featuresignatures = new FeatureSignatures(_videobase->mVideoFileName, _videobase->mVideoID, _videobase->mClazz, _videobase->mStartFrameNumber, framecount);
	featuresignatures->mVectors = signatures;

	video.release();
	return featuresignatures;
}


void defuse::DFS2Xtractor::getSamples(cv::Mat& frame, std::vector<cv::Point2f> points, cv::Mat& out) const
{
	cv::Mat samples;
	out.copyTo(samples);

	cv::xfeatures2d::pct_signatures::GrayscaleBitmap grayscaleBitmap(frame, mGrayscaleBits);
	std::vector<cv::Point2f> corners;
	for (int iSample = 0; iSample < points.size(); iSample++)
	{
		float x = std::size_t(points[iSample].x);
		float y = std::size_t(points[iSample].y);

		//currPoints.push_back(cv::Point(x, y));

		if (x >= frame.cols)
			continue;

		if (y >= frame.rows)
			continue;

		samples.at<float>(iSample, as_integer(IDX::X)) = float(double(x) / double(frame.cols));	// x, y normalized
		samples.at<float>(iSample, as_integer(IDX::Y)) = float(double(y) / double(frame.rows));

		cv::Mat rgbPixel(frame, cv::Rect(x, y, 1, 1));			// get Lab pixel color
		cv::Mat labPixel;										// placeholder -> TODO: optimize
		rgbPixel.convertTo(rgbPixel, CV_32FC3, 1.0 / 255);		//scale it down
		cv::cvtColor(rgbPixel, labPixel, cv::COLOR_BGR2Lab);
		cv::Vec3f labColor = labPixel.at<cv::Vec3f>(0, 0);		// end

		samples.at<float>(iSample, as_integer(IDX::L)) = float(std::floor(labColor[0] + 0.5) / float(cv::xfeatures2d::pct_signatures::L_COLOR_RANGE));	// Lab color normalized
		samples.at<float>(iSample, as_integer(IDX::A)) = float(std::floor(labColor[1] + 0.5 + cv::xfeatures2d::pct_signatures::A_COLOR_RANGE) / float((cv::xfeatures2d::pct_signatures::A_COLOR_RANGE * 2) + 1));
		samples.at<float>(iSample, as_integer(IDX::B)) = float(std::floor(labColor[2] + 0.5 + cv::xfeatures2d::pct_signatures::B_COLOR_RANGE) / float((cv::xfeatures2d::pct_signatures::B_COLOR_RANGE * 2) + 1));

		double contrast = 0.0, entropy = 0.0;
		grayscaleBitmap.getContrastEntropy(x, y, contrast, entropy, mWindowRadius);
		samples.at<float>(iSample, as_integer(IDX::C))
			= static_cast<float>(contrast / cv::xfeatures2d::pct_signatures::SAMPLER_CONTRAST_NORMALIZER * mWeights[as_integer(IDX::C)] + mTranslations[as_integer(IDX::C)]);			// contrast
		samples.at<float>(iSample, as_integer(IDX::E))
			= static_cast<float>(entropy / cv::xfeatures2d::pct_signatures::SAMPLER_ENTROPY_NORMALIZER * mWeights[as_integer(IDX::E)] + mTranslations[as_integer(IDX::E)]);

		samples.at<float>(iSample, as_integer(IDX::MD)) = 0;
		samples.copyTo(out);
	}

}


void defuse::DFS2Xtractor::getMotionDirection(std::vector<uchar>& statusVector, std::vector<float>& errorVector, std::vector<cv::Point2f>& prevPoints, std::vector<cv::Point2f>& currPoints, int height, int width, cv::Mat& out) const
{
	cv::Mat samples;
	out.copyTo(samples);

	for (int iStatus = 0; iStatus < statusVector.size(); iStatus++)
	{
		if (statusVector[iStatus] == 0)
		{
			continue;
		}

		cv::Point2f pA = prevPoints[iStatus];
		cv::Point2f pB = currPoints[iStatus];

		float mvX = pA.x - pB.x;
		float mvY = pA.y - pB.y;

		//Filter to length motion vector, the same as displayed
		//Skip to length motion vectors
		//if (cv::norm(pA - pB) < int(height / maxLenght))
		//{
		//	continue;
		//}

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
					direction = 4;
				else
					direction = 10;
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
		samples.copyTo(out);
	} //end status
}

void defuse::DFS2Xtractor::getTemporalSamples(cv::Mat& tsamples, cv::Mat& signatures) const
{

	//LOG_INFO("Temporal sampling finished; start to cluster");

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


	for (std::size_t iteration = 0; iteration < mIterations; iteration++)
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

	signatures.create(clusters.rows, as_integer(IDX::DIMS), clusters.type());
	clusters.copyTo(signatures);

	//LOG_INFO("Temporal clustering finished.");
}

void defuse::DFS2Xtractor::computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const
{
	cv::Mat signatures;
	int numframes = static_cast<int>(_video.get(CV_CAP_PROP_FRAME_COUNT));
	int numberOfFramesPerShot = mMaxFrames;

	//the shot has fewer frames than should be used
	if (mMaxFrames > numframes)
	{
		//grapping starts from 0
		numberOfFramesPerShot = numframes - 1;
		LOG_ERROR("Video Segment is smaller than max frames");
	}

	std::vector<cv::Point2f> prevPoints, currPoints, initPoints;
	int widht = _video.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = _video.get(CV_CAP_PROP_FRAME_HEIGHT);

	initPoints = mSamplepoints->getPoints();
	//De-Normalize
	for (int i = 0; i < initPoints.size(); i++)
	{
		cv::Point p(0, 0);
		p.x = initPoints.at(i).x * widht;
		p.y = initPoints.at(i).y * height;
		initPoints.at(i) = p;
	}
	
	currPoints = std::vector<cv::Point2f>(initPoints);

	cv::Mat frame, prevGrayFrame, grayFrame;

	cv::Mat tsamples, backupSamples;
	std::vector<cv::Mat> temporalsamples;
	int interval = 0;
	int counter = 0;
	int limit = 0;

	//use fix number of frames per segment
	if (mFrameSelection == 0)
	{
		interval = static_cast<int>(numframes / float(numberOfFramesPerShot));

		if (numframes % mMaxFrames == 0)
		{
			limit = numframes - 1;
		}
		else
		{
			limit = (interval * mMaxFrames) - 1;
		}
		for (int iFrame = 0; iFrame  < limit; iFrame = iFrame + interval)
		{
			//Vectors of samplepoints
			cv::Mat samples;
			samples.create(mSamplepoints->getSampleCnt(), as_integer(IDX::DIMS), CV_32F);

			//Capture the current frame
			_video.set(CV_CAP_PROP_POS_FRAMES, iFrame);
			_video.grab();
			_video.retrieve(frame);

			if (frame.empty()) continue;

			//convert the frame to grayscale
			cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);

			if(mVariant == 1)
			{
				if (prevPoints.size() != currPoints.size())
					prevPoints = currPoints;
			}else
			{
				currPoints = std::vector<cv::Point2f>(initPoints);
			}


			getSamples(frame, currPoints, samples);



			if (iFrame > 0)
			{
				//Indicates wheter the flow for the corresponding features has been found
				std::vector<uchar> statusVector;
				statusVector.resize(currPoints.size());
				//Indicates the error for the corresponding feature
				std::vector<float> errorVector;
				errorVector.resize(currPoints.size());

				//cv::TermCriteria termcrit(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01);
				//Calculate movements between previous and actual frame
				//cv::calcOpticalFlowPyrLK( //Default Settings of FlowPyrLK
				//	prevGrayFrame, grayFrame, prevPoints,
				//	currPoints, statusVector, errorVector,
				//	cv::Size(21, 21), 3, termcrit, 0, 0.0001
				//);
				cv::calcOpticalFlowPyrLK(
					prevGrayFrame, grayFrame, prevPoints, 
					currPoints, statusVector, errorVector
				);
				//
				getMotionDirection(statusVector, errorVector, prevPoints, currPoints, height, widht, samples);

				if (display)
				{
					//visualize STARTs
					cv::Mat frameCopy = frame.clone();
					for (uint i = 0; i < statusVector.size(); i++) {
						if (statusVector[i] != 0) {

							cv::Point p(ceil(prevPoints[i].x), ceil(prevPoints[i].y));
							cv::Point q(ceil(currPoints[i].x), ceil(currPoints[i].y));
							//arrowedLine(frameCopy, p0, p1, cv::Scalar(255, 255, 255, 0), 1, CV_AA, 0,0.5);
							// && errorVector[i] < int(height / maxLenght)
	/*						if (cv::norm(p - q) < int(height / maxLenght))
							{*/
								drawLine(frameCopy, p, q, 3);
							//}

						}
					}

					//cv::imshow("FrameCopy", frameCopy);
					cv::Mat sampleImg;
					draw2DSampleSignature(frame, samples, sampleImg);

					cv::Mat resFrame, resSampleImg;

					cv::resize(frameCopy, resFrame, cv::Size(frame.cols / 2, frame.rows / 2));
					cv::imshow("Video Stream", resFrame);
					cv::moveWindow("Video Stream", 0, 0);

					cv::resize(sampleImg, resSampleImg, cv::Size(frame.cols / 2, frame.rows / 2));
					cv::imshow("Feature Signatures", resSampleImg);
					cv::moveWindow("Feature Signatures", resFrame.cols + 10, 0);

					int keyPressed;
					keyPressed = cv::waitKey(5);

					//visualization ENDs				
				}

				prevPoints.clear();
			}

			//temporalsamples.push_back(samples);

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
				LOG_ERROR("Sampling ended with framenumber " << iFrame << " of" << numframes);
				LOG_ERROR("Sampling ended with minute " << (iFrame / 25 / 60) << " of" << (numframes / 25 / 60));
				break;
			}

			prevGrayFrame = grayFrame.clone();
			prevPoints = std::vector<cv::Point2f>(initPoints);
			
		}

		getTemporalSamples(tsamples, signatures);

		if (display)
		{
			cv::Mat signatureImg, resSignatureImg;
			draw2DFSSignature(frame, signatures, signatureImg);
			drawSignatureMotionDirection(signatures, signatureImg, frame.cols, frame.rows);

			cv::resize(signatureImg, resSignatureImg, cv::Size(frame.cols / 2, frame.rows / 2));
			cv::imshow("Flow-based Feature Signatures", resSignatureImg);
			cv::moveWindow("Flow-based Feature Signatures", (resSignatureImg.cols * 2) + 10, 0);

			while (cv::waitKey(1) != 27);

			cv::destroyAllWindows();
		}
		

	}
	else if (mFrameSelection == 1)//use fix number of frames per second
	{
		//TODO Implement fix number per segment
		LOG_FATAL("Frame Selection 2 not implemented; use a fix number per segment = 0. Aborted!")
			return;
	}
	//use all frames
	else if (mFrameSelection == 2)
	{
		//@TODO Implement all frames
		LOG_FATAL("Frame Selection 2 not implemented; use a fix number per segment = 0. Aborted!")
			return;
	}
	else
	{
		LOG_FATAL("Frame Selection 3 not implemented; use a fix number per segment, second or all frames. Aborted!")
			return;
	}

	//if (temporalsamples.size() > mMaxFrames)
	//{
	//	LOG_FATAL("Bug: More frames extracted than should be used. Aborted!")
	//	return;
	//}

	signatures.copyTo(_signatures);
}


void defuse::DFS2Xtractor::joinCloseClusters(cv::Mat clusters) const
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

void defuse::DFS2Xtractor::dropLightPoints(cv::Mat& clusters) const
{
	int frontIdx = 0;

	// Skip leading continuous part of weighted-enough points.
	while (frontIdx < clusters.rows && clusters.at<float>(frontIdx, as_integer(IDX::WEIGHT)) > mPCTSignatures->getDropThreshold())
	{
		++frontIdx;
	}

	// Mark first emptied position and advance front index.
	int tailIdx = frontIdx++;

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

int defuse::DFS2Xtractor::findClosestCluster(cv::Mat& clusters, cv::Mat& points, int pointIdx) const
{
	int iClosest = 0;
	float minDistance = computeL2(clusters, 0, points, pointIdx);
	for (int iCluster = 1; iCluster < clusters.rows; iCluster++)
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

// ReSharper disable once CppMemberFunctionMayBeStatic
void defuse::DFS2Xtractor::normalizeWeights(cv::Mat& clusters) const
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

// ReSharper disable once CppMemberFunctionMayBeStatic
void defuse::DFS2Xtractor::singleClusterFallback(const cv::Mat& points, cv::Mat& clusters) const
{
	if (clusters.rows != 0)
	{
		return;
	}

	// Initialize clusters.
	clusters.resize(1);
	clusters.at<float>(0, as_integer(IDX::WEIGHT)) = static_cast<float>(points.rows);

	// Sum all points.
	for (int i = 0; i < points.rows; ++i)
	{
		for (int d = 0; d < as_integer(IDX::DIMS) - 1; d++)
		{
			clusters.at<float>(0, d) += points.at<float>(i, d);
		}
	}

	// Divide centroid by number of points -> compute average in each dimension.
	for (int i = 0; i < points.rows; ++i)
	{
		for (int d = 0; d < as_integer(IDX::DIMS) - 1; d++)
		{
			clusters.at<float>(0, d) = clusters.at<float>(0, d) / clusters.at<float>(0, as_integer(IDX::WEIGHT));
		}
	}
}

void defuse::DFS2Xtractor::cropClusters(cv::Mat& clusters) const
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

float defuse::DFS2Xtractor::computeL2(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2) const
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

void defuse::DFS2Xtractor::draw2DSampleSignature(const cv::InputArray _source, const cv::InputArray _signature, cv::OutputArray& _result) const
{

	// check source
	if (_source.empty())
	{
		return;
	}
	cv::Mat source = _source.getMat();

	// create result
	_result.create(source.size(), source.type());
	cv::Mat result = _result.getMat();

	cv::cvtColor(result, result, CV_BGR2BGRA);
	result.setTo(cv::Scalar(255, 255, 255, 0));

	// check signature
	if (_signature.empty())
	{
		return;
	}
	cv::Mat signature = _signature.getMat();



	// compute max radius = one eigth of the length of the shorter dimension
	int maxRadius = ((source.rows < source.cols) ? source.rows : source.cols) / 8;


	// draw signature
	for (int i = 0; i < signature.rows; i++)
	{
		cv::Vec3f labColor(
			signature.at<float>(i, as_integer(IDX::L)) * cv::xfeatures2d::pct_signatures::L_COLOR_RANGE, // get Lab pixel color
			signature.at<float>(i, as_integer(IDX::A)) * (cv::xfeatures2d::pct_signatures::A_COLOR_RANGE * 2) - cv::xfeatures2d::pct_signatures::A_COLOR_RANGE,		// placeholder -> TODO: optimize
			signature.at<float>(i, as_integer(IDX::B)) * (cv::xfeatures2d::pct_signatures::B_COLOR_RANGE * 2) - cv::xfeatures2d::pct_signatures::B_COLOR_RANGE);
		cv::Mat labPixel(1, 1, CV_32FC3);
		labPixel.at<cv::Vec3f>(0, 0) = labColor;
		cv::Mat rgbPixel;
		cvtColor(labPixel, rgbPixel, cv::COLOR_Lab2BGR);
		rgbPixel.convertTo(rgbPixel, CV_8UC4, 255);
		rgbPixel.convertTo(rgbPixel, cv::COLOR_BGR2BGRA);
		cv::Vec4b rgbColor = rgbPixel.at<cv::Vec4b>(0, 0);	// end
		rgbColor[3] = 255;

		// precompute variables
		cv::Point center(signature.at<float>(i, as_integer(IDX::X)) * source.cols, signature.at<float>(i, as_integer(IDX::Y)) * source.rows);
		int radius(maxRadius*0.05);

		cv::Vec3b borderColor(0, 0, 0);
		int borderThickness(0);

		// draw filled circle
		circle(result, center, radius, rgbColor, -1);
		// draw circle outline
		//circle(result, center, radius, borderColor, borderThickness);
	}

	result.copyTo(_result);
}

double defuse::DFS2Xtractor::square(int a) const
{
	return a * a;
}

void defuse::DFS2Xtractor::drawLine(cv::Mat& image, cv::Point p, cv::Point q, int scale) const
{
	//cv::Scalar lineColor = cv::Scalar(200, 255, 170, 255);
	//cv::Scalar lineColor = cv::Scalar(230, 4, 34, 255);
	cv::Scalar lineColor = cv::Scalar(255, 255, 255, 255);
	int lineThickness = 1;

	double angle;
	angle = atan2(double(p.y) - q.y, double(p.x) - q.x);
	double hypotenuse;
	hypotenuse = sqrt(square(p.y - q.y) + square(p.x - q.x));

	q.x = int(p.x - scale * hypotenuse * cos(angle));
	q.y = int(p.y - scale* hypotenuse * sin(angle));

	line(image, p, q, lineColor, lineThickness);

	p.x = int(q.x + scale * 3 * cos(angle + M_PI / 4));
	p.y = int(q.y + scale * 3 * sin(angle + M_PI / 4));
	line(image, p, q, lineColor, lineThickness);
	p.x = int(q.x + scale * 3 * cos(angle - M_PI / 4));
	p.y = int(q.y + scale * 3 * sin(angle - M_PI / 4));
	line(image, p, q, lineColor, lineThickness);
}

void defuse::DFS2Xtractor::draw2DFSSignature(cv::InputArray _source, cv::InputArray _signature, cv::OutputArray _result) const
{

	// check source
	if (_source.empty())
	{
		return;
	}
	cv::Mat source = _source.getMat();

	// create result
	_result.create(source.size(), source.type());
	cv::Mat result = _result.getMat();
	//TODO original copies the source to signatures
	//source.copyTo(result);
	//
	cv::cvtColor(result, result, CV_BGR2BGRA);
	result.setTo(cv::Scalar(255, 255, 255, 0));

	// check signature
	if (_signature.empty())
	{
		return;
	}
	cv::Mat signature = _signature.getMat();


	//if (signature.type() != CV_32F || signature.cols != indices.dim)
	//{
	//	CV_Error_(CV_StsBadArg, ("Invalid signature format. Type must be CV_32F and signature.cols must be %d.", indices.dim));
	//}

	// compute max radius = one eigth of the length of the shorter dimension
	int maxRadius = ((source.rows < source.cols) ? source.rows : source.cols) / 8;


	// draw signature
	for (int i = 0; i < signature.rows; i++)
	{
		cv::Vec3f labColor(
			signature.at<float>(i, as_integer(IDX::L)) * cv::xfeatures2d::pct_signatures::L_COLOR_RANGE, // get Lab pixel color
			signature.at<float>(i, as_integer(IDX::A)) * (cv::xfeatures2d::pct_signatures::A_COLOR_RANGE * 2) - cv::xfeatures2d::pct_signatures::A_COLOR_RANGE,		// placeholder -> TODO: optimize
			signature.at<float>(i, as_integer(IDX::B)) * (cv::xfeatures2d::pct_signatures::B_COLOR_RANGE * 2) - cv::xfeatures2d::pct_signatures::B_COLOR_RANGE);
		cv::Mat labPixel(1, 1, CV_32FC3);
		labPixel.at<cv::Vec3f>(0, 0) = labColor;
		cv::Mat rgbPixel;
		cvtColor(labPixel, rgbPixel, cv::COLOR_Lab2BGR);
		rgbPixel.convertTo(rgbPixel, CV_8UC4, 255);
		rgbPixel.convertTo(rgbPixel, cv::COLOR_BGR2BGRA);
		cv::Vec4b rgbColor = rgbPixel.at<cv::Vec4b>(0, 0);	// end
		rgbColor[3] = 255;

		// precompute variables
		cv::Point center(signature.at<float>(i, as_integer(IDX::X)) * source.cols, signature.at<float>(i, as_integer(IDX::Y)) * source.rows);
		int radius(maxRadius * signature.at<float>(i, as_integer(IDX::WEIGHT)));

		cv::Vec3b borderColor(0, 0, 0);
		int borderThickness(0);

		// draw filled circle
		circle(result, center, radius, rgbColor, -1);
		// draw circle outline
		//circle(result, center, radius, borderColor, borderThickness);
	}

	result.copyTo(_result);
}

void defuse::DFS2Xtractor::drawSignatureMotionDirection(cv::InputArray _tempSig, cv::OutputArray _result, int _width, int _height) const
{
	//white background
	cv::Mat source(_height, _width, CV_8UC4, cv::Scalar(255, 255, 255, 0));
	cv::Mat tempSig;
	_tempSig.copyTo(tempSig);


	cv::Mat image;
	_result.copyTo(image);

	for (int i = 0; i < tempSig.rows; i++)
	{
		float x = tempSig.at<float>(i, as_integer(IDX::X));
		float y = tempSig.at<float>(i, as_integer(IDX::Y));

		int md = tempSig.at<float>(i, as_integer(IDX::MD)) * 13;

		cv::Point p((x * _width), (y * _height));

		cv::Point q;
		int length = 2;

		int angle = 0;

		if (md == 1)
		{
			angle = 0;
		}
		else if (md == 2)
		{
			angle = 30;
		}
		else if (md == 3)
		{
			angle = 60;
		}
		else if (md == 4)
		{
			angle = 90;
		}
		else if (md == 5)
		{
			angle = 120;
		}
		else if (md == 6)
		{
			angle = 150;
		}
		else if (md == 7)
		{
			angle = 180;
		}
		else if (md == 8)
		{
			angle = 210;
		}
		else if (md == 9)
		{
			angle = 240;
		}
		else if (md == 10)
		{
			angle = 270;
		}
		else if (md == 11)
		{
			angle = 300;
		}
		else if (md == 12)
		{
			angle = 330;
		}
		else
		{
			q = p;
		}


		q.x = int(round(p.x + length * cos(angle * CV_PI / 180.0)));
		q.y = int(round(p.y + (length * -sin(angle * CV_PI / 180.0))));


		if (md != 0)
		{
			drawLine(image, p, q, 5);
		}



	}

	image.copyTo(_result);
}
