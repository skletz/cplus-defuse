#ifndef _DEFUSE_DFS2PARAMETER_HPP_
#define _DEFUSE_DFS2PARAMETER_HPP_
#ifdef __cplusplus

#include <cplusutil.hpp>
#include "../parameter.hpp"
#include "sfs1parameter.hpp"

namespace defuse {

	class DFS2Parameter : public Parameter
	{
	public:
		SFS1Parameter staticparamter;
		int frames = 5;		//The number of frames
		int frameSelection = 0; //per segment

		std::string get() override
		{
			std::stringstream st;
			st << staticparamter.get();
			st << "; Frams per segment: " << frames;
			return st.str();
		}

		std::string getFilename() override
		{
			std::stringstream st;
			st << "DFS2_";
			st << staticparamter.getFilename();
			st << "_" << frameSelection;
			st << "_" << frames;
			return st.str();
		}
	};

}
#endif
#endif