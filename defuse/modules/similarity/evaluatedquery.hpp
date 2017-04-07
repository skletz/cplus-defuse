#ifndef _DEFUSE_EVALUATEDQUERY_HPP_
#define _DEFUSE_EVALUATEDQUERY_HPP_
#ifdef __cplusplus

#include <cplusutil.h>


namespace defuse {

	class EvaluatedQuery
	{
	public:

		std::string mVideoFileName;

		int mVideoID;

		int mClazz;

		unsigned int mStartFrameNumber;

		unsigned int mFrameCount;

		float mAPValue;

		float mSearchtime;
		EvaluatedQuery() {};
	};
}
#endif
#endif