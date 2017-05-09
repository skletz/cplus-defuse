#include "grayscale_bitmap.hpp"

namespace cv
{
	namespace xfeatures2d
	{
		namespace pct_signatures
		{
			GrayscaleBitmap::GrayscaleBitmap(const cv::InputArray _bitmap, std::size_t bitsPerPixel)
				: mBitsPerPixel(bitsPerPixel)
			{
				Mat bitmap = _bitmap.getMat();
				if (bitmap.empty())
				{
					CV_Error(CV_StsBadArg, "Input bitmap is empty");
				}
				if (bitmap.depth() != CV_8U && bitmap.depth() != CV_16U)
				{
					CV_Error(CV_StsUnsupportedFormat, "Input bitmap depth must be CV_8U or CV_16U");
				}
				if (bitmap.depth() == CV_8U)
				{
					bitmap.convertTo(bitmap, CV_16U, 257);
				}

				Mat grayscaleBitmap;
				cvtColor(bitmap, grayscaleBitmap, cv::COLOR_BGR2GRAY);

				mWidth = bitmap.cols;
				mHeight = bitmap.rows;

				if (bitsPerPixel == 0 || bitsPerPixel > 32)
				{
					CV_Error_(CV_StsBadArg, ("Invalid number of bits per pixel %d. Only values in range (1, 32) are accepted.", bitsPerPixel));
				}

				// Allocate space for pixel data.
				std::size_t pixelsPerItem = 32 / mBitsPerPixel;
				mData.resize((mWidth*mHeight + pixelsPerItem - 1) / pixelsPerItem);

				// Convert the bitmap to grayscale and fill the pixel data.
				CV_Assert(grayscaleBitmap.depth() == CV_16U);
				for (std::size_t y = 0; y < mHeight; y++)
				{
					for (std::size_t x = 0; x < mWidth; x++)
					{
						std::uint32_t grayVal = static_cast<std::uint32_t>(grayscaleBitmap.at<std::uint16_t>(static_cast<int>(y), static_cast<int>(x))) >> (16 - mBitsPerPixel);
						setPixel(x, y, grayVal);
					}
				}
				// Prepare the preallocated contrast matrix for contrast-entropy computations
				mHistogram.resize(1 << (mBitsPerPixel * 2));	// mHistogram size = maxPixelValue^2
			}


			void GrayscaleBitmap::getContrastEntropy(std::size_t x, std::size_t y, double &contrast, double &entropy, std::size_t radius)
			{
				std::size_t fromX = (x > radius) ? x - radius : 0;
				std::size_t fromY = (y > radius) ? y - radius : 0;
				std::size_t toX = std::min<std::size_t>(mWidth - 1, x + radius + 1);
				std::size_t toY = std::min<std::size_t>(mHeight - 1, y + radius + 1);
				for (std::size_t j = fromY; j < toY; ++j)
				{
					for (std::size_t i = fromX; i < toX; ++i)							// for each pixel in the window
					{
						updateHistogram(getPixel(i, j), getPixel(i, j + 1));			// match every pixel with all 8 its neighbours
						updateHistogram(getPixel(i, j), getPixel(i + 1, j));
						updateHistogram(getPixel(i, j), getPixel(i + 1, j + 1));
						updateHistogram(getPixel(i + 1, j), getPixel(i, j + 1));		// 4 updates per pixel in the window
					}
				}

				contrast = 0.0;
				entropy = 0.0;

				std::uint32_t pixelsScale = 1 << mBitsPerPixel;
				double normalizer = (double)((toX - fromX) * (toY - fromY) * 4);				// four increments per pixel in the window (see above)
				for (std::size_t j = 0; j < pixelsScale; ++j)								// iterate row in a 2D histogram
				{
					for (std::size_t i = 0; i <= j; ++i)									// iterate column up to the diagonal in 2D histogram
					{
						if (mHistogram[j*pixelsScale + i] != 0) 							// consider only non-zero values
						{
							double value = (double)mHistogram[j*pixelsScale + i] / normalizer;	// normalize value by number of histogram updates
							contrast += (i - j) * (i - j) * value;		// compute contrast
							entropy -= value * std::log(value);			// compute entropy
							mHistogram[j*pixelsScale + i] = 0;			// clear the histogram array for the next computation
						}
					}
				}
			}


			void GrayscaleBitmap::convertToMat(const cv::OutputArray _bitmap, bool normalize) const
			{
				_bitmap.create((int)getHeight(), (int)getWidth(), CV_8U);
				cv::Mat bitmap = _bitmap.getMat();

				for (std::size_t y = 0; y < getHeight(); ++y)
				{
					for (std::size_t x = 0; x < getWidth(); ++x)
					{
						std::uint32_t pixel = getPixel(x, y);
						if (normalize && mBitsPerPixel < 8)
						{
							pixel <<= 8 - mBitsPerPixel;
						}
						else if (normalize && mBitsPerPixel > 8)
						{
							pixel >>= mBitsPerPixel - 8;
						}
						bitmap.at<uchar>((int)y, (int)x) = (uchar)pixel;	// TODO: optimalization
					}
				}
			}

		}
	}
}