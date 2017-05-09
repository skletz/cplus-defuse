#include "pct_sampler.hpp"

namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{
			class PCTSampler_Impl : public PCTSampler
			{
			private:
				const std::vector<cv::Point2f> &mInitPoints;
				int mSampleCount;
				int mGrayscaleBits;
				int mWindowRadius;
				std::vector<double> mWeights;
				std::vector<double> mTranslations;

				
			public:

				PCTSampler_Impl(
					const std::vector<cv::Point2f>	&initPoints,
					int						sampleCount = 500,
					int						grayscaleBits = 4,
					int						windowRadius = 5)
					: mInitPoints(initPoints),
					mSampleCount(sampleCount),
					mGrayscaleBits(grayscaleBits),
					mWindowRadius(windowRadius)
				{
					if (mSampleCount > initPoints.size())
					{
						mSampleCount = static_cast<int>(initPoints.size());
					}

					// Initialize weights and translation vectors to neutral items.
					for (int i = 0; i < SIGNATURE_DIMENSION; i++)
					{
						mWeights.push_back(1.0);
						mTranslations.push_back(0.0);
					}
				}




				/**** Acessors ****/

				int getSampleCount() const		{ return mSampleCount; }
				int getGrayscaleBits() const	{ return mGrayscaleBits; }
				int getWindowRadius() const		{ return mWindowRadius; }

				double getWeightX() const				{ return mWeights[X_IDX]; }
				double getWeightY() const				{ return mWeights[Y_IDX]; }
				double getWeightL() const				{ return mWeights[L_IDX]; }
				double getWeightA() const				{ return mWeights[A_IDX]; }
				double getWeightB() const				{ return mWeights[B_IDX]; }
				double getWeightConstrast() const		{ return mWeights[CONTRAST_IDX]; }
				double getWeightEntropy() const			{ return mWeights[ENTROPY_IDX]; }

				
				void setSampleCount(int sampleCount)		{ mSampleCount = sampleCount; }
				void setGrayscaleBits(int grayscaleBits)	{ mGrayscaleBits = grayscaleBits; }
				void setWindowRadius(int windowRadius)		{ mWindowRadius = windowRadius; }

				void setWeightX(double weight)			{ mWeights[X_IDX] = weight; }
				void setWeightY(double weight)			{ mWeights[Y_IDX] = weight; }
				void setWeightL(double weight)			{ mWeights[L_IDX] = weight; }
				void setWeightA(double weight)			{ mWeights[A_IDX] = weight; }
				void setWeightB(double weight)			{ mWeights[B_IDX] = weight; }
				void setWeightContrast(double weight)	{ mWeights[CONTRAST_IDX] = weight; }
				void setWeightEntropy(double weight)	{ mWeights[ENTROPY_IDX] = weight; }

				
				void setWeight(int idx, double value)
				{
					mWeights[idx] = value;
				}


				void setWeights(const std::vector<double> &weights)
				{
					if (weights.size() != mWeights.size())
					{
						CV_Error_(CV_StsUnmatchedSizes, ("Invalid weights dimension %d (max %d)", weights.size(), mWeights.size()));
					}
					else
					{
						for (int i = 0; i < mWeights.size(); ++i)
						{
							mWeights[i] = weights[i];
						}
					}
				}


				void setTranslation(int idx, double value)
				{
					mTranslations[idx] = value;
				}


				void setTranslations(const std::vector<double> &translations)
				{
					if (translations.size() != mTranslations.size())
					{
						CV_Error_(CV_StsUnmatchedSizes, ("Invalid translations dimension %d (max %d)", translations.size(), mTranslations.size()));
					}
					else
					{
						for (int i = 0; i < mTranslations.size(); ++i)
						{
							mTranslations[i] = translations[i];
						}
					}
				}



				virtual void sample(const cv::InputArray &_image, cv::OutputArray &_samples) const
				{
					// check init points size
					if (mInitPoints.size() < mSampleCount)
					{
						CV_Error_(CV_StsBadArg, 
							("Insufficient initial points for sampling. Total %d samples requested but only %d initial points provided."
							, mSampleCount, mInitPoints.size()));
					}

					// prepare matrices
					cv::Mat image = _image.getMat();
					_samples.create(static_cast<int>(mSampleCount), SIGNATURE_DIMENSION, CV_32F);
					cv::Mat samples = _samples.getMat();
					GrayscaleBitmap grayscaleBitmap(image, mGrayscaleBits);

					// debug
					//cv::Mat gs;
					//grayscaleBitmap.convertToMat(gs, true);


					for (int iSample = 0; iSample < mSampleCount; iSample++)
					{
						int x = static_cast<int>(mInitPoints[iSample].x * (image.cols - 1) + 0.5);
						int y = static_cast<int>(mInitPoints[iSample].y * (image.rows - 1) + 0.5);

						samples.at<float>(iSample, X_IDX) = static_cast<float>(static_cast<double>(x) / static_cast<double>(image.cols) * mWeights[X_IDX] + mTranslations[X_IDX]);	// x, y normalized
						samples.at<float>(iSample, Y_IDX) = static_cast<float>(static_cast<double>(y) / static_cast<double>(image.rows) * mWeights[Y_IDX] + mTranslations[Y_IDX]);

						Mat rgbPixel(image, Rect(x, y, 1, 1));				// get Lab pixel color
						Mat labPixel;										// placeholder -> TODO: optimize
						rgbPixel.convertTo(rgbPixel, CV_32FC3, 1.0 / 255);
						cvtColor(rgbPixel, labPixel, COLOR_BGR2Lab);
						Vec3f labColor = labPixel.at<Vec3f>(0, 0);			// end

						samples.at<float>(iSample, L_IDX) = static_cast<float>(std::floor(labColor[0] + 0.5) / L_COLOR_RANGE * mWeights[L_IDX] + mTranslations[L_IDX]);	// Lab color normalized
						samples.at<float>(iSample, A_IDX) = static_cast<float>(std::floor(labColor[1] + 0.5) / A_COLOR_RANGE * mWeights[A_IDX] + mTranslations[A_IDX]);
						samples.at<float>(iSample, B_IDX) = static_cast<float>(std::floor(labColor[2] + 0.5) / B_COLOR_RANGE * mWeights[B_IDX] + mTranslations[B_IDX]);

						double contrast = 0.0, entropy = 0.0;
						grayscaleBitmap.getContrastEntropy(x, y, contrast, entropy, mWindowRadius);
						samples.at<float>(iSample, CONTRAST_IDX)
							= static_cast<float>(contrast / SAMPLER_CONTRAST_NORMALIZER * mWeights[CONTRAST_IDX] + mTranslations[CONTRAST_IDX]);			// contrast
						samples.at<float>(iSample, ENTROPY_IDX)
							= static_cast<float>(entropy / SAMPLER_ENTROPY_NORMALIZER * mWeights[ENTROPY_IDX] + mTranslations[ENTROPY_IDX]);				// entropy
					}

				}
			};



			

			Ptr<PCTSampler> PCTSampler::create(
				const std::vector<cv::Point2f>	&initPoints,
				int						sampleCount,
				int						grayscaleBits,
				int						windowRadius)
			{
				return makePtr<PCTSampler_Impl>(initPoints, sampleCount, grayscaleBits, windowRadius);
			}



		}
	}
}
