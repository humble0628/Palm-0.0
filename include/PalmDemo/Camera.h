#ifndef _CAMERA_H
#define _CAMERA_H

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

/*
类名：CCamera
入口：无
出口：获取当前帧原始图像 get_frame()
*/
class CCamera
{
private:
    cv::VideoCapture ir_cap;
    cv::VideoCapture rgb_cap;
    cv::Mat ir_frame;
    cv::Mat rgb_frame;

public:
    // cv::VideoCapture& get_capture(std::string id);   // id: {ir, rgb}
    cv::Mat& get_frame(std::string id);                  // 获取图像，调用摄像头，用于子线程
    cv::Mat& get_frame(std::string id, std::string);     // 只获取图像，不调用摄像头，用于主线程
    CCamera();
    ~CCamera();
};

#endif