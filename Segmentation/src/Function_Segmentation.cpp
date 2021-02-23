#include "Segmentation.h"
/* \author Leo Wang */
// Customized Filtering function for pointcloud processing 
// using PCL

/**
 * Developer: Leo Wang
 * E-mail:    liangyu@student.chalmers.se
 * Date:      02/2020
 */

template<typename PointT>
std::tuple<typename pcl::PointCloud<PointT>::Ptr, typename pcl::PointCloud<PointT>::Ptr> 
 Segmentation<PointT>::PlaneSegmentation(const typename pcl::PointCloud<PointT>::Ptr &cloud, 
                                         const int &maxIterations, 
                                         const float &distanceThreshold) {
    pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
    pcl::PointIndices::Ptr inliers (new pcl::PointIndices); // defeine plane inliers
    pcl::SACSegmentation<PointT> seg;
    seg.setOptimizeCoefficients (true);
    seg.setModelType (pcl::SACMODEL_PLANE); // Set plane model
    seg.setMethodType (pcl::SAC_RANSAC); // Method: RANSAC
    seg.setMaxIterations(maxIterations);
    seg.setDistanceThreshold (distanceThreshold); // [meter]
    seg.setInputCloud (cloud); // Input
    seg.segment (*inliers, *coefficients);
    if (inliers->indices.size () == 0)
        PCL_ERROR ("Could not estimate a planar model for the given dataset.");
    typename pcl::PointCloud<PointT>::Ptr cloud_plane(new pcl::PointCloud<PointT>());
    typename pcl::PointCloud<PointT>::Ptr cloud_other(new pcl::PointCloud<PointT>());
    // Copy inliers point cloud as plane
    for (int index : inliers -> indices) {
        cloud_plane -> points.push_back(cloud->points[index]);
    }
    pcl::ExtractIndices<PointT> EI;
    // Extract the inliers so that we can get other point cloud
    EI.setInputCloud(cloud);
    EI.setIndices(inliers);
    EI.setNegative(true);
    EI.filter(*cloud_other);
    std::cout << "Plane points: " << cloud_plane->points.size() << ", other points: " 
              << cloud_other->points.size() << std::endl;
    return std::make_tuple(cloud_plane, cloud_other);
 }

template<typename PointT>
std::tuple<typename pcl::PointCloud<PointT>::Ptr, typename pcl::PointCloud<PointT>::Ptr> 
 Segmentation<PointT>::ProgressiveMorphologicalSegmentation(const typename pcl::PointCloud<PointT>::Ptr &cloud, 
                                                            const int &MaxWindowSize, 
                                                            const float &Slope, 
                                                            const float &InitialDistance, 
                                                            const float &MaxDistance){
    typename pcl::PointCloud<PointT>::Ptr cloud_plane(new pcl::PointCloud<PointT>());
    typename pcl::PointCloud<PointT>::Ptr cloud_other(new pcl::PointCloud<PointT>());
    pcl::PointIndicesPtr ground(new pcl::PointIndices);
    typename pcl::ProgressiveMorphologicalFilter<PointT> pmf;
    pmf.setInputCloud(cloud);
    pmf.setMaxWindowSize(MaxWindowSize);
    pmf.setSlope(Slope);
    pmf.setInitialDistance(InitialDistance);
    pmf.setMaxDistance(MaxDistance);
    pmf.extract(ground->indices);
    // Extract pointcloud
    pcl::ExtractIndices<PointT> extract;
    extract.setInputCloud(cloud);
    extract.setIndices(ground);
    extract.filter(*cloud_plane);
    extract.setNegative(true);
    extract.filter(*cloud_other);
    std::cout << "Plane points: " << cloud_plane->points.size() << ", other points: " 
              << cloud_other->points.size() << std::endl;
    return std::make_tuple(cloud_plane, cloud_other);
 }

template<typename PointT>
std::vector<typename pcl::PointCloud<PointT>::Ptr> 
 Segmentation<PointT>::EuclideanClustering( const typename pcl::PointCloud<PointT>::Ptr &cloud, 
                                            const float &ClusterTolerance,
                                            const int &MinSize, 
                                            const int &MaxSize){
    std::vector<typename pcl::PointCloud<PointT>::Ptr> clusters;
    std::vector<pcl::PointIndices> cluster_indices;
    // Creating KdTree object for the search method of extraction.
    typename pcl::search::KdTree<PointT>::Ptr kdtree(new pcl::search::KdTree<PointT>());
    kdtree->setInputCloud(cloud);
    pcl::EuclideanClusterExtraction<PointT> EC;
    EC.setClusterTolerance(ClusterTolerance);
    EC.setMinClusterSize(MinSize);
    EC.setMaxClusterSize(MaxSize);
    EC.setSearchMethod(kdtree);
    EC.setInputCloud(cloud); // Input PointCloud
    EC.extract(cluster_indices);
    // Iteratively visit Pointcloud indexs 'cluster_indices', until all the clusters are splited
    for (std::vector<pcl::PointIndices>::const_iterator it=cluster_indices.begin(); it!=cluster_indices.end(); ++ it){
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster (new pcl::PointCloud<pcl::PointXYZ>);
        for (std::vector<int>::const_iterator pit=it->indices.begin(); pit!=it->indices.end(); ++ pit)
            cloud_cluster -> points.push_back(cloud->points[*pit]);
        cloud_cluster -> width = cloud_cluster -> points.size ();
        cloud_cluster -> height = 1;
        cloud_cluster -> is_dense = true;
        if (cloud_cluster->width >= MinSize && cloud_cluster->width <= MaxSize)
            clusters.push_back(cloud_cluster);
    }
    return clusters;
}   