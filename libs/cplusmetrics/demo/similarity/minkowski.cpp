#include "minkowski.hpp"
#include <opencv2/core.hpp>
#include <sstream>

defuse::Minkowski::Minkowski(int _distance)
{
	mDistance = _distance;

}

float defuse::Minkowski::compute(cv::Mat& _f1, cv::Mat& _f2) const
{
	if (mDistance == 1)
	{
		return computeL1(_f1, _f2);
	}

	return computeL2(_f1, _f2);
}

float defuse::Minkowski::compute(cv::Mat& _f1, int _idx1, cv::Mat& _f2, int _idx2) const
{
	if (mDistance == 1)
	{
		return computeL1(_f1, _idx1, _f2, _idx2);
	}

	return computeL2(_f1, _idx1, _f2, _idx2);
}

float defuse::Minkowski::computeL1(cv::Mat& _f1, cv::Mat& _f2) const
{
	float result = float(cv::norm(_f1 - _f2, cv::NORM_L1));
	return result;
}

float defuse::Minkowski::computeL2(cv::Mat& _f1, cv::Mat& _f2) const
{
	float result = float(cv::norm(_f1 - _f2, cv::NORM_L2));
	return result;
}

float defuse::Minkowski::computeL1(cv::Mat& _f1, int _idx1, cv::Mat& _f2, int _idx2) const
{
	float result = 0.0;

	for (int i = 0; i < _f1.cols; i++)
	{
		float distance = _f1.at<float>(_idx1, i) - _f2.at<float>(_idx2, i);
		result += float(std::abs(distance));
	}

	return result;
}

float defuse::Minkowski::computeL2(cv::Mat& _f1, int _idx1, cv::Mat& _f2, int _idx2) const
{
	float result = 0.0;

	for (int i = 0; i < _f1.cols; i++)
	{
		float distance = _f1.at<float>(_idx1, i) - _f2.at<float>(_idx2, i);
		result += distance * distance;
	}

	result = std::sqrt(result);

	return result;
}

std::string defuse::Minkowski::toString() const
{
	std::stringstream st;
	st << "Ground distance: L" << mDistance;
	return st.str();
}

