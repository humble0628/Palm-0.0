#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <typeinfo>
#include <memory>

int& func(int* p)
{
    int& a = *p;
    std::cout << "局部变量a的地址：" << &a;
    std::cout << std::endl;
    return a;
}

struct UserData
{
public:
    std::string name;      // 用户名
    std::vector<float> feature; // 特征向量
}m_user;

void func_out()
{
    std::string m_userdata_path = R"(./test.dat)";

    UserData user1{"abc", {1.0, 2.0, 3.0}}, user2{"def", {4.0, 5.0, 6.0}};
    std::vector<UserData> m_user_database;

    m_user_database.push_back(user1);
    m_user_database.push_back(user2);

    std::ofstream fout;

    // 设置清空原文件数据并以二进制写入
    fout.open(m_userdata_path, std::ios::out | std::ios::binary);
    if (!fout.is_open()) return;

    for (const UserData& data : m_user_database)
    {
        size_t nameLength = data.name.length();
        fout.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        fout.write(data.name.c_str(), nameLength);

        // 写入特征向量大小和特征向量
        size_t featureSize = data.feature.size();
        fout.write(reinterpret_cast<const char*>(&featureSize), sizeof(featureSize));
        fout.write(reinterpret_cast<const char*>(data.feature.data()), sizeof(float) * featureSize);
    }

    fout.close();
}

void func_in()
{
    std::string m_userdata_path = R"(./test.dat)";
    std::vector<UserData> m_user_database;
    std::ifstream fin;

    // 二进制读取
    fin.open(m_userdata_path, std::ios::binary);
    if (!fin.is_open()) return;

    while (true)
    {
        UserData user;
        size_t nameLength, featureSize;

        fin.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

        if (fin.eof()) break;

        user.name.resize(nameLength);
        fin.read(&user.name[0], nameLength);

        fin.read(reinterpret_cast<char*>(&featureSize), sizeof(featureSize));
        user.feature.resize(featureSize);
        fin.read(reinterpret_cast<char*>(user.feature.data()), sizeof(float) * featureSize);

        m_user_database.push_back(user);

        std::cout << fin.eof() << std::endl;
    }

    fin.close();

    for (auto it = m_user_database.begin(); it != m_user_database.end(); ++it)
    {
        std::cout << it->name << std::endl;
    }    
}

int main()
{
    // func_in();

    // int* a = new int(1);
    // std::unique_ptr<int> p(a);

    std::cout << m_user.name << std::endl;

    return 0;
}