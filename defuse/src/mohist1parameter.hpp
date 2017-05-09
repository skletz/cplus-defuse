#ifndef _DEFUSE_MOHIST1PARAMETER_HPP_
#define _DEFUSE_MOHIST1PARAMETER_HPP_
#ifdef __cplusplus

#include <cplusutil.hpp>
#include "parameter.hpp"

namespace defuse {

	class MoHist1Parameter : public Parameter
	{
	public:

		//int samplex = 8;
		//int sampley = 8;

		int samplepoints;
		std::string samplepointsfile = "";
		int distribution = 0; //The distribution of sample points 0 = random, 1 = regular, 2 = gaussian

		int frames = 0; //The number of frames
		int frameSelection = 0; //0 = per segment

		int samplex;
		int sampley;

		std::string imgOutputPath = "";

		std::string get() override
		{
			std::stringstream st;
			st << "Samplepoints: " << samplepoints;
			st << "; Distribution: " << distribution;
			st << "; Frams per second 0 = all: " << frames;
			return st.str();
		}

		std::string getFilename() override
		{
			std::stringstream st;
			st << "MoHist1_";
			st << samplepoints;
			st << "_" << distribution;
			st << "_" << frameSelection;
			st << "_" << frames;
			//TODO when used, add also samplex and sampley
			return st.str();
		}
	};

}
#endif
#endif