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
#ifndef PCT_SIGNATURES_GRAYSCALE_BITMAP_HPP
#define PCT_SIGNATURES_GRAYSCALE_BITMAP_HPP

#include <cstdint>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"


namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{
			/**
			* \brief Specific implementation of grayscale bitmap. This bitmap is used
			*		to compute contrast and entropy features from surroundings of a pixel.
			*/
			class GrayscaleBitmap
			{
			public:
				/**
				* \brief Initialize the grayscale bitmap from regular bitmap.
				* \param bitmap Bitmap used as source of data.
				* \param bitsPerPixel How many bits occupy one pixel in grayscale (e.g., 8 ~ 256 grayscale values).
				*		Must be within (1, 32) range.
				*/
				GrayscaleBitmap(const cv::InputArray _bitmap, std::size_t bitsPerPixel = 4);

				/**
				* \brief Return the width of the image in pixels.
				*/
				virtual std::size_t getWidth() const
				{
					return mWidth;
				}

				/**
				* \brief Return the height of the image in pixels.
				*/
				virtual std::size_t getHeight() const
				{
					return mHeight;
				}

				/**
				* \brief Return the height of the image in pixels.
				*/
				virtual std::size_t getBitsPerPixel() const
				{
					return mBitsPerPixel;
				}


				/**
				* \brief Compute contrast and entropy at selected coordinates.
				* \param x The horizontal coordinate of the pixel (0..width-1).
				* \param y The vertical coordinate of the pixel (0..height-1).
				* \param contrast Output variable where contrast value is saved.
				* \param entropy Output variable where entropy value is saved.
				* \param radius Radius of the rectangular window around selected pixel used for computing
				*		contrast and entropy. Size of the window side is (2*radius + 1).
				*		The window is cropped if [x,y] is too near the image border.
				*/
				virtual void getContrastEntropy(std::size_t x, std::size_t y, double &contrast, double &entropy, std::size_t windowRadius = 5);


				virtual void convertToMat(const cv::OutputArray _bitmap, bool normalize = false) const;


			protected:
				std::size_t mWidth;			///< Width of the image.
				std::size_t mHeight;		///< Height of the image.
				std::size_t mBitsPerPixel;	///< Number of bits per pixel.



			private:
				std::vector<std::uint32_t> mData;		///< Pixel data packed in 32-bit uints.
				std::vector<std::uint32_t> mHistogram;	///< Tmp matrix used for computing contrast and entropy.


				/**
				* \brief writeToLogFile pixel from packed data vector.
				* \param x The horizontal coordinate of the pixel (0..width-1).
				* \param y The vertical coordinate of the pixel (0..height-1).
				* \return Grayscale value (0 ~ black, 2^bitPerPixel - 1 ~ white).
				*/
				std::uint32_t getPixel(std::size_t x, std::size_t y) const
				{
					std::size_t pixelsPerItem = 32 / mBitsPerPixel;
					std::size_t offset = y*mWidth + x;
					std::size_t shift = (offset % pixelsPerItem) * mBitsPerPixel;
					std::uint32_t mask = (1 << mBitsPerPixel) - 1;
					return (mData[offset / pixelsPerItem] >> shift) & mask;
				}

				/**
				* \brief Set pixel in the packed data vector.
				* \param x The horizontal coordinate of the pixel (0..width-1).
				* \param y The vertical coordinate of the pixel (0..height-1).
				* \param val Grayscale value (0 ~ black, 2^bitPerPixel - 1 ~ white).
				*/
				void setPixel(std::size_t x, std::size_t y, std::uint32_t val)
				{
					std::size_t pixelsPerItem = 32 / mBitsPerPixel;
					std::size_t offset = y*mWidth + x;
					std::size_t shift = (offset % pixelsPerItem) * mBitsPerPixel;
					std::uint32_t mask = (1 << mBitsPerPixel) - 1;
					val &= mask;
					mData[offset / pixelsPerItem] &= ~(mask << shift);
					mData[offset / pixelsPerItem] |= val << shift;
				}

				/**
				* \brief Perform an update of contrast matrix.
				*/
				void updateHistogram(std::uint32_t a, std::uint32_t b)
				{
					int offset = (int)((a > b) ? (a << mBitsPerPixel) + b : a + (b << mBitsPerPixel));	// merge to a variable with greater higher bits
					mHistogram[offset]++;												// to accumulate just in a triangle in 2D histogram for efficiency
				}


			};
		}
	}
}

#endif
