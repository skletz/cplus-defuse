#ifndef _DEFUSE_EVALUATEDQUERY_HPP_
#define _DEFUSE_EVALUATEDQUERY_HPP_
#ifdef __cplusplus

#include <cplusutil.h>


namespace defuse {

	class EvaluatedQuery
	{
	public:

		int mVideoID;

		int mClazz;

		unsigned int mStartFrameNumber;

		unsigned int mFrameCount;

		float mAPValue;
		EvaluatedQuery() {};
	};
}
#endif
#endif