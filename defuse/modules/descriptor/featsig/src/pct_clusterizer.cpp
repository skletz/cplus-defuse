#include "pct_clusterizer.hpp"

namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{
			class PCTClusterizer_Impl : public PCTClusterizer
			{
			public:

				PCTClusterizer_Impl(
					std::size_t initSeedCount,
					std::size_t iterationCount,
					std::size_t maxClustersCount,
					std::size_t clusterMinSize,
					float joiningDistance,
					float dropThreshold,
					float LpNorm)
					: mInitSeedCount(initSeedCount),
					mIterationCount(iterationCount),
					mMaxClustersCount(maxClustersCount),
					mClusterMinSize(clusterMinSize),
					mJoiningDistance(joiningDistance),
					mDropThreshold(dropThreshold),
					mLpNorm(LpNorm),
					mDistance(*createDistance(LpNorm))
				{

				}

				~PCTClusterizer_Impl()
				{
					delete(&mDistance);
				}


				std::size_t	getIterationCount() const				{ return mIterationCount; }
				std::size_t	getInitSeedCount() const				{ return mInitSeedCount; }
				std::size_t	getMaxClustersCount() const				{ return mMaxClustersCount; }
				std::size_t	getClusterMinSize() const				{ return mClusterMinSize; }
				float getJoiningDistance() const					{ return mJoiningDistance; }
				float getDropThreshold() const						{ return mDropThreshold; }
				float getLpNorm() const								{ return mLpNorm; }


				void setIterationCount(std::size_t iterationCount)			{ mIterationCount = iterationCount; }
				void setInitSeedCount(std::size_t initSeedCount)			{ mInitSeedCount = initSeedCount; }
				void setMaxClustersCount(std::size_t maxClustersCount)		{ mMaxClustersCount = maxClustersCount; }
				void setClusterMinSize(std::size_t clusterMinSize)			{ mClusterMinSize = clusterMinSize; }
				void setJoiningDistance(float joiningDistance)				{ mJoiningDistance = joiningDistance; }
				void setDropThreshold(float dropThreshold)					{ mDropThreshold = dropThreshold; }
				void setLpNorm(float LpNorm)								
				{ 
					mLpNorm = LpNorm; 
					delete(&mDistance);
					mDistance = *createDistance(LpNorm);
				}


				/**
				* \brief Join clusters that are closer than joining distance.
				*		If two clusters are joined one of them gets its weight set to 0.
				* \param clusters List of clusters to be scaned and joined.
				*/
				void joinCloseClusters(cv::Mat clusters)
				{
					for (int i = 0; i < clusters.rows - 1; i++)
					{
						if (clusters.at<float>(i, WEIGHT_IDX) == 0)
						{
							continue;
						}

						for (int j = i + 1; j < clusters.rows; j++)
						{
							if (clusters.at<float>(j, WEIGHT_IDX) > 0 && mDistance(clusters, i, clusters, j) <= mJoiningDistance)
							{
								clusters.at<float>(i, WEIGHT_IDX) = 0;
								break;
							}
						}
					}
				}


				/**
				* \brief Remove points whose weight is lesser or equal to given threshold.
				*		The point list is compacted and relative order of points is maintained.
				* \param dropThreshold Largest weight of the points being dropped.
				*/
				void dropLightPoints(cv::Mat& clusters)
				{
					std::size_t frontIdx = 0;

					// Skip leading continuous part of weighted-enough points.
					while (frontIdx < clusters.rows && clusters.at<float>(frontIdx, WEIGHT_IDX) > mDropThreshold)
					{
						++frontIdx;
					}

					// Mark first emptied position and advance front index.
					std::size_t tailIdx = frontIdx++;

					while (frontIdx < clusters.rows)
					{
						if (clusters.at<float>(frontIdx, WEIGHT_IDX) > mDropThreshold)
						{
							// Current (front) item is not dropped -> copy it to the tail.
							clusters.row(frontIdx).copyTo(clusters.row(tailIdx));
							++tailIdx;	// grow the tail
						}
						++frontIdx;
					}
					clusters.resize(tailIdx);
				}


				/**
				* \brief Find closest cluster to selected point.
				* \param clusters List of cluster centroids.
				* \param points List of points.
				* \param pointIdx Index to the list of points (the point for which the closest cluster is being found).
				* \return Index to clusters list pointing at the closest cluster.
				*/
				std::size_t findClosestCluster(Mat &clusters, Mat &points, std::size_t pointIdx) const
				{
					std::size_t iClosest = 0;
					float minDistance = mDistance(clusters, 0, points, pointIdx);
					for (std::size_t iCluster = 1; iCluster < clusters.rows; iCluster++)
					{
						float distance = mDistance(clusters, iCluster, points, pointIdx);
						if (distance < minDistance)
						{
							iClosest = iCluster;
							minDistance = distance;
						}
					}
					return iClosest;
				}


				/**
				* \brief Make sure that the number of clusters does not exceed maxClusters parameter.
				*		If it does, the clusters are sorted by their weights and the smallest clusters
				*		are cropped.
				* \param clusters List of clusters being scanned and cropped.
				* \note This method does nothing iff the number of clusters is within the range.
				*/
				void cropClusters(Mat &clusters) const
				{
					if (clusters.rows > mMaxClustersCount)
					{
						Mat duplicate;						// save original clusters
						clusters.copyTo(duplicate);

						Mat sortedIdx;						// sort using weight column
						sortIdx(clusters(Rect(SIGNATURE_DIMENSION - 1, 0, 1, clusters.rows)), sortedIdx, SORT_EVERY_COLUMN + SORT_DESCENDING);

						clusters.resize(mMaxClustersCount);		// crop to max clusters
						for (int i = 0; i < mMaxClustersCount; i++)
						{									// copy sorted rows
							duplicate.row(sortedIdx.at<int>(i, 0)).copyTo(clusters.row(i));
						}
					}
				}


				/**
				* \brief Fallback procedure invoked when the clustering eradicates all clusters.
				*		Single cluster consisting of all points is then created.
				*/
				void singleClusterFallback(const Mat &points, Mat &clusters) const
				{
					if (clusters.rows != 0)
					{
						return;
					}

					// Initialize clusters.
					clusters.resize(1);
					clusters.at<float>(0, WEIGHT_IDX) = static_cast<float>(points.rows);

					// Sum all points.
					for (std::size_t i = 0; i < points.rows; ++i)
					{
						for (std::size_t d = 0; d < SIGNATURE_DIMENSION - 1; d++)
						{
							clusters.at<float>(0, d) += points.at<float>(i, d);
						}
					}
					
					// Divide centroid by number of points -> compute average in each dimension.
					for (std::size_t i = 0; i < points.rows; ++i)
					{
						for (std::size_t d = 0; d < SIGNATURE_DIMENSION - 1; d++)
						{
							clusters.at<float>(0, d) = clusters.at<float>(0, d) / clusters.at<float>(0, WEIGHT_IDX);
						}
					}
				}

				void normalizeWeights(Mat &clusters)
				{
					// get max weight
					float maxWeight = clusters.at<float>(0, WEIGHT_IDX);
					for (int i = 1; i < clusters.rows; i++)
					{
						if (clusters.at<float>(i, WEIGHT_IDX) > maxWeight)
						{
							maxWeight = clusters.at<float>(i, WEIGHT_IDX);
						}
					}

					// normalize weight
					float weightNormalizer = 1 / maxWeight;
					for (int i = 0; i < clusters.rows; i++)
					{
						clusters.at<float>(i, WEIGHT_IDX) = clusters.at<float>(i, WEIGHT_IDX) * weightNormalizer;
					}
				}

				void clusterize(const cv::InputArray _samples, cv::OutputArray _signature)
				{
					CV_Assert(!_samples.empty());

					// prepare matrices
					cv::Mat samples = _samples.getMat();

					if (this->mInitSeedCount >= samples.rows)
					{
						CV_Error_(CV_StsBadArg, ("Number of seeds %d must be less than number of samples %d.", mInitSeedCount, samples.rows));
					}
					
					// Prepare initial centroids.
					Mat clusters;
					samples(Rect(0, 0, samples.cols, mInitSeedCount)).copyTo(clusters);		// make seeds from the first mInitSeeds samples
					clusters(Rect(WEIGHT_IDX, 0, 1, clusters.rows)) = 1;					// set initial weight to 1


					// prepare for iterating
					joinCloseClusters(clusters);
					dropLightPoints(clusters);


					// Main iterations cycle. Our implementation has fixed number of iterations.
					for (std::size_t iteration = 0; iteration < this->mIterationCount; iteration++)
					{
						// Prepare space for new centroid values.
						Mat tmpCentroids(clusters.size(), clusters.type());
						tmpCentroids = 0;

						// Clear weights for new iteration.
						clusters(Rect(WEIGHT_IDX, 0, 1, clusters.rows)) = 0;
						
						// Compute affiliation of points and sum new coordinates for centroids.
						for (std::size_t iSample = 0; iSample < samples.rows; iSample++)
						{
							std::size_t iClosest = findClosestCluster(clusters, samples, iSample);
							for (std::size_t iDimension = 0; iDimension < SIGNATURE_DIMENSION - 1; iDimension++)	
							{
								tmpCentroids.at<float>(iClosest, iDimension) += samples.at<float>(iSample, iDimension);
							}
							clusters.at<float>(iClosest, WEIGHT_IDX)++;
						}

						// Compute average from tmp coordinates and throw away too small clusters.
						std::size_t lastIdx = 0;
						for (std::size_t i = 0; (int)i < tmpCentroids.rows; ++i)
						{
							// Skip clusters that are too small (large-enough clusters are packed right away)
							if (clusters.at<float>(i, WEIGHT_IDX) > (iteration + 1) * this->mClusterMinSize)
							{
								for (std::size_t d = 0; d < SIGNATURE_DIMENSION - 1; d++)
								{
									clusters.at<float>(lastIdx, d) = tmpCentroids.at<float>(i, d) / clusters.at<float>(i, WEIGHT_IDX);
								}
								// weights must be compacted as well
								clusters.at<float>(lastIdx, WEIGHT_IDX) = clusters.at<float>(i, WEIGHT_IDX);
								lastIdx++;
							}
						}

						// Crop the arrays if some centroids were dropped.
						clusters.resize(lastIdx);
						if (clusters.rows == 0)
						{
							break;
						}

						// Finally join clusters with too close centroids.
						joinCloseClusters(clusters);
						dropLightPoints(clusters);
					}

					// The result must not be empty!
					if (clusters.rows == 0)
					{
						singleClusterFallback(samples, clusters);
					}

					cropClusters(clusters);

					normalizeWeights(clusters);

					// save the result
					_signature.create(clusters.rows, SIGNATURE_DIMENSION, clusters.type());
					Mat signature = _signature.getMat();
					clusters.copyTo(signature);
				}


			protected:
				/**
				* \brief Number of iterations performed (the number is fixed).
				*/
				std::size_t mIterationCount;

				/**
				* \brief Number of intitial seeds used for clustering.
				*/
				std::size_t mInitSeedCount;

				/**
				* \brief Maximal number of clusters. If the number of clusters is exceeded after
				*		all iterations are completed, the smallest clusters are thrown away.
				*/
				std::size_t mMaxClustersCount;

				/**
				* \brief Minimal size of the cluster. After each (i-th) iteration, the cluster sizes are checked.
				*		Cluster smaller than i*mClusterMinSize are disposed of.
				*/
				std::size_t mClusterMinSize;

				/**
				* \brief Distance threshold between two centroids. If two centroids become closer
				*		than this distance, the clusters are joined.
				*/
				float mJoiningDistance;

				/**
				* \brief Weight threshold of centroids. If weight of a centroid is below this value at the end of the iteration, it is thrown away.
				*/
				float mDropThreshold;
				
				/**
				* \brief L_p metric is used for computing distances. This value defines the p parameter.
				*		That means the 2.0 value ~ Euclidian metrics, 1.0 ~ Manhattan metric, etc.
				* \note The values MIGHT be restricted to following: 0.125, 0.25, 0.5, 1.0, 2.0, and 5.0.
				*/
				float mLpNorm;

				Distance &mDistance;
			};


			Ptr<PCTClusterizer> PCTClusterizer::create(
				std::size_t initSeedCount,
				std::size_t iterationCount,
				std::size_t maxClustersCount,
				std::size_t clusterMinSize,
				float joiningDistance,
				float dropThreshold,
				float Lp)
			{
				return makePtr<PCTClusterizer_Impl>(initSeedCount, iterationCount, maxClustersCount, clusterMinSize, joiningDistance, dropThreshold, Lp);
			}
		}
	}
}