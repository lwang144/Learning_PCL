#include "Registration.h"
#include "Filtering.h"
#include "Function_Registration.cpp"
#include "Function_Filtering.cpp"

int main(int argc, char** argv){
    Filters<pcl::PointXYZ> filter;
    Registration<pcl::PointXYZ> registration;
    pcl::PCLPointCloud2::Ptr cloud_source(new pcl::PCLPointCloud2());
    pcl::PCLPointCloud2::Ptr cloud_target(new pcl::PCLPointCloud2());
    pcl::PCDReader fileReader;
    fileReader.read("../../Test_data/data_1/0000000000.pcd", *cloud_source);
    fileReader.read("../../Test_data/data_1/0000000001.pcd", *cloud_target);
    if(cloud_source == NULL || cloud_target == NULL){ 
        std::cout << "Source pcd file read err" << std::endl; return -1;}

    // Down Sampling
    std::cout << "...Downsampling..." << std::endl;
    auto cloud_source_down = filter.VoxelGridDownSampling(cloud_source, 0.1f);
    auto cloud_target_down = filter.VoxelGridDownSampling(cloud_target, 0.1f);
    
    // Estimate normals
    std::cout << "...Estimate normals..." << std::endl;
    auto cloud_source_normal = registration.Normal_Estimation(cloud_source_down, 30);
    auto cloud_target_normal = registration.Normal_Estimation(cloud_target_down, 30);

    // ICP:
    // pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_ICP(new pcl::PointCloud<pcl::PointXYZ>);
    auto cloud_ICP = registration.ICP_Point2Plane(cloud_source_normal, cloud_target_normal, 100);
    
    
    // Visualization 
    // pcl::visualization::PCLVisualizer viewer("ICP TEST");
    // viewer.setBackgroundColor(255, 255, 255); // Set black background
    // viewer.initCameraParameters();
    // pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud_icp_color_h(cloud_ICP, 180, 20, 20);
    // viewer.addPointCloud(cloud_ICP, cloud_icp_color_h, "cloud_icp");
    // viewer.addText("ICP result", 10, 15, 16, 0.0, 0.0, 0.0, "cloud_icp");
    // while(!viewer.wasStopped()){
    //     viewer.spinOnce(100);
    //     boost::this_thread::sleep(boost::posix_time::microseconds(100000));
    // }
    return 0;
}