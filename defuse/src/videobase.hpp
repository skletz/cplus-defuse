#ifndef _DEFUSE_VIDEOBASE_HPP_
#define _DEFUSE_VIDEOBASE_HPP_

#include <cplusutil.hpp>

namespace defuse {

	class VideoBase
	{
	public:

		/**
		 * \brief 
		 */
		File* mFile = nullptr;

		/**
		* \brief
		*/
		VideoBase(File* _file);

		/**
		* \brief
		*/
		~VideoBase() {};


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

	};
}
#endif //_DEFUSE_VIDEOBASE_HPP_