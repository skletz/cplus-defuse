/*
* PCT Signatures is an implementation of feature signatures.
* It was implemented with the focus to contribute to the
* OpenCV's extra modules. This module is a preliminary version
* which was received in 2015. In more detail, this module
* implements PCT (position-color-texture) signature extractor
* and SQFD (Signature Quadratic Form Distance).
*
* @author Gregor Kovalcik, Martin Krulis, Jakub Lokoc
* @repository https://github.com/GregorKovalcik/opencv_contrib
* @version 1.0 19/10/15
*/
#ifndef PCT_SIGNATURES_CONSTANTS_HPP
#define PCT_SIGNATURES_CONSTANTS_HPP

#include <cstddef>

namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{
			
			const std::size_t SIGNATURE_DIMENSION = 8;

			const std::size_t X_IDX = 0;
			const std::size_t Y_IDX = 1;
			const std::size_t L_IDX = 2;
			const std::size_t A_IDX = 3;
			const std::size_t B_IDX = 4;
			const std::size_t CONTRAST_IDX = 5;
			const std::size_t ENTROPY_IDX = 6;
			const std::size_t WEIGHT_IDX = 7;

			const float L_COLOR_RANGE = 100;
			const float A_COLOR_RANGE = 127;
			const float B_COLOR_RANGE = 127;

			const float SAMPLER_CONTRAST_NORMALIZER = 25.0f;
			const float SAMPLER_ENTROPY_NORMALIZER = 4.0f;


		}
	}
}

#endif
