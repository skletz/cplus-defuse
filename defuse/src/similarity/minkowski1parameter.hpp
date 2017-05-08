#ifndef _DEFUSE_GD1PARAMETER_HPP_
#define _DEFUSE_GD1PARAMETER_HPP_
#include "../parameter.hpp"
#ifdef __cplusplus

#include <string>
#include <sstream>

namespace defuse {

	//Paramters for Ground Distance
	class MinkowskiParamter : public Parameter
	{
	public:

		int distance = 2; //The distance function 2 = euclidian distance

		std::string get() override
		{
			std::stringstream st;
			st << "Ground distance: L" << distance;
			return st.str();
		}

		std::string getFilename() override
		{
			std::stringstream st;
			st << "GD";
			st << "_L" << distance;
			return st.str();
		}
	};

}
#endif
#endif