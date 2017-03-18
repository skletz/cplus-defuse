#include "mohist1xtractor.hpp"
#include "mohist1parameter.hpp"
#include <cpluslogger.h>

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
	mSampleX = dynamic_cast<MoHist1Parameter *>(_parameter)->samplex;
	mSampleY = dynamic_cast<MoHist1Parameter *>(_parameter)->sampley;
	mFrames = dynamic_cast<MoHist1Parameter *>(_parameter)->frames;
}

defuse::Features* defuse::MoHist1Xtractor::xtract(VideoBase* _videobase)
{
	cv::VideoCapture video(_videobase->mFile->getFile());

	if (!video.isOpened())
	{
		LOG_FATAL("Fatal error: Video File does not exists." << _videobase->mFile->getFile());
	}


	size_t start = cv::getTickCount();
	cv::Mat histogram;
	computeMotionHistogram(video, histogram);
	size_t end = cv::getTickCount();

	double elapsedTime = (end - start) / (cv::getTickFrequency() * 1.0f);
	unsigned int framecount = video.get(CV_CAP_PROP_FRAME_COUNT);
	LOG_PERFMON(PINTERIM, "Execution Time (One MotionHistogram): " << "\tFrame Count\t" << framecount << "\tElapsed Time\t" << elapsedTime);

	MotionHistogram* motionhistogram = new MotionHistogram(_videobase->mVideoID, _videobase->mClazz, _videobase->mStartFrameNumber, framecount);
	motionhistogram->mVectors = histogram;

	//video.release();
	return motionhistogram;
}

void defuse::MoHist1Xtractor::computeMotionHistogram(cv::VideoCapture& _video, cv::OutputArray _histogram)
{
	//bool slow(false);

	bool display = false;
	float filter = 5;

	cv::Mat outputhistogram;

	int SAMPLEX = mSampleX, SAMPLEY = mSampleY;

	std::vector<cv::Point2f> prevCorners, corners;

	unsigned int numframes = _video.get(CV_CAP_PROP_FRAME_COUNT);

	int samplingrate = 1; //each frame
	if (mFrames != 0)
	{
		samplingrate = numframes / mFrames; //mFrames per Second
	}

	cv::Mat frame, prevGrayFrame, grayFrame;
	for (unsigned int j = 0; j < numframes; j = j + samplingrate)
	{
		_video >> frame;

		corners.clear();

		cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);

		for (int y = 0; y < frame.rows; y += SAMPLEY)
		{
			for (int x = 0; x < frame.cols; x += SAMPLEX) {

				corners.push_back(cv::Point2f(x, y));
			}
		}

		std::vector<cv::Point2f> cornersCopy = std::vector<cv::Point2f>(corners);

		if (j > 0)
		{

			std::vector<uchar> status;
			std::vector<float> error;

			int width = frame.cols;
			int height = frame.rows;


			//Calculate movements between previous and actual frame
			cv::calcOpticalFlowPyrLK(prevGrayFrame, grayFrame, prevCorners, corners, status, error);

			cv::Mat motionHist;
			extractMotionHistogram(status, prevCorners, corners, width / SAMPLEX, height / SAMPLEY, motionHist);

			if (j == 1)
				motionHist.copyTo(outputhistogram);
			else
				outputhistogram = outputhistogram + motionHist;

			if (display)
			{
				//visualize STARTs
				cv::Mat frameCopy = frame.clone();
				for (uint i = 0; i < status.size(); i++) {
					if (status[i] != 0) {

						cv::Point p(ceil(prevCorners[i].x), ceil(prevCorners[i].y));
						cv::Point q(ceil(corners[i].x), ceil(corners[i].y));
						//arrowedLine(frameCopy, p0, p1, cv::Scalar(255, 255, 255, 0), 1, CV_AA, 0,0.5);


						if (cv::norm(p - q) < int(height / 10.0) && error[i] < filter)
						{
							drawLine(frameCopy, p, q, 3);
						}

					}
				}

				cv::imshow("FrameCopy", frameCopy);
				//visualization ENDs				
			}

			prevCorners.clear();
		}

		prevGrayFrame = grayFrame.clone();
		prevCorners = std::vector<cv::Point2f>(cornersCopy);

		int wait = cv::waitKey(1);

	}

	outputhistogram = outputhistogram / float(numframes);

	outputhistogram.copyTo(_histogram);
}

void defuse::MoHist1Xtractor::extractMotionHistogram(std::vector<uchar> status, std::vector<cv::Point2f> prevCorner, std::vector<cv::Point2f> corner, int width, int height, cv::Mat& motionHist)
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
	std::vector<cv::Point2f> cornerCopy = std::vector<cv::Point2f>(corner);
	for (uint y = 0; y < height; y++) {
		for (uint x = 0; x < width; x++) {

			int count = 0;
			float avgX = 0, avgY = 0;
			for (uint yy = MAX(y - 2, 0); yy <= MIN(y + 2, height - 1); yy++) {
				for (uint xx = MAX(x - 2, 0); xx <= MIN(x + 2, width - 1); xx++) {
					int k = (yy * width) + xx;
					avgX += cornerCopy[k].x;
					avgY += cornerCopy[k].y;
					count++;
				}
			}

			avgX /= count;
			avgY /= count;

			int i = (y * width) + x;
			corner[i].x = avgX;
			corner[i].y = avgY;
		}
	}

	for (i = 0; i < status.size(); i++) {
		if (status[i] != 0) {
			cv::Point2f pA = prevCorner[i];
			cv::Point2f pB = corner[i];

			float mvX = pA.x - pB.x;
			float mvY = pA.y - pB.y;

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
	cv::Mat velocity = cv::Mat(1, (sizeof(motionLen) / sizeof(*motionLen)), CV_32F, motionLen);

	cv::vconcat(direction, velocity, histogram);
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
