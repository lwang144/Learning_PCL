#include "RealTimeTest.h"
/* \author Leo Wang */
// Customized Supporting function for pointcloud processing 
// using PCL

/**
 * Developer: Leo Wang
 * E-mail:    liangyu@student.chalmers.se
 * Date:      02/2020
 */
template<typename PointT>
std::tuple<std::vector<std::string>, int16_t> 
 User<PointT>::loadFile (const std::string &folderPath){
    // Count the total number of files in the path and return the path of all files.
    std::vector<std::string> filePaths; 
    DIR *path;
    struct dirent *ep;
    char path_array[(int) folderPath.length() + 1];
    strcpy(path_array, folderPath.c_str());
    path = opendir(path_array);
    int16_t count = 0;
    if(path != NULL){
        while(ep = readdir(path)){
            if(!ep -> d_name || ep -> d_name[0] == '.')
                continue;
            filePaths.push_back(folderPath + ep -> d_name);
            count ++;
        }
        (void) closedir(path);
        std::sort(filePaths.begin(), filePaths.end());
    }
    else
        perror("Couldn't open the directory...");
    std::cout << "Found " << count << " files in folder [" << folderPath << "]."<< std::endl;
    return std::make_tuple(filePaths, count);
}

template<typename PointT>
void 
 User<PointT>::timerCalculator (const std::chrono::_V2::system_clock::time_point &start_time,
                                const std::string &function){
    // Should use "auto start_fast = std::chrono::system_clock::now()" to start timer.
    auto end_time = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    double time_passed = (double) duration.count() * 
            std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
    std::cout << "[--Timer--] " << function <<" time used: " << time_passed << " [s]." << std::endl;
    //return time_passed; // [seconds]
}

template<typename PointT>
typename pcl::PCLPointCloud2::Ptr
 User<PointT>::loadPCD(const std::vector<std::string> &filePaths, 
                          const int16_t &NUM){
    // Load .pcd file
    pcl::PCDReader fileReader;
    pcl::PCLPointCloud2::Ptr cloud(new pcl::PCLPointCloud2());
    fileReader.read(filePaths[NUM], *cloud);
    std::cout << "Load file: [" << filePaths[NUM] << "]." << std::endl;
    return cloud;
}

template<typename PointT>
void 
 User<PointT>::initCamera (pcl::visualization::PCLVisualizer &viewer,
                           const Color &background_color, 
                           const CameraAngle &camera_angle){
    viewer.setBackgroundColor(background_color.R, background_color.G, background_color.B); // Set black background
    viewer.initCameraParameters();
    const int distance = 30;
    if(camera_angle != FPS)
        viewer.addCoordinateSystem(1.0);
    switch(camera_angle) {
        case TOP:
        viewer.setCameraPosition(0, 0, distance, 1, 0, 1); break;
        case SIDE:
        viewer.setCameraPosition(0, -distance, 0, 0, 0, 1); break;
        case FPS:
        viewer.setCameraPosition(-10, 0, 0, 0, 0, 1); break;
    }
}

template<typename PointT>
void 
 User<PointT>::showPointcloud (pcl::visualization::PCLVisualizer &viewer, 
                               typename pcl::PointCloud<PointT>::Ptr &cloud, 
                               const int &point_size,
                               const Color &color, 
                               const std::string &name){
    //pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud_color_h(cloud, color.R, color.G, color.B);
    viewer.addPointCloud(cloud, name);
    viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, point_size, name);
    viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_COLOR, color.R, color.G, color.B, name);
}