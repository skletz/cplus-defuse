#ifndef _DEFUSE_MHD1PARAMETER_HPP_
#define _DEFUSE_MHD1PARAMETER_HPP_
#include "parameter.hpp"
#ifdef __cplusplus

#include <string>
#include <sstream>

namespace defuse {

	//Paramters for Ground Distance
	class MHDParamter : public Parameter
	{
	public:

		std::string get() override
		{
			std::stringstream st;
			st << "";
			return st.str();
		}

		std::string getFilename() override
		{
			std::stringstream st;
			st << "";
			return st.str();
		}
	};

}
#endif
#endif