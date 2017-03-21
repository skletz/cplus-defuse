#include "minkowski1.hpp"
#include "gd1parameter.hpp"

defuse::Minkowski::Minkowski(Parameter* _parameter)
{
	mDistance = dynamic_cast<GDParamter*>(_parameter)->distance;
}

float defuse::Minkowski::compute(Features& _f1, Features& _f2)
{
	cv::Mat f1 = _f1.mVectors;
	cv::Mat f2 = _f2.mVectors;

	//float result = cv::norm(f1 - f2, cv::NORM_L2);

	//return result;
	return 0.0f;
}
