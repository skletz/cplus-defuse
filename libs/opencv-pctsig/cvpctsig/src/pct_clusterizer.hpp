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
#ifndef PCT_SIGNATURES_CLUSTERIZER_HPP
#define PCT_SIGNATURES_CLUSTERIZER_HPP

#include "opencv2/core.hpp"
#include "constants.hpp"
#include "distance.hpp"


namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{
			class PCTClusterizer : public Algorithm
			{
			public:
				
				static Ptr<PCTClusterizer> create(
					int initSeeds = 50,
					int iterations = 5,
					int maxClusters = 256,
					int clusterMinSize = 2,
					float joiningDistance = 0.01,
					float dropThreshold = 0,
					float Lp = 2.0f);


				/**** Accessors ****/

				virtual int	getIterationCount() const = 0;
				virtual int	getInitSeedCount() const = 0;
				virtual int	getMaxClustersCount() const = 0;
				virtual int	getClusterMinSize() const = 0;
				virtual float getJoiningDistance() const = 0;
				virtual float getDropThreshold() const = 0;
				virtual float getLpNorm() const = 0;


				virtual void setIterationCount(int iterationCount) = 0;
				virtual void setInitSeedCount(int initSeedCount) = 0;
				virtual void setMaxClustersCount(int maxClustersCount) = 0;
				virtual void setClusterMinSize(int clusterMinSize) = 0;
				virtual void setJoiningDistance(float joiningDistance) = 0;
				virtual void setDropThreshold(float dropThreshold) = 0;
				virtual void setLpNorm(float LpNorm) = 0;

				virtual void clusterize(const cv::InputArray samples, cv::OutputArray signature) = 0;
			};
		}
	}
}

#endif