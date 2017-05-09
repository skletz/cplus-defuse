#ifndef _DEFUSE_MAHALANOBIS_HPP_
#define _DEFUSE_MAHALANOBIS_HPP_
#ifdef __cplusplus

#include <cplusutil.hpp>
#include "distance.hpp"
#include "parameter.hpp"

namespace defuse {

	class Mahalanobis : public Distance
	{
	public:
		float compute(Features& _f1, Features& _f2) override;
	};
}
#endif
#endif