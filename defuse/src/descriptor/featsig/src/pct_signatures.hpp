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
#ifndef PCT_SIGNATURES_HPP
#define PCT_SIGNATURES_HPP

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

#include "constants.hpp"
#include "pct_sampler.hpp"
#include "pct_clusterizer.hpp"
#include "similarity.hpp"

namespace cv
{
	namespace xfeatures2d
	{
		/** @brief Class implementing PCT (position-color-texture) signatures
			The algorithm is divided to a feature sampler and a clusterizer.
			Feature sampler produces samples at the given set of coordinates initPoints.
			Clusterizer then produces clusters of these samples.
			Resulting clusters are the signature of the input image.

			A signature is an array of SIGNATURE_DIMENSION-dimensional points.
			Used dimensions are:
			x, y position; lab color, contrast, entropy, weight

		constructor:
		@param initSamplingPoints list of coordinates used to produce feature samples
		@param initSampleCount number of used sampling points (must be less or equal to initSamplingPoints.size())
		@param initSeedCount number of initial clusterization seeds (must be less than initSampleCount),

		sampler:
		@param grayscaleBits bit depth of grayscale image used to sample contrast and entropy
		@param windowRadius radius of the window centered around a sampling point, used to sample contrast and entropy
		@param weightX weight of the sampled property
		@param translationX translation of the sampled property

		clusterizer:
		@param iterationCount number of clusterization iterations
		@param maxClustersCount maximal number of produced clusters
		@param clusterMinSize minimal size of a cluster
		@param joiningDistance distance in which two clusters are merged together
		@param dropThreshold cluster size drop threshold (clusters smaller than this number are dropped)
		@param LpNorm norm used in computing distances
		
		*/
		class CV_EXPORTS_W PCTSignatures : public Algorithm
		{
		public:
			CV_WRAP enum PointDistribution{ RANDOM , REGULAR/*, GAUSSIAN*/ };

			CV_WRAP static Ptr<PCTSignatures> create(
				const int initSampleCount = 10000,
				const int initSeedCount = 50,
				const PointDistribution pointDistribution = PointDistribution::RANDOM);
			
			CV_WRAP static Ptr<PCTSignatures> create(
				const std::vector<Point2f> initSamplingPoints, 
				const int initSeedCount);

			CV_WRAP static Ptr<PCTSignatures> create(
				const std::vector<Point2f> initSamplingPoints,
				const int initSampleCount, 
				const int initSeedCount);


			
			CV_WRAP virtual void computeSignature(InputArray image, OutputArray signature) const = 0;

			CV_WRAP virtual void computeSignatures(const std::vector<Mat> &images, std::vector<Mat> &signatures) const = 0;


			CV_WRAP static void drawSignature(const cv::InputArray source, const cv::InputArray signature, cv::OutputArray result);

			CV_WRAP static float computeQuadraticFormDistance(const cv::InputArray signature0, const cv::InputArray signature1, const pct_signatures::Similarity &similarity = pct_signatures::HeuristicSimilarity());
			
			CV_WRAP static void computeQuadraticFormDistances(const cv::Mat &sourceSignature, const std::vector<Mat> &imageSignatures, 
				std::vector<float> &distances, const pct_signatures::Similarity &similarity = pct_signatures::HeuristicSimilarity());

			CV_WRAP static void generateInitPoints(std::vector<Point2f> &initPoints, const size_t count, PCTSignatures::PointDistribution pointsDistribution);


			/**** sampler ****/
			CV_WRAP virtual int getSampleCount() const = 0;
			CV_WRAP virtual int getGrayscaleBits() const = 0;
			CV_WRAP virtual int getWindowRadius() const = 0;
			CV_WRAP virtual double getWeightX() const = 0;
			CV_WRAP virtual double getWeightY() const = 0;
			CV_WRAP virtual double getWeightL() const = 0;
			CV_WRAP virtual double getWeightA() const = 0;
			CV_WRAP virtual double getWeightB() const = 0;
			CV_WRAP virtual double getWeightConstrast() const = 0;
			CV_WRAP virtual double getWeightEntropy() const = 0;

			CV_WRAP virtual void setSampleCount(int sampleCount) = 0;
			CV_WRAP virtual void setGrayscaleBits(int grayscaleBits) = 0;
			CV_WRAP virtual void setWindowRadius(int radius) = 0;
			CV_WRAP virtual void setWeightX(double weight) = 0;
			CV_WRAP virtual void setWeightY(double weight) = 0;
			CV_WRAP virtual void setWeightL(double weight) = 0;
			CV_WRAP virtual void setWeightA(double weight) = 0;
			CV_WRAP virtual void setWeightB(double weight) = 0;
			CV_WRAP virtual void setWeightContrast(double weight) = 0;
			CV_WRAP virtual void setWeightEntropy(double weight) = 0;

			CV_WRAP virtual void setWeight(int idx, double value) = 0;
			CV_WRAP virtual void setWeights(const std::vector<double> &weights) = 0;
			CV_WRAP virtual void setTranslation(int idx, double value) = 0;
			CV_WRAP virtual void setTranslations(const std::vector<double> &translations) = 0;


			/**** clusterizer ****/
			CV_WRAP virtual int	getInitSeedCount() const = 0;
			CV_WRAP virtual int	getIterationCount() const = 0;
			CV_WRAP virtual int	getMaxClustersCount() const = 0;
			CV_WRAP virtual int	getClusterMinSize() const = 0;
			CV_WRAP virtual float getJoiningDistance() const = 0;
			CV_WRAP virtual float getDropThreshold() const = 0;
			CV_WRAP virtual float getLpNorm() const = 0;

			CV_WRAP virtual void setInitSeedCount(int initSeedCount) = 0;
			CV_WRAP virtual void setIterationCount(int iterationCount) = 0;
			CV_WRAP virtual void setMaxClustersCount(int maxClustersCount) = 0;
			CV_WRAP virtual void setClusterMinSize(int clusterMinSize) = 0;
			CV_WRAP virtual void setJoiningDistance(float joiningDistance) = 0;
			CV_WRAP virtual void setDropThreshold(float dropThreshold) = 0;
			CV_WRAP virtual void setLpNorm(float LpNorm) = 0;

		private:
			//MODIFIED PCTSignatures::computePartialSQFD
			static float computePartialSQFD(const cv::Mat &signature0, const cv::Mat &signature1, const pct_signatures::Similarity &similarity);

			
		};
	}
}

#endif
