#ifndef _PALM_H
#define _PALM_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

#include "AESCoder.h"
#include "License.h"
#include "Camera.h"
#include "Net.h"

// 用户data
struct UserData
{
public:
    std::string name;      // 用户名
    std::vector<float> feature; // 特征向量
};

/*
类名：CPalm
入口：CLicense类 激活标志位
     CCamera类 ir_frame
     CNet类特征向量 roi标志位
出口：注册
     识别
     数据库操作
     退出
     绘图
*/
class CPalm
{
private:
    // Camera接口变量
    CCamera m_camera;                               // Camera对象
    cv::Mat m_ir_frame;                             // ir原始图像

    // Net接口变量
    CNet m_cnet;                                    // CNet对象
    cv::Mat m_roi_img;                              // roi图像
    bool m_roi_fail;                                // roi获取失败标志位 true代表获取失败
    std::vector<cv::Point> m_points;                // 关键点
    std::vector<cv::Point> m_roi_rect;              // roi矩形四个顶点

    // Palm自有成员属性
    static const int IMG_NUM_REGISTER = 5;          // 注册需要的有效图像数量
    static const int IMG_NUM_RECOGNIZE = 2;         // 识别需要的有效图像数量
    static const int PALM_COUNT = 2000;             // 手掌检测轮数
    static const int ROI_COUNT = 1000;              // roi检测轮数
    static const int LEN_FEATURE = 128;             // 特征向量长度   C++11对静态常量的声明方式，不需要在类外初始化
    
    std::string m_userdata_path;                    // 数据库保存路径
    float m_match_thres;                            // 匹配阈值

    bool m_palm_fail;                               // 检测手掌标志位
    bool m_img_scalar_fail;                         // 图像平均曝光 即进行手掌检测时图像的曝光
    
    UserData m_user;                                // 包含用户名与特征向量
    std::vector<std::vector<float>> m_features;     // 储存有效的特征

    //std::map<std::string, std::vector<float>> m_user_database; // 数据库储存变量(使用map容器)
    std::vector<UserData> m_user_database;          // 数据库储存变量(使用结构体)

private:
    void userdata_load(std::string& userdata_path);
    void userdata_save(std::string& userdata_path);
    bool username_check(std::string& username);
    int userfeature_match(std::vector<float>& feature, float& threshold);
    bool assess_img(cv::Mat& img);
    void members_clear();

public:
    void user_register();
    void user_recognize();
    void user_database();
    void user_quit();
    const cv::Mat get_frame_with_points();

    CPalm();
    ~CPalm();
};

#endif