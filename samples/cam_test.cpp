#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture ir_cap;

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

	cv::namedWindow("Video", 1);
	for (;;)
	{
		// 获取新的一帧;
		cv::Mat frame;
		ir_cap >> frame; 
		if (frame.empty())
			return 0;

		// 显示新的帧;
		imshow("Video", frame);
		
		// 按键退出显示;
		if (cv::waitKey(30) >= 0) break;
	}

	// 5.释放视频采集对象;
	ir_cap.release();

    return 0;    
}