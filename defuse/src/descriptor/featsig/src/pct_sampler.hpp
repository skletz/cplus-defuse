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
#ifndef PCT_SIGNATURES_SAMPLER_HPP
#define PCT_SIGNATURES_SAMPLER_HPP

#include "opencv2/core.hpp"
#include "constants.hpp"
#include "grayscale_bitmap.hpp"


namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{
			class PCTSampler : public Algorithm
			{
			public:
				
				static Ptr<PCTSampler> create(
					const std::vector<cv::Point2f>	&initPoints,
					int						sampleCount = 500,
					int						grayscaleBits = 4,
					int						windowRadius = 5);

				
				virtual void sample(const cv::InputArray image, cv::OutputArray samples) const = 0;

				
				/**** accessors ****/

				virtual int getSampleCount() const = 0;
				virtual int getGrayscaleBits() const = 0;
				virtual int getWindowRadius() const = 0;
				virtual double getWeightX() const = 0;
				virtual double getWeightY() const = 0;
				virtual double getWeightL() const = 0;
				virtual double getWeightA() const = 0;
				virtual double getWeightB() const = 0;
				virtual double getWeightConstrast() const = 0;
				virtual double getWeightEntropy() const = 0;

				virtual void setSampleCount(int sampleCount) = 0;
				virtual void setGrayscaleBits(int grayscaleBits) = 0;
				virtual void setWindowRadius(int radius) = 0;
				virtual void setWeightX(double weight) = 0;
				virtual void setWeightY(double weight) = 0;
				virtual void setWeightL(double weight) = 0;
				virtual void setWeightA(double weight) = 0;
				virtual void setWeightB(double weight) = 0;
				virtual void setWeightContrast(double weight) = 0;
				virtual void setWeightEntropy(double weight) = 0;

				virtual void setWeight(int idx, double value) = 0;
				virtual void setWeights(const std::vector<double> &weights) = 0;
				virtual void setTranslation(int idx, double value) = 0;
				virtual void setTranslations(const std::vector<double> &translations) = 0;


			};

		}
	}
}


#endif
