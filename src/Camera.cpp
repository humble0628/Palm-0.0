#include "Camera.h"

CCamera::CCamera()
{
    ir_cap.open(2);
    if (!ir_cap.isOpened())
    {
        std::cerr << "\nIR摄像头打开失败,请检查" << std::endl;
        exit(-1);   // 程序终止状态码为 -1
    }
	ir_cap.set(cv::CAP_PROP_FPS,30);
	ir_cap.set(cv::CAP_PROP_CONTRAST, 45);
	ir_cap.set(cv::CAP_PROP_GAMMA, 90);
	ir_cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'V', '2'));
	ir_cap.set(cv::CAP_PROP_FRAME_WIDTH,640);
	ir_cap.set(cv::CAP_PROP_FRAME_HEIGHT,480);
    ir_cap >> ir_frame;     // 读取一帧图像 用作初始化 ir_frame
    cv::transpose(ir_frame, ir_frame);
    cv::flip(ir_frame, ir_frame, 0);

    rgb_cap.open(0);
    if (!ir_cap.isOpened())
    {
        std::cerr << "\nRGB摄像头打开失败,请检查" << std::endl;
        exit(-1);   // 程序终止状态码为 -1
    }
    rgb_cap.set(cv::CAP_PROP_FPS,30);
	rgb_cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	rgb_cap.set(cv::CAP_PROP_FRAME_WIDTH,640);
	rgb_cap.set(cv::CAP_PROP_FRAME_HEIGHT,480);
    rgb_cap >> rgb_frame;   // 读取一帧图像 用作初始化 rgb_frame
    cv::transpose(rgb_frame, rgb_frame);
    cv::flip(rgb_frame, rgb_frame, 0);

    std::cout << "\n摄像头初始化完成" << std::endl;
}

CCamera::~CCamera()
{
    // 释放摄像头
    ir_cap.release(); rgb_cap.release();
    cv::destroyAllWindows();
}

// 获取 Mat 对象 frame, 得到一帧图像
cv::Mat& CCamera::get_frame(std::string id)
{
    if (id == "ir")
    {
        if (!ir_cap.isOpened())
        {
            std::cerr << "\nIR摄像头未打开,请检查" << std::endl;
            exit(-1);   // 程序终止状态码为 -1
        }
        if (ir_frame.empty())
        {
            std::cerr << "\nIR摄像头无图像" << std::endl;
            exit(-2);   // 程序终止状态码为 -2
        }
        ir_cap >> ir_frame;
        cv::transpose(ir_frame, ir_frame);
        cv::flip(ir_frame, ir_frame, 0);
        return ir_frame;
    }
    else if (id == "rgb")
    {
        if (!rgb_cap.isOpened())
        {
            std::cerr << "\nRGB摄像头未打开,请检查" << std::endl;
            exit(-1);   // 程序终止状态码为 -1
        }
        if (rgb_frame.empty())
        {
            std::cerr << "\nRGB摄像头无图像" << std::endl;
            exit(-2);   // 程序终止状态码为 -2
        }
        rgb_cap >> rgb_frame;
        cv::transpose(rgb_frame, rgb_frame);
        cv::flip(rgb_frame, rgb_frame, 0);
        return rgb_frame;
    }
    else
    {
        std::cerr << "\nid输入错误" << std::endl;
        exit(-3);   // 程序终止状态码为 -3
    }
}

cv::Mat& CCamera::get_frame(std::string id, std::string)
{
    if (id == "ir")
    {
        return ir_frame;
    }
    else if (id == "rgb")
    {
        return rgb_frame;
    }
    else
    {
        std::cerr << "\nid输入错误" << std::endl;
        exit(-3);   // 程序终止状态码为 -3
    }
}