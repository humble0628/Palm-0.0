#ifndef _NET_H
#define _NET_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <typeinfo>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include "AESCoder.h"

#define PI 3.14159265

/*
类名：CNet
入口：CCamera类 ir_frame
     AESCoder类 解密方法
出口：获取roi图以及标志位
     获取特征向量
     获取关键点
*/
class CNet
{
private:
    cv::Mat m_roi_img;                  // roi图像
    bool m_roi_fail;                    // roi获取失败标志位 true代表获取失败
    std::vector<float> m_feature;       // 特征向量
    std::vector<cv::Point> m_points;    // 关键点坐标 两个手指和手掌的中心坐标
    std::vector<cv::Point> m_roi_rect;  // roi矩形四个顶点

    float m_conf_thres;                 // 置信度阈值
    float m_nms_thres;                  // 非极大抑制阈值

    int m_init_img_size;                // 原始图像大小
    int m_roi_img_size;                 // roi图像大小

    cv::Mat m_blob_roi;                 // roi模型输入                 
    std::vector<cv::Mat> m_outs;        // roi模型输出
    cv::Mat m_blob_feature;             // feature模型输入

    cv::dnn::Net m_roi_net, m_feature_net;
    std::string m_roi_define_path, m_roi_model_path, m_feature_model_path;
    std::vector<uchar> m_roi_define_vec, m_roi_net_vec, m_feature_net_vec;

private:
    bool decrypt_model(std::string model_path, std::vector<uchar>& model_vec);
    void read_model();
    std::vector<cv::String> get_outlayer_names(cv::dnn::Net net);
    void get_yolo_output(cv::Mat& ir_frame);
    void extract_roi(cv::Mat& ir_frame, float coordinate[3][2]);
    std::vector<float> normalize(const std::vector<float>& code);
    cv::Mat& add_channels(cv::Mat& img);

public:
    bool palm_detect(cv::Mat& ir_frame);
    bool roi_detect(cv::Mat& ir_frame);
    std::vector<float> get_feature();
    const cv::Mat& get_roi_img() const;
    const bool& get_roi_fail() const;
    const std::vector<cv::Point>& get_points() const;
    const std::vector<cv::Point>& get_rect() const;
    CNet();
    ~CNet();
};

#endif