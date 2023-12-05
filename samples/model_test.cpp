#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Net.h"

void draw_points(cv::Mat& frame, std::vector<cv::Point>& points)
{
    cv::circle(frame, points[0], 3, cv::Scalar(0, 0, 255), 4);
    cv::circle(frame, points[1], 3, cv::Scalar(0, 255, 0), 4);
    cv::circle(frame, points[2], 3, cv::Scalar(0, 255, 0), 4);
}

int main()
{
    cv::VideoCapture ir_cap, rgb_cap;
    CNet net;

    ir_cap.open(2);

    if (!ir_cap.isOpened())
    {
        std::cerr << "IR摄像头打开失败,请检查" << std::endl;
        exit(-1);   // 程序终止状态码为 -1
    }

	ir_cap.set(cv::CAP_PROP_FPS,30);
	ir_cap.set(cv::CAP_PROP_CONTRAST, 45);
	ir_cap.set(cv::CAP_PROP_GAMMA, 100);
	ir_cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'V', '2'));
	ir_cap.set(cv::CAP_PROP_FRAME_WIDTH,640);
	ir_cap.set(cv::CAP_PROP_FRAME_HEIGHT,480);

    rgb_cap.open(0);
    if (!ir_cap.isOpened())
    {
        std::cerr << "RGB摄像头打开失败,请检查" << std::endl;
        exit(-1);   // 程序终止状态码为 -1
    }
    rgb_cap.set(cv::CAP_PROP_FPS,30);
	rgb_cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	rgb_cap.set(cv::CAP_PROP_FRAME_WIDTH,640);
	rgb_cap.set(cv::CAP_PROP_FRAME_HEIGHT,480);

	cv::namedWindow("Video", 1);
	for (;;)
	{
		// 获取新的一帧;
		cv::Mat frame, frame_rgb;
        std::vector<cv::Point> points;
		ir_cap >> frame; rgb_cap >> frame_rgb;
		if (frame.empty())
			return 0;
	
        net.roi_detect(frame);
        if (!net.get_roi_fail())
        {
            points = net.get_points();
            draw_points(frame, points);
            net.get_feature();
        }
		imshow("Video", frame);

		// 按键退出显示;
		if (cv::waitKey(30) >= 0) break;
	}

	// 5.释放视频采集对象;
	ir_cap.release();
    rgb_cap.release();

    return 0;
}