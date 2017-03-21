#include "sqfd1.hpp"
#include <future>
#include "sqfd1parameter.hpp"

defuse::SQFD::SQFD(Parameter* _parameter, int _idxWeight, int _skipDim)
{
	mGrounddistance = dynamic_cast<SQFDParamter *>(_parameter)->grounddistance.distance;
	mSimilarity = dynamic_cast<SQFDParamter *>(_parameter)->similarity;
	mAlpha = dynamic_cast<SQFDParamter *>(_parameter)->alpha;

	mIDXWeight = _idxWeight;
	mSkipDim = _skipDim;

	mGDParam = new MinkowskiParamter();
	mGDParam->distance = mGrounddistance;
	mGDDistance = new Minkowski(mGDParam);
}

float defuse::SQFD::compute(Features& _f1, Features& _f2)
{
	double result = 0;

	std::vector<std::future<double>> pendingFutures;


	pendingFutures.push_back(std::async(std::launch::async, &SQFD::computePartialSQFD, this, _f1, _f1));
	pendingFutures.push_back(std::async(std::launch::async, &SQFD::computePartialSQFD, this, _f2, _f2));
	std::future<double> result3(std::async(std::launch::async, &SQFD::computePartialSQFD, this, _f1, _f2));

	for (int i = 0; i < pendingFutures.size(); i++)
	{
		result += pendingFutures.at(i).get();
	}

	result -= result3.get() * 2;

	//result += computePartialSQFD(_f1, _f1);
	//result += computePartialSQFD(_f2, _f2);
	//result -= computePartialSQFD(_f1, _f2) * 2;

	return sqrt(result);
}

float defuse::SQFD::compute(Features& _f1, int _idx1, Features& _f2, int _idx2) const
{
	if (mGrounddistance == 1)
	{
		return mGDDistance->computeL1(_f1, _idx1, _f2, _idx2);
	}
	else if (mGrounddistance == 2)
	{
		return mGDDistance->computeL2(_f1, _idx1, _f2, _idx2);
	}

	LOG_FATAL("Minkowski distance greater than 2 are not implemented. Aborted!");
	return -1.0;

}

float defuse::SQFD::compute(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim) const
{
	if (mGrounddistance == 1)
	{
		return mGDDistance->computeL1(_f1, _idx1, _f2, _idx2, skipDim);
	}
	else if (mGrounddistance == 2)
	{
		return mGDDistance->computeL2(_f1, _idx1, _f2, _idx2, skipDim);
	}

	LOG_FATAL("Minkowski distance greater than 2 are not implemented. Aborted!");
	return -1.0;
}

double defuse::SQFD::computePartialSQFD(Features& _f1, Features& _f2) const
{
	float result = 0;
	for (int i = 0; i < _f1.mVectors.rows; i++)
	{
		for (int j = 0; j < _f2.mVectors.rows; j++)
		{
			float dist = 0.0;
			if (mGrounddistance == 2)
			{
				if (mSkipDim == -1)
					dist = compute(_f1, i, _f2, j);
				else
					dist = compute(_f1, i, _f2, j, mSkipDim);
			}
			else
			{
				if (mSkipDim == -1)
					dist = compute(_f1, i, _f2, j);
				else
					dist = compute(_f1, i, _f2, j, mSkipDim);
			}


			result +=
				_f1.mVectors.at<float>(i, mIDXWeight) *
				_f2.mVectors.at<float>(j, mIDXWeight) *
				(1 / (mAlpha + dist));
		}
	}
	return result;
}
