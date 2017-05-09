#ifndef _DEFUSE_SQFD1PARAMETER_HPP_
#define _DEFUSE_SQFD1PARAMETER_HPP_

#ifdef __cplusplus

#include "parameter.hpp"
#include "minkowski1parameter.hpp"
#include <string>
#include <sstream>

namespace defuse {
	//Paramters for Signature Matching Distance
	class SQFDParamter : public Parameter
	{
	public:

		MinkowskiParamter grounddistance; //The distance function 2 = euclidian distance
		int similarity = 0; //The similarity funciton 0 = Heuristics
		float alpha = 1.0; //influence of cross distances

		std::string get() override
		{
			std::stringstream st;
			st << grounddistance.get();
			st << "; Similarity: S" << similarity;
			st << "; Alpha: A" << alpha;
			return st.str();
		}

		std::string getFilename() override
		{
			std::stringstream st;
			st << "SQFD_";
			st << grounddistance.getFilename();
			st << "_S" << similarity;
			st << "_A" << alpha;
			return st.str();
		}
	};

}
#endif
#endif