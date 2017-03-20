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
	};

}
#endif
#endif