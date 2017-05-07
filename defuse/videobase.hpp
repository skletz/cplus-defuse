#ifndef _DEFUSE_VIDEOBASE_HPP_
#define _DEFUSE_VIDEOBASE_HPP_
#ifdef __cplusplus

#include <cplusutil.hpp>

namespace defuse {

	class VideoBase
	{
	public:
		File* mFile = nullptr;

		std::string mVideoFileName;

		int mVideoID;

		int mStartFrameNumber;

		int mClazz;

	
		VideoBase(File* _file, int _videoID, int _startFrameNumber, int _clazz)
			: mFile(_file)
		{
			mVideoFileName = _file->getFilename();
			mVideoID = _videoID;
			mStartFrameNumber = _startFrameNumber;
			mClazz = _clazz;
		}

		~VideoBase();
	};

}
#endif
#endif