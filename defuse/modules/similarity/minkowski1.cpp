#include "minkowski1.hpp"
#include "gd1parameter.hpp"

defuse::Minkowski::Minkowski(Parameter* _parameter)
{
	mDistance = dynamic_cast<GDParamter*>(_parameter)->distance;
}

float defuse::Minkowski::compute(Features& _f1, Features& _f2)
{
	return 0.0;
}
