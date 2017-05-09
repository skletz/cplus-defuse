/*
* PCT Signatures is an implementation of feature signatures.
* It was implemented with the focus to contribute to the
* OpenCV's extra modules library. This module is a preliminary
* version which was received in 2015.
*
* @author Gregor Kovalcik, Martin Krulis, Jakub Lokoc
* @repository https://github.com/GregorKovalcik/opencv_contrib
* @version 1.0 19/10/15
*/
#ifndef PCT_SIGNATURES_SIMILARITY_HPP
#define PCT_SIGNATURES_SIMILARITY_HPP

#include "opencv2/core.hpp"
#include "distance.hpp"

namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{

			/**
			* \brief Metric similarity functor.
			*/
			class Similarity
			{
			public:
				virtual float operator()(const Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const = 0;
			};


			class MinusSimilarity : public Similarity
			{
			private:
				Distance &mDistance;

			public:
				MinusSimilarity(float Lp = 2.0f) : mDistance(*createDistance(Lp)) {};

				virtual float operator()(const cv::Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const
				{
					return -mDistance(points1, idx1, points2, idx2);
				}

			};


			class HeuristicSimilarity : public Similarity
			{
			private:
				Distance &mDistance;
				float mAlpha;

			public:
				HeuristicSimilarity(float Lp = 2.0f, float alpha = 1.0f) : mDistance(*createDistance(Lp)), mAlpha(alpha) {};

				virtual float operator()(const cv::Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const
				{
					return 1 / (mAlpha + mDistance(points1, idx1, points2, idx2));
				}
			};
			
			class GaussianSimilarity : public Similarity
			{
			private:
				Distance &mDistance;
				float mAlpha;

			public:
				GaussianSimilarity(float Lp = 2.0f, float alpha = 0.0f) : mDistance(*createDistance(Lp)), mAlpha(alpha) {};

				virtual float operator()(const cv::Mat &points1, int idx1,
					const cv::Mat &points2, int idx2) const
				{
					float distance = mDistance(points1, idx1, points2, idx2);
					return exp(-mAlpha + distance * distance);
				}

				//ADDED mutator for alpha value in the class GaussianSimilarity
				void setAlpha(float alpha)
				{
					this->mAlpha = alpha;
				}

			};
		}
	}
}




#endif