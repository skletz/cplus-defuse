#include "mohist1xtractor.hpp"
#include "mohist1parameter.hpp"
#include <cpluslogger.h>

#include <histLib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include "motionhistogram.hpp"


#if defined(_WIN64) || defined(_WIN32)
#include<windows.h>
#else
#include <unistd.h>
#endif

defuse::MoHist1Xtractor::MoHist1Xtractor(Parameter* _parameter)
{
	int samples = dynamic_cast<MoHist1Parameter *>(_parameter)->samplepoints;

	Directory samplepointdir(dynamic_cast<MoHist1Parameter *>(_parameter)->samplepointsfile);
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
	mDistribution = dynamic_cast<MoHist1Parameter *>(_parameter)->distribution;
	mMaxFrames = dynamic_cast<MoHist1Parameter *>(_parameter)->frames;
	mFrameSelection = dynamic_cast<MoHist1Parameter *>(_parameter)->frameSelection;

	if(dynamic_cast<MoHist1Parameter *>(_parameter)->samplex != -1 && dynamic_cast<MoHist1Parameter *>(_parameter)->sampley != -1)
	{
		mSampleX = dynamic_cast<MoHist1Parameter *>(_parameter)->samplex;
		mSampleY = dynamic_cast<MoHist1Parameter *>(_parameter)->sampley;
		mFixSampling = true;
	}else
	{
		mFixSampling = false;
	}

	maxLenght = 3.0;
	mFilterLength = 5;

	imgOutputPath = dynamic_cast<MoHist1Parameter *>(_parameter)->imgOutputPath;

}

defuse::Features* defuse::MoHist1Xtractor::xtract(VideoBase* _videobase)
{
	cv::VideoCapture video(_videobase->mFile->getFile());

	if (!video.isOpened())
	{
		LOG_FATAL("Fatal error: Video File does not exists." << _videobase->mFile->getFile());
	}

	currentVideoFile = _videobase->mVideoFileName;

	size_t start = cv::getTickCount();
	cv::Mat histogram;
	computeMotionHistogram(video, histogram);
	size_t end = cv::getTickCount();

	double elapsedTime = (end - start) / (cv::getTickFrequency() * 1.0f);
	unsigned int framecount = video.get(CV_CAP_PROP_FRAME_COUNT);
	//LOG_PERFMON(PINTERIM, "Execution Time (One MotionHistogram): " << "\tFrame Count\t" << framecount << "\tElapsed Time\t" << elapsedTime);

	MotionHistogram* motionhistogram = new MotionHistogram(_videobase->mVideoFileName, _videobase->mVideoID, _videobase->mClazz, _videobase->mStartFrameNumber, framecount);
	motionhistogram->mVectors = histogram;

	//video.release();
	return motionhistogram;
}

void defuse::MoHist1Xtractor::computeMotionHistogram(cv::VideoCapture& _video, cv::OutputArray _histogram)
{
	cv::Mat outputhistogram;
	int numframes = static_cast<int>(_video.get(CV_CAP_PROP_FRAME_COUNT));
	int numberOfFramesPerShot = mMaxFrames;

	//the shot has fewer frames than should be used
	if (mMaxFrames > numframes)
	{
		//grapping starts from 0
		numberOfFramesPerShot = numframes;
		//LOG_ERROR("Video Segment is smaller than max frames");
	}

	std::vector<cv::Point2f> prevPoints, currPoints, initPoints;
	int width = _video.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = _video.get(CV_CAP_PROP_FRAME_HEIGHT);


	initPoints = mSamplepoints->getPoints();
	//De-Normalize
	for (int i = 0; i < initPoints.size(); i++)
	{
		cv::Point p(0, 0);
		p.x = initPoints.at(i).x * width;
		p.y = initPoints.at(i).y * height;
		initPoints.at(i) = p;
	}

	currPoints = std::vector<cv::Point2f>(initPoints);

	cv::Mat frame, prevFrame, prevGrayFrame, grayFrame;

	//std::vector<cv::Point2f> prevCorners, corners;
	//unsigned int numframes = _video.get(CV_CAP_PROP_FRAME_COUNT);
	//int samplingrate = 1; //each frame
	//if (mMaxFrames != 0)
	//{
	//	samplingrate = numframes / mMaxFrames; //mFrames per Second
	//}
	//cv::Mat frame, prevGrayFrame, grayFrame;

	int interval = 0;
	int counter = 0;
	std::string videofile = currentVideoFile;
	int limit = 0;

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
			counter++;
			//Capture the current frame
			_video.set(CV_CAP_PROP_POS_FRAMES, iFrame);
			_video.grab();
			_video.retrieve(frame);

			if (frame.empty()) continue;

			//convert the frame to grayscale
			cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);

			//however, we use dense sampling
			//for (int y = 0; y < frame.rows; y += SAMPLEY)
			//	for (int x = 0; x < frame.cols; x += SAMPLEX) {
			//		corners.push_back(Point2f(x, y));
			//	}
			//vector<Point2f> cornersCopy = vector<Point2f>(corners);

			currPoints = std::vector<cv::Point2f>(initPoints);

			if (iFrame > 0)
			{
				//Indicates wheter the flow for the corresponding features has been found
				std::vector<uchar> statusVector;
				statusVector.resize(currPoints.size());
				//Indicates the error for the corresponding feature
				std::vector<float> errorVector;
				errorVector.resize(currPoints.size());


				//Calculate movements between previous and actual frame
				cv::calcOpticalFlowPyrLK(prevGrayFrame, grayFrame, prevPoints, currPoints, statusVector, errorVector);

				cv::Mat motionHist;
				extractMotionHistogram(statusVector, errorVector, prevPoints, currPoints, width, height, motionHist);

				if (iFrame == interval)
					motionHist.copyTo(outputhistogram);
				else
					outputhistogram = outputhistogram + motionHist;

				if (display)
				{
					//visualize STARTs
					cv::Mat frameCopy = frame.clone();
					for (uint i = 0; i < statusVector.size(); i++) 
					{
						if (statusVector[i] != 0) {

							cv::Point p(ceil(prevPoints[i].x), ceil(prevPoints[i].y));
							cv::Point q(ceil(currPoints[i].x), ceil(currPoints[i].y));
							// && errorVector[i] < mFilterLength
							//if (cv::norm(p - q) < int(height / maxLenght))
							//{
								drawLine(frameCopy, p, q, 3);
							//}
						}
					}

					

					std::string framename = videofile + "_Frame_" + std::to_string(iFrame) + ".jpg";
					std::string featurename1 = videofile + "_MV_" + std::to_string(iFrame) + ".jpg";
					
					File f(framename);
					f.setPath(imgOutputPath);
					cv::imwrite(f.getFile(), frame);

					cv::imshow("Frame ", frameCopy);

					File fn(featurename1);
					fn.setPath(imgOutputPath);

					cv::imwrite(fn.getFile(), frameCopy);

					int wait = cv::waitKey(1);
					//visualization ENDs				
				}

				//prevPoints.clear();
			}else
			{
				std::string framename = videofile + "_Frame_" + std::to_string(iFrame) + ".jpg";
				File f(framename);
				f.setPath(imgOutputPath);
				cv::imwrite(f.getFile(), frame);
			}

			prevGrayFrame = grayFrame.clone();
			prevPoints = std::vector<cv::Point2f>(initPoints);
		}


	}else if(mFrameSelection == 1)
	{
		//LOG_ERROR("Number of Frames: " << numframes);
		interval = static_cast<int>(numframes / float(numberOfFramesPerShot));

		if (numframes % mMaxFrames == 0)
			limit = numframes;
		else
			limit = (interval * mMaxFrames);

		for (int iFrame = 1; iFrame < limit; iFrame = iFrame + interval)
		{
			counter++;
			//Capture the current frame
			_video.set(CV_CAP_PROP_POS_FRAMES, iFrame);
			_video.grab();
			_video.retrieve(frame);

			//Capture the previous frame
			_video.set(CV_CAP_PROP_POS_FRAMES, (iFrame - 1));
			_video.grab();
			_video.retrieve(prevFrame);

			if (prevFrame.empty() || frame.empty()) continue;

			//convert the frames to grayscale
			cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);
			cv::cvtColor(prevFrame, prevGrayFrame, CV_BGR2GRAY);

			currPoints = std::vector<cv::Point2f>(initPoints);
			prevPoints = std::vector<cv::Point2f>(initPoints);

			//Indicates wheter the flow for the corresponding features has been found
			std::vector<uchar> statusVector;
			statusVector.resize(currPoints.size());
			//Indicates the error for the corresponding feature
			std::vector<float> errorVector;
			errorVector.resize(currPoints.size());


			//Calculate movements between previous and actual frame
			cv::calcOpticalFlowPyrLK(prevGrayFrame, grayFrame, prevPoints, currPoints, statusVector, errorVector);

			cv::Mat motionHist;
			extractMotionHistogram(statusVector, errorVector, prevPoints, currPoints, width, height, motionHist);

			if (iFrame == 1)
				motionHist.copyTo(outputhistogram);
			else
				outputhistogram = outputhistogram + motionHist;

			if (display)
			{
				//visualize STARTs
				cv::Mat frameCopy = frame.clone();
				for (uint i = 0; i < statusVector.size(); i++)
				{
					if (statusVector[i] != 0) {

						cv::Point p(ceil(prevPoints[i].x), ceil(prevPoints[i].y));
						cv::Point q(ceil(currPoints[i].x), ceil(currPoints[i].y));
						drawLine(frameCopy, p, q, 3);
					}
				}

				std::string currFrameName = videofile + "_Frame_" + std::to_string(iFrame) + ".jpg";
				std::string prevFrameName = videofile + "_Frame_" + std::to_string(iFrame - 1) + ".jpg";
				std::string motionvectorsName = videofile + "_MV_" + std::to_string(iFrame) + ".jpg";

				File curFrameFile(currFrameName);
				curFrameFile.setPath(imgOutputPath);
				File prevFrameFile(prevFrameName);
				prevFrameFile.setPath(imgOutputPath);
				cv::imwrite(curFrameFile.getFile(), frame);
				cv::imwrite(prevFrameFile.getFile(), prevFrame);

				cv::imshow("Frame ", frameCopy);

				File mvFile(motionvectorsName);
				mvFile.setPath(imgOutputPath);
				cv::imwrite(mvFile.getFile(), frameCopy);

				int wait = cv::waitKey(1);
				//visualization ENDs				
			}
		}
	}
	else if (mFrameSelection == 2) //use all frames
	{

		for (int iFrame = 0; iFrame < numframes; iFrame++)
		{
			counter++;
			//Capture the current frame
			_video.set(CV_CAP_PROP_POS_FRAMES, iFrame);
			_video.grab();
			_video.retrieve(frame);

			if (frame.empty()) continue;

			//convert the frame to grayscale
			cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);

			currPoints = std::vector<cv::Point2f>(initPoints);

			if (iFrame > 0)
			{
				//Indicates wheter the flow for the corresponding features has been found
				std::vector<uchar> statusVector;
				statusVector.resize(currPoints.size());
				//Indicates the error for the corresponding feature
				std::vector<float> errorVector;
				errorVector.resize(currPoints.size());


				//Calculate movements between previous and actual frame
				cv::calcOpticalFlowPyrLK(prevGrayFrame, grayFrame, prevPoints, currPoints, statusVector, errorVector);

				cv::Mat motionHist;
				extractMotionHistogram(statusVector, errorVector, prevPoints, currPoints, width, height, motionHist);

				if (iFrame == interval)
					motionHist.copyTo(outputhistogram);
				else
					outputhistogram = outputhistogram + motionHist;

				if (display)
				{
					//visualize STARTs
					cv::Mat frameCopy = frame.clone();
					for (uint i = 0; i < statusVector.size(); i++)
					{
						if (statusVector[i] != 0) {

							cv::Point p(ceil(prevPoints[i].x), ceil(prevPoints[i].y));
							cv::Point q(ceil(currPoints[i].x), ceil(currPoints[i].y));
							drawLine(frameCopy, p, q, 3);
						}
					}



					std::string framename = videofile + "_Frame_" + std::to_string(iFrame) + ".jpg";
					std::string featurename1 = videofile + "_MV_" + std::to_string(iFrame) + ".jpg";

					File f(framename);
					f.setPath(imgOutputPath);
					cv::imwrite(f.getFile(), frame);

					cv::imshow("Frame ", frameCopy);

					File fn(featurename1);
					fn.setPath(imgOutputPath);

					cv::imwrite(fn.getFile(), frameCopy);

					int wait = cv::waitKey(1);
					//visualization ENDs				
				}
			}
			else
			{
				if(display)
				{
					std::string framename = videofile + "_Frame_" + std::to_string(iFrame) + ".jpg";
					File f(framename);
					f.setPath(imgOutputPath);
					cv::imwrite(f.getFile(), frame);
				}

			}

			prevGrayFrame = grayFrame.clone();
			prevPoints = std::vector<cv::Point2f>(initPoints);
		}
	}

	if (display)
	{
		cv::Mat histImage;
		cv::Mat tmpHist = outputhistogram / float(counter);
		drawMotionHistogram(tmpHist, true, histImage);

		cv::imshow("Motion Histogram", histImage);

		std::string histname = videofile + "_MoHist_" + std::to_string(numframes) + ".jpg";

		File f(histname);
		f.setPath(imgOutputPath);
		cv::imwrite(f.getFile(), histImage);
		cv::destroyAllWindows();
	}

	outputhistogram = outputhistogram / float(counter);
	outputhistogram.copyTo(_histogram);
}

void defuse::MoHist1Xtractor::extractMotionHistogram(std::vector<uchar> status, std::vector<float> error, std::vector<cv::Point2f> prevCorner, std::vector<cv::Point2f> corner, int width, int height, cv::Mat& motionHist) const
{
	//float *motionDir = new float[13];
	//float *motionLen = new float[13]; //averaged by MAX(width/height)

	float motionDir[13] = { 0 };
	float motionLen[13] = { 0 };

	uint i;

	float maxWH = MAX(width, height);

	//for (i = 0; i < 13; i++) {
	//	motionDir[i] = 0;
	//	motionLen[i] = 0;
	//}

	//first: perform average filter
	//std::vector<cv::Point2f> cornerCopy = std::vector<cv::Point2f>(corner);
	//for (uint y = 0; y < height; y++) {
	//	for (uint x = 0; x < width; x++) {

	//		int count = 0;
	//		float avgX = 0, avgY = 0;
	//		for (uint yy = MAX(y - 2, 0); yy <= MIN(y + 2, height - 1); yy++) {
	//			for (uint xx = MAX(x - 2, 0); xx <= MIN(x + 2, width - 1); xx++) {
	//				int k = (yy * width) + xx;
	//				avgX += cornerCopy[k].x;
	//				avgY += cornerCopy[k].y;
	//				count++;
	//			}
	//		}

	//		avgX /= count;
	//		avgY /= count;

	//		int i = (y * width) + x;
	//		corner[i].x = avgX;
	//		corner[i].y = avgY;
	//	}
	//}

	for (i = 0; i < status.size(); i++) {
		if (status[i] != 0) {
			cv::Point2f pA = prevCorner[i];
			cv::Point2f pB = corner[i];

			float mvX = pA.x - pB.x;
			float mvY = pA.y - pB.y;

			//Filter to length motion vector, the same as displayed
			//Skip to length motion vectors
			//if (cv::norm(pA - pB) < int(height / maxLenght) && error[i] < mFilterLength)
			//{
			//	continue;
			//}



			float powDist = pow(mvX, 2.0) + pow(mvY, 2.0);
			float len = 0;
			if (powDist > 0)
				len = (float)sqrt(powDist);

			int direction;

			mvX = -mvX;
			/* IMPORTANT NOTE: as Y is already vice-versa-scale,
			we cannot invert it (e.g. mvX=10, mvY=10 means we
			have prediction 45 degree left up (and not right up
			as in usual mathematics coordinates!) */

			float angle = 0;

			if ((mvX < 0.000001 && mvY < 0.000001) || len < 0.000001)
				direction = 0;
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

			motionDir[direction]++;
			motionLen[direction] += len;

		}
	}


	//compute sum of inspected pixels (for normalization)
	float sum = 0;
	for (i = 0; i < 13; i++) {
		sum += motionDir[i];
	}

	//normalize and print histograms
	for (i = 0; i < 13; i++) {
		if (fabs(motionDir[i]) > 0.0001)
			motionLen[i] = (motionLen[i] / motionDir[i]) / maxWH;
		motionDir[i] /= sum;

		//printf("%.2f(%.2f) ", motionDir[i], motionLen[i]);
	}


	cv::Mat histogram;
	cv::Mat direction = cv::Mat(1, (sizeof(motionDir) / sizeof(*motionDir)), CV_32F, motionDir);
	cv::Mat veltmp = cv::Mat(1, (sizeof(motionLen) / sizeof(*motionLen)), CV_32F, motionLen);
	cv::Mat velocity = veltmp(cv::Rect(1,0, (sizeof(motionLen) / sizeof(*motionLen))-1, 1));


	cv::hconcat(direction, velocity, histogram);
	histogram.copyTo(motionHist);

	//std::cout << direction << std::endl;
	//std::cout << velocity << std::endl;
}

void defuse::MoHist1Xtractor::drawLine(cv::Mat& image, cv::Point p, cv::Point q, int scale)
{
	cv::Scalar lineColor = cv::Scalar(200, 255, 170, 255);
	//cv::Scalar lineColor = cv::Scalar(230, 4, 34, 255);
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

double defuse::MoHist1Xtractor::square(int a)
{
	return a * a;
}

void defuse::MoHist1Xtractor::drawMotionHistogram(cv::Mat& motionHist, bool showLength, cv::Mat& histImage)
{
	// Declare histlib instance
	CHistLib Histogram;

	// Customize the display properties
	Histogram.SetPlotColor(cv::Scalar(0x00, 0x00, 0xff));
	Histogram.SetAxisColor(cv::Scalar(0x00, 0x00, 0x00));
	Histogram.SetBackgroundColor(cv::Scalar(0xee, 0xee, 0xee));
	Histogram.SetHistImageHeight(120);
	Histogram.SetDrawSpreadOut(true);

	// Create a row vector for histogram values
	cv::Mat Hist = cv::Mat(motionHist.cols, 1, CV_32S);

	// The ith bin gets i
	for (int i = 0; i < Hist.rows; i++)
	{
		float tmp = motionHist.at<float>(0, i);
		tmp = tmp * 200;
		Hist.at<int>(i, 0) = tmp;
	}

	//for (int i = (Hist.rows / 2); i < Hist.rows; i++)
	//{
	//	Hist.at<int>(i, 0) = motionHist.at<float>(1, i) * 240;
	//}

	cv::Mat image;
	Histogram.DrawHistogram(Hist, image);
	image.copyTo(histImage);
}

/*void showMotionHistogram(cv::Mat& motionHist, bool showLength)
{
	// Draw the histograms Direction and Length
	int hist_w = 854; int hist_h = 480;
	
	/// Establish the number of bins
	int histSize = 25; //13 MD + 13 Length + 1 No direction
	int bin_w = cvRound( (double) hist_w/histSize );
	
	cv::Mat histImage( hist_h, hist_w, cv::CV_8UC3, cv::Scalar( 0,0,0) );

	/// Draw for each dimension
	for( int i = 1; i < histSize; i++ )
	{
	  line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
					   Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
					   Scalar( 255, 0, 0), 2, 8, 0  );
					   
	  line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
					   Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
					   Scalar( 0, 255, 0), 2, 8, 0  );
	}
	
	
}*/
