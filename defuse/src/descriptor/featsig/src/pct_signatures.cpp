#include "pct_signatures.hpp"
#include <iostream>

using namespace cv::xfeatures2d::pct_signatures;

namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{
			class PCTSignatures_Impl : public PCTSignatures
			{
			public:
				PCTSignatures_Impl(std::vector<Point2f> initPoints, int initSampleCount, int initSeedCount) : mInitSamplingPoints(initPoints)
				{
					mSampler = PCTSampler::create(mInitSamplingPoints, initSampleCount);
					mClusterizer = PCTClusterizer::create(initSeedCount);
				}

				void computeSignature(InputArray image, OutputArray signature) const;

				void computeSignatures(const std::vector<Mat> &images, std::vector<Mat> &signatures) const;

				
				/**** sampler ****/
				int getSampleCount() const				{ return mSampler->getSampleCount(); }
				int getGrayscaleBits() const			{ return mSampler->getGrayscaleBits(); }
				int getWindowRadius() const				{ return mSampler->getWindowRadius(); }
				double getWeightX() const						{ return mSampler->getWeightX(); }
				double getWeightY() const						{ return mSampler->getWeightY(); }
				double getWeightL() const						{ return mSampler->getWeightL(); }
				double getWeightA() const						{ return mSampler->getWeightA(); }
				double getWeightB() const						{ return mSampler->getWeightB(); }
				double getWeightConstrast() const				{ return mSampler->getWeightConstrast(); }
				double getWeightEntropy() const					{ return mSampler->getWeightEntropy(); }

				void setSampleCount(int sampleCount)	{ mSampler->setSampleCount(sampleCount); }
				void setGrayscaleBits(int grayscaleBits){ mSampler->setGrayscaleBits(grayscaleBits); }
				void setWindowRadius(int windowRadius)	{ mSampler->setWindowRadius(windowRadius); }
				void setWeightX(double weight)					{ mSampler->setWeightX(weight); }
				void setWeightY(double weight)					{ mSampler->setWeightY(weight); }
				void setWeightL(double weight)					{ mSampler->setWeightL(weight); }
				void setWeightA(double weight)					{ mSampler->setWeightA(weight); }
				void setWeightB(double weight)					{ mSampler->setWeightB(weight); }
				void setWeightContrast(double weight)			{ mSampler->setWeightContrast(weight); }
				void setWeightEntropy(double weight)			{ mSampler->setWeightEntropy(weight); }

				void setWeight(int idx, double value)					{ mSampler->setWeight(idx, value); }
				void setWeights(const std::vector<double> &weights)				{ mSampler->setWeights(weights); }
				void setTranslation(int idx, double value)				{ mSampler->setTranslation(idx, value); }
				void setTranslations(const std::vector<double> &translations)	{ mSampler->setTranslations(translations); }


				/**** clusterizer ****/
				int	getIterationCount() const				{ return mClusterizer->getIterationCount(); }
				int	getInitSeedCount() const				{ return mClusterizer->getInitSeedCount(); }
				int	getMaxClustersCount() const				{ return mClusterizer->getMaxClustersCount(); }
				int	getClusterMinSize() const				{ return mClusterizer->getClusterMinSize(); }
				float getJoiningDistance() const					{ return mClusterizer->getJoiningDistance(); }
				float getDropThreshold() const						{ return mClusterizer->getDropThreshold(); }
				float getLpNorm() const								{ return mClusterizer->getLpNorm(); }

				void setIterationCount(int iterations)		{ mClusterizer->setIterationCount(iterations); }
				void setInitSeedCount(int initSeeds)		{ mClusterizer->setInitSeedCount(initSeeds); }
				void setMaxClustersCount(int maxClusters)	{ mClusterizer->setMaxClustersCount(maxClusters); }
				void setClusterMinSize(int clusterMinSize)	{ mClusterizer->setClusterMinSize(clusterMinSize); }
				void setJoiningDistance(float joiningDistance)		{ mClusterizer->setJoiningDistance(joiningDistance); }
				void setDropThreshold(float dropThreshold)			{ mClusterizer->setDropThreshold(dropThreshold); }
				void setLpNorm(float LpNorm)						{ mClusterizer->setLpNorm(LpNorm); }

			private:
				std::vector<Point2f> mInitSamplingPoints;
				Ptr<PCTSampler> mSampler;
				Ptr<PCTClusterizer> mClusterizer;
			};


			class Parallel_computeSignatures : public ParallelLoopBody
			{
			private:
				const PCTSignatures &mPctSignaturesAlgorithm;
				const std::vector<Mat> &mImages;
				std::vector<Mat> &mSignatures;

			public:
				Parallel_computeSignatures(const PCTSignatures& pctSignaturesAlgorithm, const std::vector<Mat> &images, std::vector<Mat> &signatures)
					: mPctSignaturesAlgorithm(pctSignaturesAlgorithm), mImages(images), mSignatures(signatures)
				{
					mSignatures.resize(images.size());
				}

				void operator()(const Range &range) const
				{
					for (int i = range.start; i < range.end; i++)
					{
						mPctSignaturesAlgorithm.computeSignature(mImages[i], mSignatures[i]);
					}
				}
			};

			class Parallel_computeSQFDs : public ParallelLoopBody
			{
			private:
				const Mat &mSourceSignature;
				const std::vector<Mat> &mImageSignatures;
				std::vector<float> &mDistances;
				const Similarity &mSimilarity;

			public:
				Parallel_computeSQFDs(const Mat &sourceSignature, const std::vector<Mat> &imageSignatures, std::vector<float> &distances,
					const Similarity &similarity)
					: mSourceSignature(sourceSignature), mImageSignatures(imageSignatures), mDistances(distances), mSimilarity(similarity)
				{
					mDistances.resize(imageSignatures.size());
				}

				void operator()(const Range &range) const
				{
					for (int i = range.start; i < range.end; i++)
					{
						mDistances[i] = PCTSignatures::computeQuadraticFormDistance(mSourceSignature, mImageSignatures[i], mSimilarity);
					}
				}
			};


			void PCTSignatures_Impl::computeSignature(InputArray _image, OutputArray _signature) const
			{
				if (_image.empty())
				{
					return;
				}

				Mat image = _image.getMat();
				CV_Assert(image.depth() == CV_8U);	// uchar

				// TODO: OpenCL
				//if (ocl::useOpenCL())
				//{
				//}

				// sample features
				Mat samples;
				mSampler->sample(image, samples);

				// kmeans clusterize, use feature samples, produce signature clusters
				Mat signature;
				mClusterizer->clusterize(samples, signature);

				
				// set result
				_signature.create(signature.size(), signature.type());
				Mat result = _signature.getMat();
				signature.copyTo(result);
			}

			void PCTSignatures_Impl::computeSignatures(const std::vector<Mat> &images, std::vector<Mat> &signatures) const
			{
				parallel_for_(Range(0, static_cast<int>(images.size())), Parallel_computeSignatures(*this, images, signatures));
			}


		} // end of namespace pct_signatures



		Ptr<PCTSignatures> PCTSignatures::create(
			const int initSampleCount,
			const int initSeedCount,
			const PointDistribution pointDistribution)
		{
			std::vector<Point2f> initPoints;
			generateInitPoints(initPoints, initSampleCount, pointDistribution);
			Ptr<PCTSignatures> pctSignatures = create(initPoints, initSampleCount, initSeedCount);
			return pctSignatures;
		}

		Ptr<PCTSignatures> PCTSignatures::create(
			const std::vector<Point2f> initPoints,
			const int initSeedCount)
		{
			return create(initPoints, static_cast<int>(initPoints.size()), initSeedCount);
		}

		Ptr<PCTSignatures> PCTSignatures::create(
			const std::vector<Point2f> initPoints,
			const int initSampleCount,
			const int initSeedCount)
		{
			return makePtr<PCTSignatures_Impl>(initPoints, initSampleCount, initSeedCount);
		}


		void PCTSignatures::drawSignature(const InputArray _source, const InputArray _signature, OutputArray _result)
		{
			// check source
			if (_source.empty())
			{
				return;
			}
			Mat source = _source.getMat();

			// create result
			_result.create(source.size(), source.type());
			Mat result = _result.getMat();
			//TODO original copies the source to signatures
			//source.copyTo(result);

			// check signature
			if (_signature.empty())
			{
				return;
			}
			Mat signature = _signature.getMat();
			if (signature.type() != CV_32F || signature.cols != SIGNATURE_DIMENSION)
			{
				CV_Error_(CV_StsBadArg, ("Invalid signature format. Type must be CV_32F and signature.cols must be %d.", SIGNATURE_DIMENSION));
			}

			// compute max radius = one eigth of the length of the shorter dimension
			int maxRadius = ((source.rows < source.cols) ? source.rows : source.cols) / 8;


			// draw signature
			for (int i = 0; i < signature.rows; i++)
			{
				Vec3f labColor(
					signature.at<float>(i, L_IDX) * L_COLOR_RANGE,		// get Lab pixel color
					signature.at<float>(i, A_IDX) * A_COLOR_RANGE,		// placeholder -> TODO: optimize
					signature.at<float>(i, B_IDX) * B_COLOR_RANGE);
				Mat labPixel(1, 1, CV_32FC3);
				labPixel.at<Vec3f>(0, 0) = labColor;
				Mat rgbPixel;
				cvtColor(labPixel, rgbPixel, COLOR_Lab2BGR);
				rgbPixel.convertTo(rgbPixel, CV_8UC3, 255);
				Vec3b rgbColor = rgbPixel.at<Vec3b>(0, 0);				// end

				// precompute variables
				Point center(static_cast<int>(signature.at<float>(i, X_IDX) * static_cast<float>(source.cols)), static_cast<int>(signature.at<float>(i, Y_IDX) * static_cast<float>(source.rows)));
				int radius(static_cast<int>(static_cast<float>(maxRadius) * signature.at<float>(i, WEIGHT_IDX)));
				Vec3b borderColor(0, 0, 0);
				int borderThickness(1);

				// draw filled circle
				circle(result, center, radius, rgbColor, -1);
				// draw circle outline
				circle(result, center, radius, borderColor, borderThickness);
			}
		}

		
		float PCTSignatures::computeQuadraticFormDistance(const InputArray _signature0, const InputArray _signature1, const Similarity &similarity)
		{
			// check input
			if (_signature0.empty() || _signature1.empty())
			{
				CV_Error(CV_StsBadArg, "Empty signature!");
			}

			Mat signature0 = _signature0.getMat();
			Mat signature1 = _signature1.getMat();

			if (signature0.cols != SIGNATURE_DIMENSION || signature1.cols != SIGNATURE_DIMENSION)
			{
				CV_Error_(CV_StsBadArg, ("Signature dimension must be %d!", SIGNATURE_DIMENSION));
			}

			if (signature0.rows <= 0 || signature1.rows <= 0)
			{
				CV_Error(CV_StsBadArg, "Signature count must be greater than 0!");
			}

			// compute sqfd
			float result = 0;
			result += computePartialSQFD(signature0, signature0, similarity);
			result += computePartialSQFD(signature1, signature1, similarity);
			result -= computePartialSQFD(signature0, signature1, similarity) * 2;

			return sqrt(result);
		}

		void PCTSignatures::computeQuadraticFormDistances(const Mat &sourceSignature, const std::vector<Mat> &imageSignatures, std::vector<float> &distances,
			const pct_signatures::Similarity &similarity)
		{
			parallel_for_(Range(0, static_cast<int>(imageSignatures.size())), Parallel_computeSQFDs(sourceSignature, imageSignatures, distances, similarity));
		}

		float PCTSignatures::computePartialSQFD(const Mat &signature0, const Mat &signature1, const Similarity& similarity)
		{
			float result = 0;
			for (int i = 0; i < signature0.rows; i++)
			{
				for (int j = 0; j < signature1.rows; j++)
				{
					result += signature0.at<float>(i, WEIGHT_IDX) * signature1.at<float>(j, WEIGHT_IDX) * similarity(signature0, i, signature1, j);
				}
			}
			return result;
		}

		void PCTSignatures::generateInitPoints(std::vector<Point2f> &initPoints, const size_t count, PCTSignatures::PointDistribution pointsDistribution)
		{
			RNG random;
			random.state = getTickCount();
			initPoints.resize(count);

			switch (pointsDistribution)
			{
			case PCTSignatures::PointDistribution::RANDOM:
				for (int i = 0; i < count; i++)
				{
					initPoints[i] = (Point2f(random.uniform(static_cast<float>(0.0), static_cast<float>(1.0)), random.uniform(static_cast<float>(0.0), static_cast<float>(1.0))));
				}
				break;
			//ADDED option for regular choosen keypoints
			case PCTSignatures::PointDistribution::REGULAR:
			{
				float result = static_cast<float>(sqrt(count));
				//initPoints.reserve(count);

				for (int i = 0; i < (int)result; i++)
				{
					for (int j = 0; j < (int)result; j++)
					{
						initPoints[i*result + j] = Point2f(static_cast<float>(i / result), static_cast<float>(j / result));
					}
					
				}
				break;
			}

				//case PCTSignatures::PointDistribution::GAUSSIAN:
				//	break;
			default:
				CV_Error(CV_StsNotImplemented, "Generation of this init point distribution is not implemented!");
				break;
			}
		}
		

	} // end of namespace xfeatures2d
} // end of namespace cv
