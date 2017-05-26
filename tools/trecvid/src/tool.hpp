#ifndef _TOOL_HPP_
#define  _TOOL_HPP_

#include <boost/program_options.hpp>

namespace trecvid {

	/**
	* \brief
	*/
	class ToolBase
	{

	protected:
		boost::program_options::variables_map mArgs;

	public:
		virtual void setProgrammOptions(boost::program_options::variables_map _args) = 0;

	};
}

#endif //_TOOL_HPP_