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

		int desirelizedSize = static_cast<int>(samplePoints->getPoints().size());
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
	double framecount = video.get(CV_CAP_PROP_FRAME_COUNT);
	//LOG_PERFMON(PINTERIM, "Execution Time (One Signature per Video): " << "\tFrame Count\t" << framecount << "\tElapsed Time\t" << elapsedTime);

	FeatureSignatures* featuresignatures = new FeatureSignatures(_videobase->mVideoFileName, _videobase->mVideoID, _videobase->mClazz, _videobase->mStartFrameNumber, framecount);
	featuresignatures->mVectors = signatures;

	video.release();
	return featuresignatures;
}

void defuse::DFS1Xtractor::computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const
{
	int numframes = static_cast<int>(_video.get(CV_CAP_PROP_FRAME_COUNT));
	int numberOfFramesPerShot = mMaxFrames;

	//the shot has fewer frames than shuld be used
	if (mMaxFrames > numframes)
	{
		//starts from 0
		numberOfFramesPerShot = numframes - 1;
	}

	std::vector<cv::Mat> sampledsignatures;

	int interval = 0;
	//use fix number of frames per segment
	if (mFrameSelection == 0) 
	{
		interval = static_cast<int>(numframes / float(numberOfFramesPerShot));

		for (int iFrame = 0; iFrame < numframes-1; iFrame = iFrame + interval)
		{
			cv::Mat image, signatures;
			_video.set(CV_CAP_PROP_POS_FRAMES, iFrame);
			_video.grab();
			_video.retrieve(image);

			mPCTSignatures->computeSignature(image, signatures);

			//use all frames that exists
			if (interval == 0)
			{
				iFrame++;
			}

			if (!signatures.empty())
				sampledsignatures.push_back(signatures);
		}

	}
	//use fix number of frames per second
	else if (mFrameSelection == 1) 
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
	}else
	{
		LOG_FATAL("Frame Selection 3 not implemented; use a fix number per segment, second or all frames. Aborted!")
		return;
	}
		
	cv::Mat tSignatures;
	mTPCTSignatures->computeTemporalSignature(sampledsignatures, tSignatures);

	if(sampledsignatures.size() > mMaxFrames)
	{
		LOG_FATAL("Bug: More frames extracted than should be used. Aborted!")
		return;
	}

	tSignatures.copyTo(_signatures);
}
