#ifndef _DEFUSE_SQFD1_HPP_
#define _DEFUSE_SQFD1_HPP_
#ifdef __cplusplus

#include <cplusutil.h>
#include "distance.hpp"
#include "../parameter.hpp"

namespace defuse {

	class SQFD : public Distance
	{

	public:
		int mGrounddistance;
		int mSimilarity;
		float mAlpha;

		int mIDXWeight;

		int mSkipDim = -1;

		SQFD(Parameter* _parameter, int _idxWeight, int _skipDim);

		float compute(Features& _f1, Features& _f2) override;
		double computePartialSQFD(Features& _f1, Features& _f2);
	};
}
#endif
#endif