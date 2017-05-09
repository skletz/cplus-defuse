#ifndef _DEFUSE_SMD_HPP_
#define _DEFUSE_SMD_HPP_

#include "minkowski.hpp"
#include <opencv2/core/mat.hpp>

/**
 * \brief Signature Matching Distance (SMD)
 * 
 * @author skletz
 * @version 2.0 25/04/17
 */
namespace defuse {

	class SMD
	{

		Minkowski* mGDDistance;
		int mDirection;
		float mLambda = 1.0;

	public:

		/**
		 * \brief Signature Matching Distance
		 * \param _GDDistance ground distance using Minkowski distance
		 * \param _direction varient of SMD (0 = bidirectional, 1 = asymmetric query, 2 = asymmetric db)
		 * \param _lambda influence of bidirectional matches
		 */
		SMD(Minkowski* _GDDistance, int _direction, float _lambda);

		/**
		 * \brief  Calculates the distance between two features
		 * \param _f1 features
		 * \param _f2 features
		 * \return Distance between _f1 and _f2
		 */
		float compute(cv::Mat& _f1, cv::Mat& _f2) const;

		/**
		 * \brief Calculates the distance between two features at a specific row
		 * \param _f1 features
		 * \param _idx1 position at row
		 * \param _f2 features
		 * \param _idx2 position at row
		 * \return Distance between _f1 and _f2
		 */
		float compute(cv::Mat& _f1, int _idx1, cv::Mat& _f2, int _idx2) const;

		/**
		 * \brief Calculates asymmetric Query variant of Signature Matching Distance
		 * \param _f1 features
		 * \param _f2 features
		 * \return Distance between _f1 and _f2
		 */
		float asymmetricQuery(cv::Mat& _f1, cv::Mat& _f2) const;

		/**
		 * \brief Calculates asymmetric DB variant of Signature Matching Distance
		 * \param _f1 features
		 * \param _f2 features
		 * \return Distance between _f1 and _f2
		 */
		float assymetricDB(cv::Mat& _f1, cv::Mat& _f2) const;

		/**
		 * \brief Calculates bidirectional variant of Signature Matching Distance (original)
		 * \param _f1 features
		 * \param _f2 features
		 * \return Distance between _f1 and _f2
		 */
		float bidirectional(cv::Mat& _f1, cv::Mat& _f2) const;

		/**
		 * \brief Finds Nearest Neighbor (Matching Strategy)
		 * \param _f1 features
		 * \param idx1 at row in _f1
		 * \param _f2 features
		 * \return Nearest Neighbor from _f1 in _f2
		 */
		int nearestNeighbor(cv::Mat& _f1, int idx1, cv::Mat& _f2) const;

		/**
		 * \brief Weights costs
		 * \param _f1 features
		 * \param idx1 at row in _f1
		 * \param distance 
		 * \return Weighted distance
		 */
		float weightedDistance(cv::Mat& _f1, int idx1, float distance) const;

		/**
		 * \brief Prints settings
		 * \return Settings of SMD
		 */
		std::string toString() const;
	};
}
#endif