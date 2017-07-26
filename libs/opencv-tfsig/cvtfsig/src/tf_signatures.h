#ifndef __TFEATURESIGNATURES_H__
#define __TFEATURESIGNATURES_H__

#include "opencv2/core.hpp"
#include <limits>
#include <vector>
#include <iostream>

#include "constants.h"
#include <cvpctsig.h>

namespace tf_signatures
{
	class TFeatureSignature
	{
	private:
		cv::Ptr<cv::xfeatures2d::PCTSignatures> pctsignatures;

	public:
		TFeatureSignature(std::vector<cv::Point2f> initPoints, int initSampleCount, int initSeedCount)
		{
			pctsignatures = cv::xfeatures2d::PCTSignatures::create(initPoints, initSampleCount, initSeedCount);
		}


		void computeSignature(cv::InputArray& _frame, cv::OutputArray& _signature) const
		{
			pctsignatures->computeSignature(_frame, _signature);
			cv::Mat representatives;
			_signature.copyTo(representatives);

			cv::Mat temporal_representatives;
			int dims = tf_signatures ::SIGNATURE_DIMENSION - cv::xfeatures2d::pct_signatures::SIGNATURE_DIMENSION;

			cv::Size newsize(dims, representatives.rows); //add additional dimensions

			temporal_representatives = cv::Mat::zeros(newsize, representatives.type()); // init by zero => no movement
			cv::hconcat(representatives, temporal_representatives, temporal_representatives); //enlarge fs2
			temporal_representatives.copyTo(_signature);
		}

		void calculateMovement(const cv::InputArrayOfArrays& _signatures, cv::OutputArray& _tsignature) const
		{
			std::vector<cv::Mat> signatures;
			_signatures.getMatVector(signatures);

			for (int iSignature = signatures.size()-1; iSignature > 0; iSignature--)
			{
				NNbruteforcematching(signatures[iSignature], signatures[iSignature - 1]);

			}

			
			//signatures.front().copyTo(_tsignature);
			cv::Mat tmp;
			signatures.front().copyTo(tmp);
			for (int i = 0; i < tmp.rows; i++)
			{
				float x = tmp.at<float>(i, cv::xfeatures2d::pct_signatures::X_IDX);
				float y = tmp.at<float>(i, cv::xfeatures2d::pct_signatures::Y_IDX);

				float dx = tmp.at<float>(i, tf_signatures::DX_IDX);
				float dy = tmp.at<float>(i, tf_signatures::DY_IDX);

				float x2 = dx + x;
				float y2 = dy + y;

				tmp.at<float>(i, tf_signatures::DX_IDX) = x2;
				tmp.at<float>(i, tf_signatures::DY_IDX) = y2;

				//if (x2 < 0 || x2 > 1)
				//{
				//	std::cout << "x2 is not normalized" << std::endl;
				//}

				//if (y2 < 0 || y2 > 1)
				//{
				//	std::cout << "y2 is not normalized" << std::endl;
				//}
			}

			tmp.copyTo(_tsignature);

		}

		void drawMovement(const cv::InputArray& _tsignature, cv::OutputArray _result, int _width, int _height) const
		{

			//white background
			cv::Mat background(_height, _width, CV_8UC3, cv::Scalar(255, 255, 255));
			cv::Mat temporal_representatives;
			_tsignature.copyTo(temporal_representatives);

			drawSignature(background, temporal_representatives, _result);

			cv::Mat visualization;
			_result.copyTo(visualization);

			for (int i = 0; i < temporal_representatives.rows; i++)
			{
				float x = temporal_representatives.at<float>(i, cv::xfeatures2d::pct_signatures::X_IDX);
				float y = temporal_representatives.at<float>(i, cv::xfeatures2d::pct_signatures::Y_IDX);

				float dx = temporal_representatives.at<float>(i, tf_signatures::DX_IDX);
				float dy = temporal_representatives.at<float>(i, tf_signatures::DY_IDX);


				cv::Point start_point((x * _width), (y * _height));
				//cv::Point end_point(((x+dx) * _width), ((y+dy) * _height));
				cv::Point end_point(((dx) * _width), ((dy) * _height));

				cv::Vec3b purple(200, 40, 190);
				int borderThickness(1);

				cv::arrowedLine(visualization, start_point, end_point, purple, borderThickness + 1);
			}

			visualization.copyTo(_result);
		}

		void drawSignature(const cv::InputArray _source, const cv::InputArray _signature, cv::OutputArray _result) const
		{
			cv::Mat tmp;
			_signature.copyTo(tmp);
			int x_length = cv::xfeatures2d::pct_signatures::SIGNATURE_DIMENSION;
			int y_length = tmp.rows;

			cv::Mat representatives = tmp(cv::Rect(0, 0, x_length, y_length));
			pctsignatures->drawSignature(_source, representatives, _result);
		}

	private:
		/*
		* ______________________________
		* _representativesX = x, y, l, a, b, c, e, dx, dy
		* ______________________________
		*
		*/
		void NNbruteforcematching(const cv::InputArray& _representativesX, const cv::InputOutputArray& _representativesY) const
		{

			cv::Mat Rx, Ry;
			_representativesX.copyTo(Rx);
			_representativesY.copyTo(Ry);

			if (Rx.empty() || Ry.empty())
			{
				CV_Error(CV_StsBadArg, "Empty signature!");
			}

			if (Rx.cols != tf_signatures::SIGNATURE_DIMENSION || Ry.cols != tf_signatures::SIGNATURE_DIMENSION)
			{
				CV_Error_(CV_StsBadArg, ("Invalid signature format. Signature.cols must be %d.", SIGNATURE_DIMENSION));
			}

			cv::Mat iVectorX, iVectorY;
			double dist = 0;
			int pos = 0;
			for (int iX = 0; iX < Rx.rows; iX++)
			{
				double minimalDist = std::numeric_limits<double>::max();

				cv::Mat iClosest_Y_to_X;
				Rx.row(iX).copyTo(iVectorX);

				for (int iY = 0; iY < Ry.rows; iY++)
				{
					Ry.row(iY).copyTo(iVectorY);
					//
					dist = groundDistance(iVectorX, iVectorY);

					//right uniqueness
					if ((dist < minimalDist) && ((iVectorY.at<float>(0, tf_signatures::DX_IDX) + iVectorY.at<float>(0, tf_signatures::DY_IDX)) == 0))
					{
						pos = iY;
						minimalDist = dist;
						
						iVectorY.copyTo(iClosest_Y_to_X);
					}
				}

				//TODO ASSERT any other cases why we could not found a nearest vector?
				//TODO if RX.cols > RY.cols then we do not found for all clusters in RX a nearest cluster in RY - then ??
				if (iClosest_Y_to_X.empty())
				{
					//break; //left totality
					iVectorX.copyTo(iClosest_Y_to_X);
					//TODO or remove this clusters?
				}
					

				float x2 = iVectorX.at<float>(0, cv::xfeatures2d::pct_signatures::X_IDX);
				float y2 = iVectorX.at<float>(0, cv::xfeatures2d::pct_signatures::Y_IDX);

				float dx = x2 - iClosest_Y_to_X.at<float>(0, cv::xfeatures2d::pct_signatures::X_IDX); //x1 - x2
				float dy = y2 - iClosest_Y_to_X.at<float>(0, cv::xfeatures2d::pct_signatures::Y_IDX); //y1 - y2


				float x1 = Rx.at<float>(iX, tf_signatures::DX_IDX);
				float y1 = Rx.at<float>(iX, tf_signatures::DY_IDX);
				Ry.at<float>(pos, tf_signatures::DX_IDX) = x1 + dx;
				Ry.at<float>(pos, tf_signatures::DY_IDX) = y1 + dy;


			}

			Ry.copyTo(_representativesY);
		}

		double groundDistance(cv::Mat _ivectorX, cv::Mat _ivectorY) const
		{
			double result = 0.0;

			for (int i = 0; i < cv::xfeatures2d::pct_signatures::SIGNATURE_DIMENSION; i++)
			{
				if (i == cv::xfeatures2d::pct_signatures::WEIGHT_IDX)
					continue;

				double distance = _ivectorX.at<float>(0, i) - _ivectorY.at<float>(0, i);
				result += distance * distance;

			}
			return std::sqrt(result);
		}

	};


}

#endif //__TFEATURESIGNATURES_H__
