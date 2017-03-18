#ifndef _DEFUSE_MOHIST1PARAMETER_HPP_
#define _DEFUSE_MOHIST1PARAMETER_HPP_
#ifdef __cplusplus

#include <cplusutil.h>
#include "../parameter.hpp"

namespace defuse {

	class MoHist1Parameter : public Parameter
	{
	public:

		int samplex = 8;
		int sampley = 8;

		int frames = 0; //The number of frames per second 0 = all
		int frameSelection = 1;

	

		std::string get() override
		{
			std::stringstream st;
			st << "SampleX: " << samplex;
			st << "; SampleY: " << sampley;
			st << "; Frams per second 0 = all: " << frames;
			return st.str();
		}

		std::string getFilename() override
		{
			std::stringstream st;
			st << "MoHist1_";
			st << samplex << "x" << sampley;
			st << "_" << frames;
			return st.str();
		}
	};

}
#endif
#endif