#ifndef _DEFUSE_DISTANCE_HPP_
#define _DEFUSE_DISTANCE_HPP_
#ifdef __cplusplus

#include "../features.hpp"

namespace defuse {

	class Distance
	{

	public:
		virtual ~Distance() = default;
		virtual float compute(Features& _f1, Features& _f2) = 0;
		float computeL2(Features& _f1, int _idx1, Features& _f2, int _idx2);

		float computeL2(Features& _f1, int _idx1, Features& _f2, int _idx2, int skipDim);
	};
}
#endif
#endif