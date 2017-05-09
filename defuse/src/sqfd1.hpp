#ifndef _DEFUSE_SQFD1_HPP_
#define _DEFUSE_SQFD1_HPP_

#ifdef __cplusplus

#include "minkowski1parameter.hpp"
#include "minkowski1.hpp"
#include <cplusutil.hpp>
#include "distance.hpp"
#include "parameter.hpp"

namespace defuse {

	class SQFD : public Distance
	{
	private:
		Minkowski* mGDDistance;
		MinkowskiParamter* mGDParam;
	public:
		int mGrounddistance;
		int mSimilarity;
		float mAlpha;

		int mIDXWeight;

		int mSkipDim = -1;

		SQFD(Parameter* _parameter, int _idxWeight, int _skipDim);

		float compute(Features& _f1, Features& _f2) override;
		float compute(Features& _f1, int _idx1, Features& _f2, int _idx2) const;
		float compute(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim) const;

		double computePartialSQFD(Features& _f1, Features& _f2) const;
	};
}
#endif
#endif