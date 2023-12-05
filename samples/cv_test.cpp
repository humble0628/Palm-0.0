#include <memory>

#include "Camera.h"

cv::Mat func(cv::Mat& frame, std::unique_ptr<CCamera>& cam)
{
    frame = cam->get_frame("ir");
    std::cout << "局部frame地址: " << &frame << std::endl;
    return frame;
}


int main()
{
    std::unique_ptr<CCamera> cam(new CCamera);
    cam->get_frame("ir");

    cv::Mat frame_1, frame_2, frame_3;

    for (int i = 0; i < 1; ++i)
    {
        // frame_1 = cam->get_frame("ir").clone();
        // frame_1 = func(frame_3, cam);
        frame_1 = cam->get_frame("ir");
        if (frame_1.data == cam->ir_frame.data) std::cout << "共享数据" << std::endl;
        std::cout << "frame_1地址: " << &frame_1 << std::endl;
        std::cout << "camera_frame地址: " << &(cam->ir_frame) << std::endl;
        cv::circle(frame_1, cv::Point(100, 200), 3, cv::Scalar(0, 0, 255), 4);
        frame_2 = cam->get_frame("ir", "aaa");

        cv::imwrite(R"(../../res/test/frame_1.jpg)", frame_1);
        cv::imwrite(R"(../../res/test/frame_2.jpg)", frame_2);
    }

    return 0;
}