#ifndef SpinImage_H
#define SpinImage_H

#include <pcl/features/spin_image.h>

#include "../types.h"
#include "../consoleargument.h"
#include "../featuredescription.hpp"

namespace PoseEstimation
{
    /**
     * @brief Feature Description using Spin Images (SI).
     */
    template<typename PointT>
    class SpinImageFeatureDescriptor : public FeatureDescriptor<PointT, pcl::Histogram<153> >
    {
    public:
        typedef pcl::Histogram<153> DescriptorType;

        /**
         * @brief Number of bins along one dimension. Changing this value
         * affects the size of the feature descriptor.
         */
        static const int Resolution = 8;

        SpinImageFeatureDescriptor()
        {
            _si.setImageWidth(Resolution);
        }

        virtual void describe(PC<PointT> &pc,
                              const typename pcl::PointCloud<PointT>::Ptr &,
                              pcl::PointCloud<DescriptorType>::Ptr &descriptors)
        {
            _si.setInputCloud(pc.cloud());
            _si.setRadiusSearch(pc.resolution() * searchRadius.value<float>());

            Logger::tic("SI Feature Extraction");
            _si.compute(*descriptors);
            Logger::toc("SI Feature Extraction");
        }

        static ConsoleArgumentCategory argumentCategory;

        static ConsoleArgument searchRadius;
        static ConsoleArgument minRadius;
        static ConsoleArgument lrfRadius;
        static ConsoleArgument densityRadius;

    private:
        pcl::SpinImageEstimation<PointT, DescriptorType, RFType> _si;
    };

    template<typename PointT>
    ConsoleArgumentCategory SpinImageFeatureDescriptor<PointT>::argumentCategory(
            "SI", "Feature Description using Spin Images (SI)";

    template<typename PointT>
    ConsoleArgument SpinImageFeatureDescriptor<PointT>::searchRadius = ConsoleArgument(
            "SI", "search_r", (float)5.0f, "Search radius for finding neighbors, must be larger than pc_normal_nn");
}

#endif // SpinImage_H