#ifndef _DEFUSE_MIKOWSKI_HPP_
#define _DEFUSE_MIKOWSKI_HPP_

#include <opencv2/core/mat.hpp>

/**
 * \brief Minkowski Distance
 * 
 * @author skletz
 * @version 2.0 25/04/17
 */
namespace defuse {

	class Minkowski
	{
	public:
		int mDistance;

		/**
		 * \brief Minkowski Distance
		 * \param _distance Type of Minkowski (1 = L1, 2 = L2)
		 */
		Minkowski(int _distance);

		/**
		 * \brief  Calculates the distance between two features 
		 * \param _f1 features
		 * \param _f2 features
		 * \return Distance between _f1 and _f2
		 */
		float compute(cv::Mat& _f1, cv::Mat& _f2) const;
		/**
		 * \brief  Calculates the distance between two features at a specific row
		 * \param _f1 features
		 * \param _idx1 position at row
		 * \param _f2 features
		 * \param _idx2 position at row
		 * \return Distance between _f1 and _f2
		 */
		float compute(cv::Mat& _f1, int _idx1, cv::Mat& _f2, int _idx2) const;

		/**
		 * \brief  Calculates the distance between two features 
		 * \param _f1 features
		 * \param _f2 features
		 * \return Distance between _f1 and _f2
		 */
		float computeL1(cv::Mat& _f1, cv::Mat& _f2) const;
		
		/**
		 * \brief  Calculates the distance between two features
		 * \param _f1 features
		 * \param _f2 features
		 * \return Distance between _f1 and _f2
		 */
		float computeL2(cv::Mat& _f1, cv::Mat& _f2) const;

		/**
		 * \brief  Calculates the distance between two features at a specific row 
		 * \param _f1 features
		 * \param _idx1 position at row
		 * \param _f2 features
		 * \param _idx2 position at row
		 * \return Distance between _f1 and _f2
		 */
		float computeL1(cv::Mat& _f1, int _idx1, cv::Mat& _f2, int _idx2) const;
		
		/**
		 * \brief  Calculates the distance between two features at a specific row 
		 * \param _f1 features
		 * \param _idx1 position at row
		 * \param _f2 features
		 * \param _idx2 position at row
		 * \return Distance between _f1 and _f2
		 */
		float computeL2(cv::Mat& _f1, int _idx1, cv::Mat& _f2, int _idx2) const;

		/**
		* \brief Prints settings
		* \return Settings of Minkowski distance
		*/
		std::string toString() const;
	};
}

#endif