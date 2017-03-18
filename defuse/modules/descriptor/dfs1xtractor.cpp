#include "dfs1xtractor.hpp"
#include "dfs1parameter.hpp"
#include <cpluslogger.h>
#include "featuresignatures.hpp"

defuse::DFS1Xtractor::DFS1Xtractor(Parameter* _parameter)
{
	int samples = dynamic_cast<DFS1Parameter *>(_parameter)->staticparamter.samplepoints;

	Directory samplepointdir(dynamic_cast<DFS1Parameter *>(_parameter)->staticparamter.samplepointsfile);
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
	mMaxCluster = dynamic_cast<DFS1Parameter *>(_parameter)->staticparamter.initialCentroids;
	mMaxFrames = dynamic_cast<DFS1Parameter *>(_parameter)->frames;

	mFrameSelection = dynamic_cast<DFS1Parameter *>(_parameter)->frameSelection;

	mTPCTSignatures = new tpct::TPCTSignatures();

	mPCTSignatures = pct::PCTSignatures::create(mSamplepoints->getPoints(), mSamplepoints->getSampleCnt(), mMaxCluster);
}

defuse::Features* defuse::DFS1Xtractor::xtract(VideoBase* _videobase)
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
	unsigned int framecount = video.get(CV_CAP_PROP_FRAME_COUNT);
	LOG_PERFMON(PINTERIM, "Execution Time (One Signature per Video): " << "\tFrame Count\t" << framecount << "\tElapsed Time\t" << elapsedTime);

	FeatureSignatures* featuresignatures = new FeatureSignatures(_videobase->mVideoID, _videobase->mClazz, _videobase->mStartFrameNumber, framecount);
	featuresignatures->mVectors = signatures;

	video.release();
	return featuresignatures;
}

void defuse::DFS1Xtractor::computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const
{
	unsigned int numframes = _video.get(CV_CAP_PROP_FRAME_COUNT);

	int numberOfFramesPerShot = mMaxFrames;

	if (mMaxFrames < numframes)
	{
		numberOfFramesPerShot = numframes - 1;
	}

	std::vector<cv::Mat> sampledsignatures;
	if (mFrameSelection == 0) //use fix number of frame per segment
	{
		int interval = numframes / float(numberOfFramesPerShot + 1);


		for (int j = 0; j < numframes; j = j + interval)
		{
			cv::Mat image, signatures;
			_video.set(CV_CAP_PROP_POS_FRAMES, j);
			_video.grab();
			_video.retrieve(image);

			mPCTSignatures->computeSignature(image, signatures);

			if (interval == 0)
			{
				j++;
			}

			if (!signatures.empty())
				sampledsignatures.push_back(signatures);
		}

	}
	else if (mFrameSelection == 2) //use fix number of frames per second
	{
		int half = numframes / float(2);

		int interval1 = half / float(numberOfFramesPerShot / double(2));

		for (int j = interval1; j < half; j = j + interval1)
		{
			cv::Mat image1, signatures1;

			_video.set(CV_CAP_PROP_POS_FRAMES, j);
			_video.grab();
			_video.retrieve(image1);

			mPCTSignatures->computeSignature(image1, signatures1);

			if (!signatures1.empty())
				sampledsignatures.push_back(signatures1);

			sampledsignatures.push_back(signatures1);
		}

		cv::Mat image2, signatures2;
		_video.set(CV_CAP_PROP_POS_FRAMES, half);
		_video.grab();
		_video.retrieve(image2);
		mPCTSignatures->computeSignature(image2, signatures2);

		if (!signatures2.empty())
			sampledsignatures.push_back(signatures2);

		for (int j = half + interval1; j < numframes; j = j + interval1)
		{
			cv::Mat image3, signatures3;

			_video.set(CV_CAP_PROP_POS_FRAMES, j);
			_video.grab();
			_video.retrieve(image3);

			mPCTSignatures->computeSignature(image3, signatures3);
			if (!signatures3.empty())
				sampledsignatures.push_back(signatures3);
		}
	}
	else if (mFrameSelection == 2) { //use all frames
	
	}else
	{
		LOG_FATAL("Frame Selection 3 not implemented; use a fix number per segment, second or all frames")
	}
		


	cv::Mat tSignatures;
	mTPCTSignatures->computeTemporalSignature(sampledsignatures, tSignatures);

	tSignatures.copyTo(_signatures);
}
