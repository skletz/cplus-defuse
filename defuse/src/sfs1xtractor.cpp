#include "sfs1xtractor.hpp"
#include "sfs1parameter.hpp"
#include "featuresignatures.hpp"
#include <cpluslogger.hpp>

defuse::SFS1Xtractor::SFS1Xtractor(Parameter* _parameter)
{
	int samples = dynamic_cast<SFS1Parameter *>(_parameter)->samplepoints;

	Directory samplepointdir(dynamic_cast<SFS1Parameter *>(_parameter)->samplepointsfile);
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
	mMaxCluster = dynamic_cast<SFS1Parameter *>(_parameter)->initialCentroids;
	mPCTSignatures = pct::PCTSignatures::create(mSamplepoints->getPoints(), mSamplepoints->getSampleCnt(), mMaxCluster);
	mKeyFrame = dynamic_cast<SFS1Parameter *>(_parameter)->keyframeSelection;
}

defuse::Features* defuse::SFS1Xtractor::xtract(VideoBase* _videobase)
{
	cv::VideoCapture video(_videobase->mFile->getFile());
	video.set(CV_CAP_PROP_BUFFERSIZE, 3);

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
	//LOG_PERFMON(PINTERIM, "Execution Time (One Signature per Video): " << "\tFrame Count\t" << framecount << "\tElapsed Time\t" << elapsedTime);

	//FeatureSignatures* featuresignatures = new FeatureSignatures(_videobase->mVideoFileName, _videobase->mVideoID, _videobase->mClazz, _videobase->mStartFrameNumber, framecount);
	FeatureSignatures* featuresignatures = new FeatureSignatures(_videobase->mVideoFileName);
	featuresignatures->mVectors = signatures;

	video.release();
	return featuresignatures;
}

void defuse::SFS1Xtractor::computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const
{
	unsigned int numframes = _video.get(CV_CAP_PROP_FRAME_COUNT);

	int framenr = numframes;

	if (mKeyFrame == 0) //use middle frame
	{
		framenr = numframes / float(2);
		_video.set(CV_CAP_PROP_POS_FRAMES, framenr);
	}
	else if(mKeyFrame == 1) //use first frame
	{
		framenr = 1;
		_video.set(CV_CAP_PROP_POS_FRAMES, framenr);
	}else if (mKeyFrame == 2) //use last frame
	{
		_video.set(CV_CAP_PROP_POS_FRAMES, framenr);
	}else
	{
		LOG_FATAL("Keyframe selection > 3 not implemented use; middle, first or last frame");
		return;
	}

	cv::Mat image, signatures;

	_video.grab();
	_video.retrieve(image);

	mPCTSignatures->computeSignature(image, signatures);
	signatures.copyTo(_signatures);
}
