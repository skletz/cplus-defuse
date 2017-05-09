#include "tpct_signatures.hpp"
#include <opencv2/core.hpp>
#include "constants.h"
#include <cpluslogger.hpp>

using namespace analysis::tpct_signatures;
TPCTSignatures::TPCTSignatures()
{
}


TPCTSignatures::~TPCTSignatures()
{
}

void TPCTSignatures::computeTemporalSignature(std::vector<cv::Mat>& staticsignatures, cv::OutputArray& _temporalsignature) const
{
	//std::vector<cv::Mat> staticsignatures;
	//_staticsignatures.getMatVector(staticsignatures);

	if(staticsignatures.size() == 0) {
		LOG_INFO("Static signatures are zero - cannot track motion");
	}

	for (int i = 0; i < staticsignatures.size(); i++)
	{
		enlarge(staticsignatures[i], staticsignatures[i]);
	}

	for (int iSignature = staticsignatures.size() - 1; iSignature > 0; iSignature--)
	{
		NNbruteforcematching(staticsignatures[iSignature], staticsignatures[iSignature - 1]);
	}

	cv::Mat tmp;
	staticsignatures.front().copyTo(tmp);

	for (int i = 0; i < tmp.rows; i++)
	{
		float x = tmp.at<float>(i, IDX::X);
		float y = tmp.at<float>(i, IDX::Y);

		float dx = tmp.at<float>(i, IDX::DX);
		float dy = tmp.at<float>(i, IDX::DY);

		float x2 = dx + x;
		float y2 = dy + y;

		tmp.at<float>(i, IDX::DX) = x2;
		tmp.at<float>(i, IDX::DY) = y2;
	}

	tmp.copyTo(_temporalsignature);
}

void TPCTSignatures::enlarge(cv::InputArray _staticsignature, cv::OutputArray _temporalsignature) const
{

	cv::Mat staticsignature;
	_staticsignature.copyTo(staticsignature);

	cv::Mat temporalsignature;
	int dims = 2;

	cv::Size newsize(dims, staticsignature.rows); //add additional dimensions

	temporalsignature = cv::Mat::zeros(newsize, staticsignature.type()); // init by zero => no movement
	cv::hconcat(staticsignature, temporalsignature, temporalsignature); //enlarge fs2
	temporalsignature.copyTo(_temporalsignature);
}

float TPCTSignatures::computeQuadraticFormDistance(const cv::InputArray _signature0, const cv::InputArray _signature1, const cv::xfeatures2d::pct_signatures::Similarity &similarity)
{
	// check input
	if (_signature0.empty() || _signature1.empty())
	{
		CV_Error(CV_StsBadArg, "Empty signature!");
	}

	cv::Mat signature0 = _signature0.getMat();
	cv::Mat signature1 = _signature1.getMat();

	if (signature0.cols != tf_signatures::SIGNATURE_DIMENSION || signature1.cols != tf_signatures::SIGNATURE_DIMENSION)
	{
		CV_Error_(CV_StsBadArg, ("Signature dimension must be %d!", tf_signatures::SIGNATURE_DIMENSION));
	}

	if (signature0.rows <= 0 || signature1.rows <= 0)
	{
		CV_Error(CV_StsBadArg, "Signature count must be greater than 0!");
	}

	// compute sqfd
	float result = 0;
	result += computePartialSQFD(signature0, signature0, similarity);
	result += computePartialSQFD(signature1, signature1, similarity);
	result -= computePartialSQFD(signature0, signature1, similarity) * 2;

	return sqrt(result);
}

float TPCTSignatures::computePartialSQFD(const cv::Mat &signature0, const cv::Mat &signature1, const cv::xfeatures2d::pct_signatures::Similarity& similarity)
{
	float result = 0;
	for (int i = 0; i < signature0.rows; i++)
	{
		for (int j = 0; j < signature1.rows; j++)
		{
			result += signature0.at<float>(i, cv::xfeatures2d::pct_signatures::WEIGHT_IDX) * signature1.at<float>(j, cv::xfeatures2d::pct_signatures::WEIGHT_IDX) * similarity(signature0, i, signature1, j);
		}
	}
	return result;
}

void TPCTSignatures::NNbruteforcematching(cv::InputArray _representativesX, cv::InputOutputArray _representativesY) const
{
	cv::Mat Rx, Ry;
	_representativesX.copyTo(Rx);
	_representativesY.copyTo(Ry);

	if (Rx.empty() || Ry.empty())
	{
		CV_Error(CV_StsBadArg, "Empty signature!");
	}

	if (Rx.cols != DIMENSION || Ry.cols != DIMENSION)
	{
		CV_Error_(CV_StsBadArg, ("Invalid signature format. Signature.cols must be %d.", DIMENSION));
	}

	cv::Mat iVectorX, iVectorY;
	double dist = 0;
	int pos = 0;
	for (int iX = 0; iX < Rx.rows; iX++)
	{
		double minimalDist = std::numeric_limits<double>::max();

		cv::Mat iClosest_Y_to_X;
		Rx.row(iX).copyTo(iVectorX);

		for (int iY = 0; iY < Ry.rows; iY++)
		{
			Ry.row(iY).copyTo(iVectorY);
			//
			dist = groundDistance(iVectorX, iVectorY);

			//right uniqueness
			if ((dist < minimalDist) && ((iVectorY.at<float>(0, IDX::DX) + iVectorY.at<float>(0, IDX::DY)) == 0))
			{
				pos = iY;
				minimalDist = dist;

				iVectorY.copyTo(iClosest_Y_to_X);
			}
		}

		//TODO ASSERT any other cases why we could not found a nearest vector?
		//TODO if RX.cols > RY.cols then we do not found for all clusters in RX a nearest cluster in RY - then ??
		if (iClosest_Y_to_X.empty())
		{
			//break; //left totality
			iVectorX.copyTo(iClosest_Y_to_X);
			//TODO or remove this clusters?
		}


		float x2 = iVectorX.at<float>(0, IDX::X);
		float y2 = iVectorX.at<float>(0, IDX::Y);

		float dx = x2 - iClosest_Y_to_X.at<float>(0, IDX::X); //x1 - x2
		float dy = y2 - iClosest_Y_to_X.at<float>(0,IDX::Y); //y1 - y2


		float x1 = Rx.at<float>(iX, IDX::DX);
		float y1 = Rx.at<float>(iX, IDX::DY);
		Ry.at<float>(pos, IDX::DX) = x1 + dx;
		Ry.at<float>(pos, IDX::DY) = y1 + dy;


	}

	Ry.copyTo(_representativesY);
}

double TPCTSignatures::groundDistance(cv::Mat _ivectorX, cv::Mat _ivectorY) const
{
	double result = 0.0;

	for (int i = 0; i < DIMENSION-2; i++)
	{
		if (i == IDX::WEIGHT)
			continue;

		double distance = _ivectorX.at<float>(0, i) - _ivectorY.at<float>(0, i);
		result += distance * distance;

	}
	return std::sqrt(result);
}

