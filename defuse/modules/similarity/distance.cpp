#include "distance.hpp"

float defuse::Distance::computeL2(Features& _f1, int _idx1, Features& _f2, int _idx2)
{
	cv::Mat f1 = _f1.mVectors;
	cv::Mat f2 = _f2.mVectors;

	double result = 0.0;

	for (int i = 0; i < f1.cols; i++)
	{
		double distance = f1.at<float>(_idx1, i) - f2.at<float>(_idx2, i);
		result += distance * distance;
	}

	result = std::sqrt(result);
	return result;
}

float defuse::Distance::computeL2(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim)
{
	cv::Mat f1 = _f1.mVectors;
	cv::Mat f2 = _f2.mVectors;

	double result = 0.0;

	for (int i = 0; i < f1.cols; i++)
	{
		if (i == skipDim)
		{
			continue;
		}

		double distance = f1.at<float>(_idx1, i) - f2.at<float>(_idx2, i);
		result += distance * distance;
	}

	result = std::sqrt(result);
	return result;
}
