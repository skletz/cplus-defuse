#ifndef _DEFUSE_MIKOWSKI1_HPP_
#define _DEFUSE_MIKOWSKI1_HPP_

#ifdef __cplusplus

#include <cplusutil.h>
#include "distance.hpp"
#include "../parameter.hpp"

namespace defuse {

	class Minkowski : public Distance
	{
	public:
		int mDistance;
		Minkowski(Parameter* _parameter);
		float compute(Features& _f1, Features& _f2) override;

		float computeL1(Features& _f1, Features& _f2) const;
		float computeL2(Features& _f1, Features& _f2) const;

		//Util - Ground Distance for SMD, SQFD
		float computeL1(Features& _f1, int _idx1, Features& _f2, int _idx2) const;
		float computeL1(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim) const;

		float computeL2(Features& _f1, int _idx1, Features& _f2, int _idx2) const;
		float computeL2(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim) const;

	};

}
#endif
#endif