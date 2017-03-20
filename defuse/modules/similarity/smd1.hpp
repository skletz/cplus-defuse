#ifndef _DEFUSE_SMD1_HPP_
#define _DEFUSE_SMD1_HPP_
#ifdef __cplusplus

#include <cplusutil.h>
#include "distance.hpp"
#include "../parameter.hpp"

namespace defuse {

	class SMD : public Distance
	{
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

		/*	_f1 Query
		_f2 DB Element
		*/
		float assymmetrischQuery(Features& _f1, Features& _f2);

		/*	_f1 Query
		_f2 DB Element
		*/
		float assymmetrischDB(Features& _f1, Features& _f2);


		float bidirectional(Features& _f1, Features& _f2);

		int nearestNeighbor(Features& _f1, int idx1, Features& _f2);

		float weightedDistance(Features& _f1, int idx1, float distance) const;
	};
}
#endif
#endif