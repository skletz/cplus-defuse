#ifndef _DEFUSE_DYSIGXTRACTOR_HPP_
#define _DEFUSE_DYSIGXTRACTOR_HPP_

#include "features.hpp"
#include "parameter.hpp"
#include "videobase.hpp"
#include "xtractor.hpp"
#include "samplepoints.hpp"

#include <opencv2/opencv.hpp>

namespace defuse {

	class DYSIGXtractor : public Xtractor
	{
	public:

		enum  class IDX { X = 0, Y, L, A, B, C, E, MD, WEIGHT, DIMS };
		
		enum FrameSelection { FramesPerVideo, FramesPerSecond };

		//Default Setting of flow-based sampling
		FrameSelection mFrameSelection;

		int mMaxFrames;

		bool mResetTracking;

		//Default Setting of the k-means clustering
		int mInitSeeds;

		int mMaxClusters;

		int mIterations;

		int mMinimalClusterSize;

		float mMinimalDistance;

		float mClusterDropThreshold;

		//Stored samplepoints, used as random distribution
		SamplePoints* mSamplepoints;

		SamplePoints::Distribution mDistribution;

		//Contrast, Entropy - Grayscale Bitmap
		//TODO Enable changes of grayscale bits and window radius via constructor
		int mGrayscaleBits;

		int mWindowRadius;

		std::vector<double> mWeights;

		std::vector<double> mTranslations;

		/**
		 * \brief Default Constructor
		 * \param _frameSelection = DYSIGXtractor::FrameSelection
		 * \param _maxFrames = 5
		 * \param _resetTracking = true
		 * \param _initSeeds = 100
		 * \param _initialCentroids = 10
		 * \param _iterations = 5
		 * \param _minClusterSize = 2
		 * \param _minDistance = 0.01
		 * \param _dropThreshold = 0
		 * \param _samplepoints = nullptr
		 * \param _distribution = SamplePoints::Distribution::REGULAR
		 * \param _grayscaleBits = 5
		 * \param _windowRadius = 4
		 */
		DYSIGXtractor(
			DYSIGXtractor::FrameSelection _frameSelection = DYSIGXtractor::FrameSelection::FramesPerVideo, 
			int _maxFrames = 5, 
			bool _resetTracking = true, 
			int _initSeeds = 100,
			int _initialCentroids = 10, 
			int _iterations = 5, 
			int _minClusterSize = 2, 
			float _minDistance = 0.01,
			float _dropThreshold = 0.0, 
			SamplePoints::Distribution _distribution = SamplePoints::Distribution::REGULAR, 
			SamplePoints* _samplepoints = nullptr,
			int _grayscaleBits = 5, 
			int _windowRadius = 4);

		/**
		 * \brief Constructor using a samplepoint directory
		 * \param _maxFrames 
		 * \param _initSeeds 
		 * \param _initialCentroids 
		 * \param _samplepointdir 
		 * \param _distribution 
		 */
		DYSIGXtractor(
			int _maxFrames, int _initSeeds,
			int _initialCentroids, std::string _samplepointdir = "", 
			SamplePoints::Distribution _distribution = SamplePoints::Distribution::REGULAR);

		/**
		 * \brief 
		 * \param _videobase 
		 * \return 
		 */
		Features* xtract(VideoBase* _videobase) override;

		std::string get() const;

		std::string getFilename() const;
		

	private:
		void computeSignatures(cv::VideoCapture& _video, cv::OutputArray _signatures) const;

		void getSamples(cv::Mat& frame, std::vector<cv::Point2f> initPoints, cv::Mat& samples) const;
		void getMotionDirection(std::vector<uchar>& statusVector, std::vector<float>& errorVector, std::vector<cv::Point2f>& prevPoints, std::vector<cv::Point2f>& currPoints, int height, int width, cv::Mat& out) const;
		void getTemporalSamples(cv::Mat& samples, cv::Mat& signatures) const;


		void joinCloseClusters(cv::Mat clusters) const;
		void dropLightPoints(cv::Mat& clusters) const;
		int findClosestCluster(cv::Mat &clusters, cv::Mat &points, int pointIdx) const;
		void normalizeWeights(cv::Mat &clusters) const;
		void singleClusterFallback(const cv::Mat &points, cv::Mat &clusters) const;
		void cropClusters(cv::Mat &clusters) const;

		void draw2DSampleSignature(const cv::InputArray _source, const cv::InputArray _signature, cv::OutputArray& _result) const;
		double square(int a) const;
		void drawLine(cv::Mat& image, cv::Point p, cv::Point q, int scale) const;

		void draw2DFSSignature(const cv::InputArray _source, const cv::InputArray _signature, cv::OutputArray& _result) const;
		void drawSignatureMotionDirection(const cv::InputArray& _tempSig, cv::OutputArray _result, int _width, int _height) const;

		float computeL2(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2) const;
	};
}

#endif //_DEFUSE_DYSIGXTRACTOR_HPP_