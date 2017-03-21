#include "minkowski1.hpp"
#include "minkowski1parameter.hpp"

#include <opencv2/core.hpp>

defuse::Minkowski::Minkowski(Parameter* _parameter)
{
	mDistance = dynamic_cast<MinkowskiParamter*>(_parameter)->distance;
}

float defuse::Minkowski::compute(Features& _f1, Features& _f2)
{
	cv::Mat f1 = _f1.mVectors;
	cv::Mat f2 = _f2.mVectors;

	float result = cv::norm(f1 - f2, cv::NORM_L2);

	return result;
}

float defuse::Minkowski::computeL1(Features& _f1, int _idx1, Features& _f2, int _idx2) const
{
	cv::Mat f1 = _f1.mVectors;
	cv::Mat f2 = _f2.mVectors;

	double result = 0.0;

	for (int i = 0; i < f1.cols; i++)
	{
		double distance = f1.at<float>(_idx1, i) - f2.at<float>(_idx2, i);
		result += std::abs(distance);
	}

	return result;
}

float defuse::Minkowski::computeL1(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim) const
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
		result += std::abs(distance);
	}

	result = std::sqrt(result);
	return result;
}

float defuse::Minkowski::computeL2(Features& _f1, int _idx1, Features& _f2, int _idx2) const
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

float defuse::Minkowski::computeL2(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim) const
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
