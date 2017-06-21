#ifndef _DEFUSE_EVALUATEDQUERY_HPP_
#define _DEFUSE_EVALUATEDQUERY_HPP_
#ifdef __cplusplus

#include <cplusutil.hpp>


namespace defuse {

	class EvaluatedQuery
	{
	public:

		std::string mVideoFileName;

		int mVideoID;

		int mQueryID;

		int mShotID;

		float mAPValue;

		float mAvgSearchtime;

		EvaluatedQuery() {};
	};
}
#endif
#endif