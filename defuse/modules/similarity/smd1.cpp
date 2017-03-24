#include "smd1.hpp"
#include "smd1parameter.hpp"
#include "minkowski1.hpp"

defuse::SMD::SMD(Parameter* _parameter, int _idxWeight, int _skipDim)
{
	mGrounddistance = dynamic_cast<SMDParamter*>(_parameter)->grounddistance.distance;
	mMatching = dynamic_cast<SMDParamter*>(_parameter)->matching;
	mDirection = dynamic_cast<SMDParamter*>(_parameter)->direction;
	mCost = dynamic_cast<SMDParamter*>(_parameter)->cost;
	mLamda = dynamic_cast<SMDParamter*>(_parameter)->lambda;

	mIDXWeight = _idxWeight;
	mSkipDim = _skipDim;
	
	mGDParam = new MinkowskiParamter();
	mGDParam->distance = mGrounddistance;
	mGDDistance = new Minkowski(mGDParam);

}

float defuse::SMD::compute(Features& _f1, Features& _f2)
{
	float dist = 0.0;
	if (mDirection == 0)
	{
		dist = bidirectional(_f1, _f2);
	}
	else if (mDirection == 1)
	{
		dist = assymmetrischQuery(_f1, _f2);
	}
	else if (mDirection == 2)
	{
		dist = assymmetrischDB(_f1, _f2);
	}

	return dist;
}

float defuse::SMD::compute(Features& _f1, int _idx1, Features& _f2, int _idx2) const
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
	return -10000.0;
}

float defuse::SMD::compute(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim) const
{
	if(mGrounddistance == 1)
	{
		return mGDDistance->computeL1(_f1, _idx1, _f2, _idx2, skipDim);
	}else if(mGrounddistance == 2)
	{
		return mGDDistance->computeL2(_f1, _idx1, _f2, _idx2, skipDim);
	}

	LOG_FATAL("Minkowski distance greater than 2 are not implemented. Aborted!");
	return -10000.0;
}

float defuse::SMD::assymmetrischQuery(Features& _f1, Features& _f2) const
{
	double result = 0;

	for (int i = 0; i < _f1.mVectors.rows; i++)
	{
		float minDist = FLT_MAX;

		for (int j = 0; j < _f2.mVectors.rows; j++)
		{
			float dist = 0.0;

			if (mSkipDim == -1)
				dist = compute(_f1, i, _f2, j);
			else
				dist = compute(_f1, i, _f2, j, mSkipDim);

			if (mCost == 0)
				dist = weightedDistance(_f1, i, dist);
			else
				dist = weightedDistance(_f1, i, dist); //TODO IMPL Distance weight ratio

			if (dist < minDist)
			{
				minDist = dist;
			}
		}

		result += minDist;
	}
	return result;
}

float defuse::SMD::assymmetrischDB(Features& _f1, Features& _f2) const
{
	double result = 0;

	for (int i = 0; i < _f2.mVectors.rows; i++)
	{
		float minDist = FLT_MAX;

		for (int j = 0; j < _f1.mVectors.rows; j++)
		{
			float dist = 0.0;

			if (mSkipDim == -1)
				dist = compute(_f2, i, _f1, j);
			else
				dist = compute(_f2, i, _f1, j, mSkipDim);

			if (mCost == 0)
				dist = weightedDistance(_f2, i, dist);
			else
				dist = weightedDistance(_f2, i, dist); //TODO IMPL Distance weight ratio

			if (dist < minDist)
			{
				minDist = dist;
			}
		}

		result += minDist;
	}
	return result;
}

float defuse::SMD::bidirectional(Features& _f1, Features& _f2) const
{
	int nrSigF1 = _f1.mVectors.rows;
	int nrSigF2 = _f2.mVectors.rows;

	float result = 0.0;

	//find all nearest from f1 in f2
	float costF1ToF2 = 0.0;
	float distF1ToF2 = 0.0;
	float costF1BetweenF2 = 0.0;
	for (int i = 0; i < nrSigF1; i++)
	{
		int closestIdx = nearestNeighbor(_f1, i, _f2);
		distF1ToF2 = compute(_f1, i, _f2, closestIdx);
		costF1ToF2 += weightedDistance(_f1, i, distF1ToF2);

		int closestInF1 = nearestNeighbor(_f2, closestIdx, _f1);
		if (i == closestInF1) //current idx the same than bidirectional match found
		{
			float tmpDist = compute(_f1, i, _f2, closestIdx);
			costF1BetweenF2 += weightedDistance(_f1, i, tmpDist);
		}
	}

	//find all nearest from f2 in f1
	float distF2ToF1 = 0.0;
	float costsF2ToF1 = 0.0;
	for (int i = 0; i < nrSigF2; i++)
	{
		int closestIdx = nearestNeighbor(_f2, i, _f1);

		distF2ToF1 = compute(_f2, i, _f1, closestIdx);
		costsF2ToF1 += weightedDistance(_f2, i, distF2ToF1);

	}

	result = costF1ToF2 + costsF2ToF1 - 2 * mLamda * costF1BetweenF2;

	return result;
}


/*
Returns the idx in f2 of the nearest neighbor from f1 to f2
*/
int defuse::SMD::nearestNeighbor(Features& _f1, int idx1, Features& _f2) const
{
	cv::Mat f2 = _f2.mVectors;

	int pos = 0;
	float dist = 0.0;

	float minimalDist = std::numeric_limits<float>::max();
	for (int j = 0; j < f2.rows; j++)
	{
		dist = compute(_f1, idx1, _f2, j);// *_f1.mVectors.at<float>(idx1, _f1.mVectors.cols - 1);

		if ((dist < minimalDist))
		{
			pos = j;
			minimalDist = dist;
		}

	}

	return pos;
}

float defuse::SMD::weightedDistance(Features& _f1, int idx1, float distance) const
{
	return distance * _f1.mVectors.at<float>(idx1, mIDXWeight);
}
