#include "smd.hpp"
#include "minkowski.hpp"
#include <iostream>
#include <sstream>

defuse::SMD::SMD(Minkowski* _GDDistance, int _direction, float _lambda)
{
	mGDDistance = _GDDistance;
	mDirection = _direction;
	mLambda = _lambda;
}

float defuse::SMD::compute(cv::Mat& _f1, cv::Mat& _f2) const
{
	float dist = 0.0;
	if (mDirection == 0)
	{
		dist = bidirectional(_f1, _f2);
	}
	else if (mDirection == 1)
	{
		dist = asymmetricQuery(_f1, _f2);
	}
	else if (mDirection == 2)
	{
		dist = assymetricDB(_f1, _f2);
	}

	return dist;
}

float defuse::SMD::compute(cv::Mat& _f1, int _idx1, cv::Mat& _f2, int _idx2) const
{
	return mGDDistance->compute(_f1, _idx1, _f2, _idx2);
}


float defuse::SMD::asymmetricQuery(cv::Mat& _f1, cv::Mat& _f2) const
{
	float result = 0;

	//for each representative in f1, find
	//the most similar in f2 (nearest neighbor matching)
	for (int i = 0; i < _f1.rows; i++)
	{
		float minDist = FLT_MAX;

		for (int j = 0; j < _f2.rows; j++)
		{
			float dist = 0.0;

			dist = compute(_f1, i, _f2, j);

			dist = weightedDistance(_f1, i, dist);

			if (dist < minDist)
			{
				minDist = dist;
			}
		}

		result += minDist;
	}
	return result;
}

float defuse::SMD::assymetricDB(cv::Mat& _f1, cv::Mat& _f2) const
{
	float result = 0;

	//for each representative in f2, find
	//the most similar in f1 (nearest neighbor matching)
	for (int i = 0; i < _f2.rows; i++)
	{
		float minDist = FLT_MAX;

		for (int j = 0; j < _f1.rows; j++)
		{
			float dist = 0.0;

			dist = compute(_f2, i, _f1, j);

			dist = weightedDistance(_f2, i, dist);

			if (dist < minDist)
			{
				minDist = dist;
			}
		}

		result += minDist;
	}
	return result;
}

float defuse::SMD::bidirectional(cv::Mat& _f1, cv::Mat& _f2) const
{
	float dist = 0.0;

	float costF1ToF2 = 0.0;
	float distF1ToF2 = 0.0;

	float costF1BetweenF2 = 0.0;

	for (int i = 0; i < _f1.rows; i++)
	{
		//for each representative in f1,
		//find the position of the most similar in f2
		int closestIdx = nearestNeighbor(_f1, i, _f2);
		distF1ToF2 = compute(_f1, i, _f2, closestIdx);
		costF1ToF2 += weightedDistance(_f1, i, distF1ToF2);

		//if the founded representative is also
		//the most similar in f1 (bidirectional match found)
		int closestInF1 = nearestNeighbor(_f2, closestIdx, _f1);
		if (i == closestInF1)
		{
			//sums up costs
			costF1BetweenF2 += weightedDistance(_f1, i, distF1ToF2);
		}
	}

	float distF2ToF1 = 0.0;
	float costF2ToF1 = 0.0;

	//for each representative in f2,
	//find the position of the most similar in f1
	for (int i = 0; i < _f2.rows; i++)
	{
		int closestIdx = nearestNeighbor(_f2, i, _f1);

		distF2ToF1 = compute(_f2, i, _f1, closestIdx);
		costF2ToF1 += weightedDistance(_f2, i, distF2ToF1);
	}

	//sums up costs and subtract bidirectional matches
	dist = costF1ToF2 + costF2ToF1 - (2 * mLambda * costF1BetweenF2);

	return dist;
}


int defuse::SMD::nearestNeighbor(cv::Mat& _f1, int idx1, cv::Mat& _f2) const
{
	int pos = 0;
	float dist = 0.0;

	float minimalDist = std::numeric_limits<float>::max();
	for (int j = 0; j < _f2.rows; j++)
	{
		dist = compute(_f1, idx1, _f2, j);

		if (dist < minimalDist)
		{
			pos = j;
			minimalDist = dist;
		}
	}

	return pos;
}

float defuse::SMD::weightedDistance(cv::Mat& _f1, int idx1, float distance) const
{
	float dist = 0.0;
	dist = distance * _f1.at<float>(idx1, _f1.cols - 1); //last position contains the weights
	return dist;
}
	 

std::string defuse::SMD::toString() const
{
	std::stringstream st;
	st << mGDDistance->toString() << ";";
	st << "Direction: D" << mDirection << ";";
	st << "Lamda: L" << mLambda;
	return st.str();
}