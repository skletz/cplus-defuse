#ifndef _DEFUSE_VALUATOR_HPP_
#define _DEFUSE_VALUATOR_HPP_

#include "evaluatedquery.hpp"
#include "resultbase.hpp"
#include "query.hpp"

namespace defuse {

	class Valuator
	{

	public:
		bool display = false;

		virtual ~Valuator() {	}
		virtual double valuate(defuse::Query* _query) = 0;
	};
}

#endif //_DEFUSE_VALUATOR_HPP_
