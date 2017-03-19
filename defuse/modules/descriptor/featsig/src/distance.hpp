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
#ifndef PCT_SIGNATURES_DISTANCE_HPP
#define PCT_SIGNATURES_DISTANCE_HPP

#include "opencv2/core.hpp"
#include "constants.hpp"

namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{

			/**
			* \brief Metric distance functor.
			*/
			class Distance
			{
			public:
				/**
				* \brief Compute a distance between two points in two lists.
				* \param points1 The first set of point.
				* \param idx1 Index into the first set of points.
				* \param points2 The second set of point.
				* \param idx2 Index into the second set of points.
				* \return Distance between two selected points (in R^cv::xfeatures2d::pct_signatures::SIGNATURE_DIMENSION).
				*/
				virtual float operator()(const Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const = 0;
			};


			class DistanceL0_25 : public Distance
			{
			public:
				virtual float operator()(const cv::Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const
				{
					float result = (float)0.0;
					for (int d = 0; d < SIGNATURE_DIMENSION - 1; ++d)
					{
						float difference = points1.at<float>(idx1, d) - points2.at<float>(idx2, d);
						result += std::sqrt(std::sqrt(std::abs(difference)));
					}
					result *= result;
					return result * result;
				}
			};


			class DistanceL0_5 : public Distance
			{
			public:
				virtual float operator()(const cv::Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const
				{
					float result = (float)0.0;
					for (int d = 0; d < SIGNATURE_DIMENSION - 1; ++d)
					{
						float difference = points1.at<float>(idx1, d) - points2.at<float>(idx2, d);
						result += std::sqrt(std::abs(difference));
					}
					return result * result;
				}
			};


			class DistanceL1 : public Distance
			{
			public:
				virtual float operator()(const cv::Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const
				{
					float result = (float)0.0;
					for (int d = 0; d < SIGNATURE_DIMENSION - 1; ++d)
					{
						float difference = points1.at<float>(idx1, d) - points2.at<float>(idx2, d);
						result += std::abs(difference);
					}
					return result;
				}
			};


			class DistanceL2 : public Distance
			{
			public:
				virtual float operator()(const cv::Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const
				{
					float result = (float)0.0;
					for (int d = 0; d < SIGNATURE_DIMENSION - 1; ++d)
					{
						float difference = points1.at<float>(idx1, d) - points2.at<float>(idx2, d);
						result += difference * difference;
					}
					return std::sqrt(result);
				}
			};


			class DistanceL5 : public Distance
			{
			public:
				virtual float operator()(const cv::Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const
				{
					float result = (float)0.0;
					for (int d = 0; d < SIGNATURE_DIMENSION - 1; ++d)
					{
						float difference = points1.at<float>(idx1, d) - points2.at<float>(idx2, d);
						result += std::abs(difference) * difference * difference * difference * difference;
					}
					return std::pow(result, (float)0.2);
				}
			};


			class DistanceLp : public Distance
			{
			private:
				float mP;

			public:
				DistanceLp(float p) : mP(p) {}

				virtual float operator()(const cv::Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const
				{
					float result = (float)0.0;
					for (int d = 0; d < SIGNATURE_DIMENSION - 1; ++d)
					{
						float difference = points1.at<float>(idx1, d) - points2.at<float>(idx2, d);
						result += std::pow(std::abs(difference), mP);
					}
					return std::pow(result, (float)1.0 / mP);
				}
			};


			/**
			* \brief Create instance of distance functor based on given L_p parameter.
			*/
			static Distance* createDistance(float Lp)
			{
				if (Lp == (float)0.25)
					return new DistanceL0_25();
				else if (Lp == (float)0.5)
					return new DistanceL0_5();
				else if (Lp == (float)1.0)
					return new DistanceL1();
				else if (Lp == (float)2.0)
					return new DistanceL2();
				else if (Lp == (float)5.0)
					return new DistanceL5();
				else
					return new DistanceLp(Lp);
			}
		}
	}
}




#endif