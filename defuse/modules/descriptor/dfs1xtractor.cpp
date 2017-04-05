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
	//LOG_ERROR("Number of Frames " << numframes);

	//the shot has fewer frames than shuld be used
	if (mMaxFrames > numframes)
	{
		//starts from 0
		numberOfFramesPerShot = numframes - 1;
		LOG_ERROR("Video Segment is smaller than max frames");
	}

	std::vector<cv::Mat> sampledsignatures;

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
		for (int iFrame = 0; iFrame < limit; iFrame = iFrame + interval)
		{
			cv::Mat image, signatures;
			_video.set(CV_CAP_PROP_POS_FRAMES, iFrame);
			_video.grab();
			_video.retrieve(image);

			if (!image.empty())
				mPCTSignatures->computeSignature(image, signatures);
			else
			{
				LOG_ERROR("Image at " << iFrame << " was empty");
			}

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
	}
	else if (mFrameSelection == 3)
	{
		LOG_FATAL("Frame Selection 3 is under development. Abort?")
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
	else
	{
		LOG_FATAL("Frame Selection 3 not implemented; use a fix number per segment, second or all frames. Aborted!")
		return;
	}
		
	cv::Mat tSignatures;
	mTPCTSignatures->computeTemporalSignature(sampledsignatures, tSignatures);

	if(sampledsignatures.size() > mMaxFrames)
	{
		LOG_FATAL("Bug: More frames extracted than should be used. Abort?")
	}

	tSignatures.copyTo(_signatures);
}
