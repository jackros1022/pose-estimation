#ifndef Hough3d_H
#define Hough3d_H

#include <pcl/recognition/cg/hough_3d.h>

#include "../transformationestimation.hpp"
#include "../consoleargument.h"
#include "../logger.h"

namespace PoseEstimation
{
    /**
     * @brief Transformation estimation using Hough 3D Voting (Correspondence Grouping).
     */
    template<typename PointT, typename DescriptorT, typename LRFEstimatorT>
    class HoughVoting : public TransformationEstimator<PointT, DescriptorT>
    {
    public:
        virtual bool estimate(PC<PointT> &source,
                              PC<PointT> &target,
                              const typename pcl::PointCloud<PointT>::Ptr &source_keypoints,
                              const typename pcl::PointCloud<PointT>::Ptr &target_keypoints,
                              const typename pcl::PointCloud<DescriptorT>::Ptr &,
                              const typename pcl::PointCloud<DescriptorT>::Ptr &,
                              const pcl::CorrespondencesPtr &correspondences,
                              std::vector<Eigen::Matrix4f> &transformations)
        {
            pcl::PointCloud<RFType>::Ptr source_lrf(new pcl::PointCloud<RFType>);
            pcl::PointCloud<RFType>::Ptr target_lrf(new pcl::PointCloud<RFType>);
            _lrfEstimator.estimate(source, source_keypoints, source_lrf);
            _lrfEstimator.estimate(target, target_keypoints, target_lrf);

            _hough.setHoughBinSize(binSize.value<float>());
            _hough.setHoughThreshold(threshold.value<float>());
            _hough.setUseInterpolation(true);
            _hough.setUseDistanceWeight(false);

            _hough.setInputCloud(source_keypoints);
            _hough.setInputRf(source_lrf);
            _hough.setSceneCloud(target_keypoints);
            _hough.setSceneRf(target_lrf);
            _hough.setModelSceneCorrespondences(correspondences);

            std::vector<Eigen::Matrix4f, Eigen::aligned_allocator<Eigen::Matrix4f> > trans;
            std::vector<pcl::Correspondences> clustered_corrs;
            _hough.recognize(trans, clustered_corrs);
            for (size_t i = 0; i < trans.size(); ++i)
            {
                transformations.push_back(trans[i]);
            }

            return !trans.empty();
        }

        static ConsoleArgumentCategory argumentCategory;

        static ConsoleArgument binSize;
        static ConsoleArgument threshold;
    private:
        pcl::Hough3DGrouping<PointT, PointT, RFType, RFType> _hough;
        LRFEstimatorT _lrfEstimator;
    };

    template<typename PointT, typename DescriptorT, typename LRFEstimatorT>
    ConsoleArgumentCategory HoughVoting<PointT, DescriptorT, LRFEstimatorT>::argumentCategory(
            "hough", "Transformation estimation using Hough 3D Voting (Correspondence Grouping)");

    template<typename PointT, typename DescriptorT, typename LRFEstimatorT>
    ConsoleArgument HoughVoting<PointT, DescriptorT, LRFEstimatorT>::binSize = ConsoleArgument(
            "hough",
            "bin_size",
            5.0f,
            "Size per bin into the Hough space");

    template<typename PointT, typename DescriptorT, typename LRFEstimatorT>
    ConsoleArgument HoughVoting<PointT, DescriptorT, LRFEstimatorT>::threshold = ConsoleArgument(
            "hough",
            "thresh",
            5.0f,
            "Minimum number of votes in the Hough space needed to infer the presence of a model instance into the scene cloud");
}

#endif // Hough3d_H