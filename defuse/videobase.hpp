#ifndef _DEFUSE_VIDEOBASE_HPP_
#define _DEFUSE_VIDEOBASE_HPP_
#ifdef __cplusplus

#include <cplusutil.h>

namespace defuse {

	class VideoBase
	{
	public:
		File* mFile = nullptr;

		int mVideoID;

		int mStartFrameNumber;

		int mClazz;

	
		VideoBase(File* _file, int _videoID, int _startFrameNumber, int _clazz)
			: mFile(_file)
		{
			mVideoID = _videoID;
			mStartFrameNumber = _startFrameNumber;
			mClazz = _clazz;
		}

		~VideoBase();
	};

}
#endif
#endif