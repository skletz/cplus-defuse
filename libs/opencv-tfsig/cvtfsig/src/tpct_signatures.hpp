#ifndef __TPCTSIGNATURES_H__
#define __TPCTSIGNATURES_H__
#include <opencv2/core/mat.hpp>
#include <cvpctsig.h>

namespace analysis
{
	namespace tpct_signatures
	{
		class TPCTSignatures
		{
		public:
			enum IDX{ X, Y, L, A, B, CONTRAST, ENTROPY, WEIGHT, DX, DY};
			const int DIMENSION = 10;

			TPCTSignatures();
			~TPCTSignatures();
			void computeTemporalSignature(std::vector<cv::Mat>& staticsignatures, cv::OutputArray& _temporalsignature) const;
			void enlarge(cv::InputArray& _staticsignature, cv::OutputArray& _temporalsignature) const;

			static float computeQuadraticFormDistance(const cv::InputArray signature0, const cv::InputArray signature1, const cv::xfeatures2d::pct_signatures::Similarity &similarity = cv::xfeatures2d::pct_signatures::HeuristicSimilarity());
			

		private:
			void NNbruteforcematching(const cv::InputArray& _representativesX, const cv::InputOutputArray& _representativesY) const;
			double groundDistance(cv::Mat _ivectorX, cv::Mat _ivectorY) const;
			static float computePartialSQFD(const cv::Mat &signature0, const cv::Mat &signature1, const cv::xfeatures2d::pct_signatures::Similarity& similarity);
		};
	}
}


#endif //__MPCTSIGNATURES_H__
