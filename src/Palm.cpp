#include "Palm.h"

CPalm::CPalm()
{
    // 检查License

    // 摄像头初始化
    m_ir_frame = m_camera.get_frame("ir", "GetFrameOlny").clone();

    // 模型初始化
    m_roi_fail = m_cnet.get_roi_fail();
    m_points = m_cnet.get_points();
    m_roi_img = m_cnet.get_roi_img();

    // 加载数据库
    m_userdata_path = R"(../../res/data.dat)";
    m_match_thres = 0.92;
    userdata_load(m_userdata_path);

    // user变量初始化
    m_user.name = "";
    m_user.feature.resize(128, 0.0);

    m_palm_fail = true;
    m_img_scalar_fail = true;
}

CPalm::~CPalm()
{

}

// 将本地用户数据加载到内存中
// 读取二进制数据有学问，不要直接使用sizeof读取结构体，因为UserData成员都是动态扩展大小
void CPalm::userdata_load(std::string& userdata_path)
{
    std::ifstream fin;

    // 二进制读取
    fin.open(m_userdata_path, std::ios::in | std::ios::binary);
    if (!fin.is_open())
    {
        // 文件不存在的情况
        if (fin.fail() && !fin.bad())
        {
            std::cout << "\n数据文件不存在 已创建" << std::endl;
            std::ofstream createFile(userdata_path);
        }
        // 文件存在但打不开
        else
        {
            std::cout << "\n数据文件无法打开 请检查" << std::endl;
            exit(-1);
        }
    }
    // 先判断文件是否为空
    if (fin.peek() == std::ifstream::traits_type::eof())
    {
        std::cout << "\n数据库为空" << std::endl;
        return;
    }

    // 读取到内存，储存在 m_user_database 中
    while (true)
    {
        UserData user;
        int name_length, feature_size;

        fin.read(reinterpret_cast<char*>(&name_length), sizeof(name_length));

        // 注意位置 EOF在执行read函数后才改变，这行代码必须放这里
        if (fin.eof()) break;

        user.name.resize(name_length);
        fin.read(&user.name[0], name_length);

        fin.read(reinterpret_cast<char*>(&feature_size), sizeof(feature_size));
        user.feature.resize(feature_size);
        fin.read(reinterpret_cast<char*>(user.feature.data()), sizeof(float) * feature_size);

        m_user_database.push_back(user);
    }

    fin.close();
    std::cout << "\n数据库加载完成" << std::endl;
}

// 保存更新后的数据库
// 写入二进制数据有学问，不要直接使用sizeof写入结构体，因为UserData成员都是动态扩展内存
void CPalm::userdata_save(std::string& userdata_path)
{
    std::ofstream fout;

    // 设置清空原文件数据并以二进制写入
    fout.open(m_userdata_path, std::ios::out | std::ios::binary);
    if (!fout.is_open())
    {
        std::cout << "\n数据保存失败" << std::endl;
        exit(1);
    }

    for (const UserData& data : m_user_database)
    {
        // 先写入用户名长度和C风格用户名
        int name_length = data.name.length();
        fout.write(reinterpret_cast<const char*>(&name_length), sizeof(name_length));
        fout.write(data.name.c_str(), name_length);

        // 写入特征向量大小和特征向量
        int feature_size = data.feature.size();
        fout.write(reinterpret_cast<const char*>(&feature_size), sizeof(feature_size));
        fout.write(reinterpret_cast<const char*>(data.feature.data()), sizeof(float) * feature_size);
    }

    fout.close();
}

// 检查输入的用户名是否合法
bool CPalm::username_check(std::string& username)
{
    // 不能为纯数字
    if (std::all_of(username.begin(), username.end(), [](char c){
        return std::isdigit(c);
    }))
    {
        std::cout << "\n用户名不能为纯数字" << std::endl;
        return false;
    }
    
    // 不能过长或过短 
    if (username.size() > 12 || username.size() < 2)
    {
        std::cout << "\n用户名长度不符合要求" << std::endl;
        return false;
    }

    // 不能包含除下划线以外的特殊符号
    if (std::any_of(username.begin(), username.end(), [](char c){
        return !std::isalnum(c) && c != '_';
    }))
    {
        std::cout << "\n用户名不能包含除下划线以外的特殊符号" << std::endl;
        return false;
    }

    // 不能与数据库存在同名注册
    auto it = std::find_if(m_user_database.begin(), m_user_database.end(), [&username](const UserData& data){
        return data.name == username;
    });
    if (it != m_user_database.end())
    {
        std::cout << "\n用户名已存在" << std::endl;
        return false;
    }

    return true;
}

// 使用匹配，返回索引
// XXX: 用户规模很大时，将用户数据加载到内存的方案不可行，应该使用数据库技术
#ifdef USEMAP
    int CPalm::userfeature_match(std::vector<float> feature, float threshold)
    {
        // 数据库为空的情况
        if (m_user_database.size() == 0) return -1;
        
        // 局部变量定义
        int index = -1;
        Eigen::Map<Eigen::VectorXf> e_feature(feature.data(), LEN_FEATURE);
        Eigen::MatrixXf e_database(m_user_database.size(), LEN_FEATURE);

        int row = 0;
        for (auto m_it = m_user_database.begin(); m_it != m_user_database.end(); ++m_it)
        {
            int col = 0;
            const std::vector<float>& v_temp = m_it->second;
            for (auto v_it = v_temp.begin(); v_it != v_temp.end(); ++v_it)
            {
                e_database(row, col) = *v_it;
                ++col;
            }
            ++row;
        }

        // 矩阵乘法计算相似度
        Eigen::VectorXf result = e_feature * e_database.transpose();
        for (int i = 0; i < result.size(); ++i)
        {
            if ((1-result[i]) < threshold)
            {
                index = i;
                break;
            }
        }
        return index;
    }
#else
    int CPalm::userfeature_match(std::vector<float>& feature, float& threshold)
    {
        // 数据库为空的情况
        if (m_user_database.size() == 0) return -1;

        // 将vector转化成eigen向量
        Eigen::Map<Eigen::VectorXf> e_feature(feature.data(), this->LEN_FEATURE);

        // 余弦相似度
        int index = -1;
        std::vector<float> v_result;
        for (auto it = m_user_database.begin(); it != m_user_database.end(); ++it)
        {
            Eigen::Map<Eigen::VectorXf> temp_feature(it->feature.data(), this->LEN_FEATURE);
            float result = static_cast<float>(e_feature.dot(temp_feature) / (e_feature.norm() * temp_feature.norm()));
            v_result.push_back(result);
        }

        // *** 测试用 ***
        // for (auto it = v_result.begin(); it != v_result.end(); ++it)
        // {
        //     std::cout << *it << " ";
        // }
        // std::cout << std::endl;

        // 查找最大元素所在索引
        std::vector<float>::iterator max_it = std::max_element(v_result.begin(), v_result.end());
        if (max_it != v_result.end())
        {
            if (*max_it < m_match_thres) index = -1;
            index = std::distance(v_result.begin(), max_it);
        }
        return index;   // 缺省返回-1，表示查找失败
    }
#endif

// 使一些 过程中一直存在但不断更改的变量 恢复缺省值
void CPalm::members_clear()
{
    m_roi_img.setTo(0);
    m_roi_fail = true;
    m_user.name = "";
    m_user.feature.resize(128, 0.0);
    m_features.clear();
    m_palm_fail = true;
    m_img_scalar_fail = true;
}

//*************** 接口函数 ***************
// 注册用户
void CPalm::user_register()
{
    // 用户名操作
    std::cout << std::endl;
    std::cout << "注意：用户名长度2-12位字符\n"
              << "     字符包含字母、数字、特殊符号\n"
              << "     不能为纯数字\n"
              << "     特殊符号只能包含'_'" << std::endl;
    std::cout << std::endl;

    std::cout << "\n请输入用户名" << std::endl;
    std::cout << ">>";
    std::cin >> m_user.name;
    while (!username_check(m_user.name))
    {
        std::cout << "\n用户名不合法，请重新输入" << std::endl;
        std::cout << ">>";
        std::cin >> m_user.name;
    }

    // roi检测
    int img_count = 0;      // 有效图像数量
    int palm_count = 0;     // 手掌检测次数
    int roi_count = 0;      // roi检测次数

    std::cout << "\n正在检测手掌..." << std::endl;

    // ********** 需要检测图像有效性 **********
    // 检测手掌进入 并等待相机曝光
    while (true)
    {
        // 手掌检测时间过长 退出识别模式
        if (palm_count >= PALM_COUNT)
        {
            std::cout << "\n未检测到手掌或曝光异常" << std::endl;
            return;
        }
        // 检测到手掌且曝光正常退出循环
        if (!m_palm_fail && !m_img_scalar_fail) break;
        m_ir_frame = m_camera.get_frame("ir", "GetFrameOlny");
        
        // 必须先检测到手掌 再计算曝光是否正常
        m_palm_fail = m_cnet.palm_detect(m_ir_frame);

        // ***********  再计算曝光  ************
        // TODO：需要一个 动态测量曝光正常范围 的方法
        if (!m_palm_fail)
        {
            // 休眠0.5秒，等待对焦和曝光
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            cv::Scalar gray_scalar = cv::mean(m_ir_frame);
            if (gray_scalar.val[0] > 95 || gray_scalar.val[0] < 75) m_img_scalar_fail = true;
            else m_img_scalar_fail = false;
        }
        ++palm_count;
    }
    std::cout << "\n检测到手掌，请保持不要动" << std::endl;

    std::cout << "\n开始注册..." << std::endl;

    // roi提取
    while (true)
    {
        // ROI检测时间过长 退出识别模式
        if (roi_count >= ROI_COUNT)
        {
            std::cout << "\n没有采集到有效图像" << std::endl;
            members_clear();
            return;
        }
        // 检测到一定数量的有效roi 退出循环
        if (img_count >= IMG_NUM_RECOGNIZE) break;

        m_ir_frame = m_camera.get_frame("ir", "GetFrameOlny").clone();
        m_roi_fail = m_cnet.roi_detect(m_ir_frame);
        if (!m_roi_fail)
        {
            ++img_count;
            m_roi_img = m_cnet.get_roi_img();

            // 特征提取
            m_features.push_back(m_cnet.get_feature());
        }
        ++roi_count;
    }

    // 有效特征取平均
    for (const auto& feature : m_features)
    {
        for (int i = 0; i < LEN_FEATURE; ++i)
        {
            m_user.feature[i] += feature[i];
        }
    }
    for (int i = 0; i < LEN_FEATURE; ++i)
    {
        m_user.feature[i] /= m_features.size();
    }
    // *** 测试用 ***
    // for (auto it = m_user.feature.begin(); it != m_user.feature.end(); ++it)
    // {
    //     std::cout << *it << " ";
    // }
    // std::cout << std::endl;

    std::cout << "\n用户 " << m_user.name << " 注册成功" << std::endl;

    // 更新数据库
    m_user_database.push_back(m_user);
    userdata_save(m_userdata_path);

    std::cout << "\n数据库已更新" << std::endl;

    // 清空部分成员属性
    members_clear();
}

// 识别用户
void CPalm::user_recognize()
{
    // roi检测
    int img_count = 0;      // 有效图像数量
    int palm_count = 0;     // 手掌检测次数
    int roi_count = 0;      // roi检测次数

    std::cout << "\n正在检测手掌..." << std::endl;

    // ********** 需要检测图像有效性 **********
    // 检测手掌进入 并等待相机曝光
    while (true)
    {
        // 手掌检测时间过长 退出识别模式
        if (palm_count >= PALM_COUNT)
        {
            std::cout << "\n未检测到手掌或曝光异常" << std::endl;
            return;
        }
        // 检测到手掌且曝光正常退出循环
        if (!m_palm_fail && !m_img_scalar_fail) break;
        m_ir_frame = m_camera.get_frame("ir", "GetFrameOlny");
        
        // 必须先检测到手掌 再计算曝光是否正常
        m_palm_fail = m_cnet.palm_detect(m_ir_frame);

        // ***********  再计算曝光  ************
        // TODO：需要一个 动态测量曝光正常范围 的方法
        if (!m_palm_fail)
        {
            // 休眠0.5秒，等待对焦和曝光
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            cv::Scalar gray_scalar = cv::mean(m_ir_frame);
            if (gray_scalar.val[0] > 95 || gray_scalar.val[0] < 75) m_img_scalar_fail = true;
            else m_img_scalar_fail = false;
            // std::cout << gray_scalar.val[0] << std::endl;
        }
        ++palm_count;
    }
    std::cout << "\n检测到手掌，请保持不要动" << std::endl;

    // 开始计时
    auto start = std::chrono::high_resolution_clock::now();

    // roi提取
    while (true)
    {
        // ROI检测时间过长 退出识别模式
        if (roi_count >= ROI_COUNT)
        {
            std::cout << "\n没有采集到有效图像" << std::endl;
            members_clear();
            return;
        }
        // 检测到一定数量的有效roi 退出循环
        if (img_count >= IMG_NUM_RECOGNIZE) break;

        m_ir_frame = m_camera.get_frame("ir", "GetFrameOlny");
        m_roi_fail = m_cnet.roi_detect(m_ir_frame);
        if (!m_roi_fail)
        {
            ++img_count;
            m_roi_img = m_cnet.get_roi_img();

            // 特征提取
            m_features.push_back(m_cnet.get_feature());
        }
        ++roi_count;
    }

    // 有效特征取平均
    for (const auto& feature : m_features)
    {
        for (int i = 0; i < LEN_FEATURE; ++i)
        {
            m_user.feature[i] += feature[i];
        }
    }
    for (int i = 0; i < LEN_FEATURE; ++i)
    {
        m_user.feature[i] /= m_features.size();
    }

    // 特征匹配
    int index = userfeature_match(m_user.feature, m_match_thres);
    auto end = std::chrono::high_resolution_clock::now();
    if (index == -1)
    {
        std::cout << "\n未找到该用户" << std::endl;
        members_clear();
        return;
    }
    std::string temp_name = m_user_database[index].name;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\n识别成功：用户 " << temp_name << "  用时："<< duration.count() << " ms" << std::endl;

    // 清空部分成员属性
    members_clear();
}

// 数据库操作
void CPalm::user_database()
{

}

// 退出程序
void CPalm::user_quit()
{
    // 退出码为0 程序正常退出
    exit(0);
}

// 在每一帧图上画出关键点   用于子线程
const cv::Mat CPalm::get_frame_with_points()
{
    cv::Mat frame = m_camera.get_frame("ir").clone();
    if (!m_roi_fail)
    {
        m_points = m_cnet.get_points();
        m_roi_rect = m_cnet.get_rect();
        cv::circle(frame, m_points[0], 3, cv::Scalar(0, 0, 255), 4);
        cv::circle(frame, m_points[1], 3, cv::Scalar(255, 0, 0), 4);
        cv::circle(frame, m_points[2], 3, cv::Scalar(255, 0, 0), 4);
        cv::polylines(frame, m_roi_rect, true, cv::Scalar(0, 255, 0), 2);
        // *** 测试用 ***
        cv::imwrite(R"(../../res/whole.jpg)", frame);
        cv::imwrite(R"(../../res/roi.jpg)", m_roi_img);
    }
    return frame;
}