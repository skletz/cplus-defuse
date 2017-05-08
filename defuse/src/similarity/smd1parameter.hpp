#ifndef _DEFUSE_SMD1PARAMETER_HPP_
#define _DEFUSE_SMD1PARAMETER_HPP_
#include "../parameter.hpp"
#include "minkowski1parameter.hpp"
#ifdef __cplusplus


#include <string>
#include <sstream>
namespace defuse {
	//Paramters for Signature Matching Distance
	class SMDParamter : public Parameter
	{
	public:

		MinkowskiParamter grounddistance; //The distance function 2 = euclidian distance
		int matching = 0; //The matching strategy 0 = nearest neighbor
		int direction = 0; //The direction 0 = bidirectionl; 1 = assymmetrisch to Q, 2 = assymmetrisch to DB
		int cost = 0;	//The cost function
		float lambda = 1.0; //influence of bidirectional matches

		std::string get() override
		{
			std::stringstream st;
			st << grounddistance.get();
			st << "; Matching: M" << matching;
			st << "; Direction: D" << direction;
			st << "; Cost: C" << cost;
			st << "; Lamda: L" << lambda;
			return st.str();
		}

		std::string getFilename() override
		{
			std::stringstream st;
			st << "SMD_";
			st << grounddistance.getFilename();
			st << "_M" << matching;
			st << "_D" << direction;
			st << "_C" << cost;
			st << "_L" << lambda;
			return st.str();
		}
	};

}
#endif
#endif