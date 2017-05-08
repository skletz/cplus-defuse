#include "mahalanobis1.hpp"
#include "mahalanobis1parameter.hpp"
#include <opencv2/core.hpp>

float defuse::Mahalanobis::compute(Features& _f1, Features& _f2)
{
	float result = 0;
	cv::Mat f1 = _f1.mVectors;
	cv::Mat f2 = _f2.mVectors;

	cv::Mat p1 = f1(cv::Range(0, 1), cv::Range::all());
	cv::Mat p2 = f1(cv::Range(1, 2), cv::Range::all());


	cv::Mat mean;
	mean.create(f1.rows, f1.cols, f1.type());
	mean += f1;
	mean += f2;
	mean /= 2;

	cv::Mat covarMatrix;
	cv::calcCovarMatrix(f1, covarMatrix, mean, cv::COVAR_NORMAL | cv::COVAR_ROWS, CV_32F);
	cv::Mat icovar = covarMatrix.inv(cv::DECOMP_SVD);


	result = cv::Mahalanobis(f1, f2, icovar);

	return result;
}
