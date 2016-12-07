/**
 * Copyright (c) 2016, David Stutz
 * Contact: david.stutz@rwth-aachen.de, davidstutz.de
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/integral_image_normal.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/segmentation/supervoxel_clustering.h>
#include "vccs_opencv_pcl.h"

void VCCS_OpenCV_PCL::computeSuperpixels(const cv::Mat &image, const cv::Mat &cloud, 
        float voxel_resolution, float seed_resolution, float spatial_weight, 
        float normal_weight, bool use_transform, cv::Mat &labels) {
    
    // Convert to point cloud:
//    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr point_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>);
//    point_cloud->height = cloud.rows;
//    point_cloud->width = cloud.cols;
//    point_cloud->is_dense = true;
//    point_cloud->resize(point_cloud->height*point_cloud->width);
//    
//    for (int i = 0; i < cloud.rows; ++i) {
//        for (int j = 0; j < cloud.cols; ++j) {
//            pcl::PointXYZRGBA point;
//            
//            // x,y,z coordinates are in meters, but depth is given in meters*1000.
//            point.x = cloud.at<cv::Vec3f>(i, j)[0];
//            point.y = cloud.at<cv::Vec3f>(i, j)[1];
//            point.z = cloud.at<cv::Vec3f>(i, j)[2];
//            
//            point.r = image.at<cv::Vec3b>(i, j)[2];
//            point.g = image.at<cv::Vec3b>(i, j)[1];
//            point.b = image.at<cv::Vec3b>(i, j)[0];
//            
//            // Note that row and column are switched here.
//            (*point_cloud)(j, i) = point;
//        }
//    }

    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr point_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>);
    for (int i = 0; i < cloud.rows; ++i) {
        for (int j = 0; j < cloud.cols; ++j) {
            pcl::PointXYZRGBA point;

            // x,y,z coordinates are in meters, but depth is given in meters*1000.
            point.x = cloud.at<cv::Vec3f>(i, j)[0];
            point.y = cloud.at<cv::Vec3f>(i, j)[1];
            point.z = cloud.at<cv::Vec3f>(i, j)[2];

            point.r = image.at<cv::Vec3b>(i, j)[2];
            point.g = image.at<cv::Vec3b>(i, j)[1];
            point.b = image.at<cv::Vec3b>(i, j)[0];

            // Note that row and column are switched here.
            point_cloud->push_back(point);
        }
    }
    
    pcl::SupervoxelClustering<pcl::PointXYZRGBA> super(voxel_resolution, seed_resolution, 
            use_transform);
    super.setInputCloud(point_cloud);
    
    float color_weight = 1.f - spatial_weight - normal_weight;
    super.setColorImportance(color_weight);
    super.setSpatialImportance(spatial_weight);
    super.setNormalImportance(normal_weight);
    
    std::map<uint32_t, pcl::Supervoxel<pcl::PointXYZRGBA>::Ptr> supervoxel_clusters;
    super.extract(supervoxel_clusters);

    int index = 0;
    pcl::PointCloud<pcl::PointXYZL>::Ptr label_cloud = super.getLabeledCloud();
    pcl::PointXYZL label;

        labels.create(cloud.rows, cloud.cols, CV_32SC1);
        for (int i = 0; i < image.rows; ++i) {
            for (int j = 0; j < image.cols; ++j) {
//                if (depth.at<unsigned short>(i, j) > 0) {
                    label = (*label_cloud).at(index);
//                    label = (*label_cloud)(j, i);
                    labels.at<int>(i, j) = label.label;
                    index++;
                    
//                    if (labels[i][j] > maxLabel) {
//                        maxLabel = labels[i][j];
//                    }
//                }
            }
        }

//z    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer ("3D Viewer"));
//    viewer->setBackgroundColor(0, 0, 0);
//    viewer->addPointCloud(point_cloud, "point cloud");
//
//    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr voxel_centroid_cloud = super.getVoxelCentroidCloud();
//    viewer->addPointCloud(voxel_centroid_cloud, "voxel centroids");
//    viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,2.0, "voxel centroids");
//    viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY,0.95, "voxel centroids");
//
//    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr colored_voxel_cloud = super.getColoredCloud();
//    viewer->addPointCloud(colored_voxel_cloud, "colored voxels");
//    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_OPACITY,0.8, "colored voxels");
//
//    while (!viewer->wasStopped()) {
//        viewer->spinOnce(100);
//    }
}