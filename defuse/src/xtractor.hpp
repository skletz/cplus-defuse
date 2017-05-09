#ifndef _DEFUSE_XTRACTOR_HPP_
#define _DEFUSE_XTRACTOR_HPP_
#ifdef __cplusplus

#include <string>
#include "../../libs/opencv-pctsig/cvpctsig//include/signatures.h"
#include "../../libs/opencv-tfsig/cvtfsig/include/signatures.h"

#include "features.hpp"
#include "videobase.hpp"

namespace defuse {

	class Xtractor
	{

	public:
		bool display = false;
		template <typename Enumeration>
		static auto as_integer(Enumeration const value)
			-> typename std::underlying_type<Enumeration>::type
		{
			return static_cast<typename std::underlying_type<Enumeration>::type>(value);
		}

		virtual ~Xtractor() {	}
		virtual Features* xtract(VideoBase* _videobase) = 0;
	};
}
#endif
#endif