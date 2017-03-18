#ifndef _DEFUSE_PARAMETER_HPP_
#define _DEFUSE_PARAMETER_HPP_
#ifdef __cplusplus

#include <string>

namespace defuse {

	class Parameter
	{
	public:
		virtual std::string get() = 0;
		virtual std::string getFilename() = 0;
	};

}
#endif
#endif