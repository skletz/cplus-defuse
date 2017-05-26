#ifndef _TRECVID_HPP_
#define  _TRECVID_HPP_

#include <vector>

namespace trecvid {

	struct MasterShotBoundary
	{
		unsigned int start;
		unsigned int end;
	};

	typedef std::vector<MasterShotBoundary> MasterShotBoundaries;

	struct VideoFile
	{
		int id;
		std::string filename;
		std::string use;
		std::string source;
		std::string filetype;
		MasterShotBoundaries msbs;
	};

	typedef std::vector<VideoFile> VideoFileList;

}

#endif //_TRECVID_HPP_