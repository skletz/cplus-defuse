#ifndef _DATA_INFO_HPP_
#define  _DATA_INFO_HPP_

#include "tool.hpp"

namespace trecvid {

	/**
	 * \brief 
	 */
	class DataInfo : public ToolBase
	{
	public:
		
		DataInfo();

		void setProgrammOptions(boost::program_options::variables_map _args) override;

		
	};
}

#endif //_DATA_INFO_HPP_