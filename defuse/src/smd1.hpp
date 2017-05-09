#ifndef _DEFUSE_SMD1_HPP_
#define _DEFUSE_SMD1_HPP_

#ifdef __cplusplus
#include "minkowski1parameter.hpp"
#include "minkowski1.hpp"
#include <cplusutil.hpp>
#include "distance.hpp"
#include "parameter.hpp"

namespace defuse {

	class SMD : public Distance
	{
	private:
		Minkowski* mGDDistance;
		MinkowskiParamter* mGDParam;
	
	public:
		int mGrounddistance;
		int mMatching;
		int mDirection;
		int mCost;
		float mLamda = 1.0;

		int mIDXWeight;

		int mSkipDim = -1;

		SMD(Parameter* _parameter, int _idxWeight, int _skipDim);

		float compute(Features& _f1, Features& _f2) override;

		float compute(Features& _f1, int _idx1, Features& _f2, int _idx2) const;
		float compute(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim) const;

		/*	_f1 Query
		_f2 DB Element
		*/
		float assymmetrischQuery(Features& _f1, Features& _f2) const;

		/*	_f1 Query
		_f2 DB Element
		*/
		float assymmetrischDB(Features& _f1, Features& _f2) const;


		float bidirectional(Features& _f1, Features& _f2) const;

		int nearestNeighbor(Features& _f1, int idx1, Features& _f2) const;

		float weightedDistance(Features& _f1, int idx1, float distance) const;
	};
}
#endif
#endif