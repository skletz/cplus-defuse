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
					std::size_t initSeeds = 50,
					std::size_t iterations = 5,
					std::size_t maxClusters = 256,
					std::size_t clusterMinSize = 2,
					float joiningDistance = 0.01,
					float dropThreshold = 0,
					float Lp = 2.0f);


				/**** Accessors ****/

				virtual std::size_t	getIterationCount() const = 0;
				virtual std::size_t	getInitSeedCount() const = 0;
				virtual std::size_t	getMaxClustersCount() const = 0;
				virtual std::size_t	getClusterMinSize() const = 0;
				virtual float getJoiningDistance() const = 0;
				virtual float getDropThreshold() const = 0;
				virtual float getLpNorm() const = 0;


				virtual void setIterationCount(std::size_t iterationCount) = 0;
				virtual void setInitSeedCount(std::size_t initSeedCount) = 0;
				virtual void setMaxClustersCount(std::size_t maxClustersCount) = 0;
				virtual void setClusterMinSize(std::size_t clusterMinSize) = 0;
				virtual void setJoiningDistance(float joiningDistance) = 0;
				virtual void setDropThreshold(float dropThreshold) = 0;
				virtual void setLpNorm(float LpNorm) = 0;

				virtual void clusterize(const cv::InputArray samples, cv::OutputArray signature) = 0;
			};
		}
	}
}

#endif