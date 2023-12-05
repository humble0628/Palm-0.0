#include <iostream>

class AA
{
private:
    int m_a;
public:
    AA(){std::cout << "AA构造" << std::endl;}
};

class BB
{
private:
    int m_b;
    AA aa;
public:
    BB(){std::cout << "BB构造" << std::endl;}    
};

int main()
{
    bool test = false;
    if (test) 
        std::cout << "111" << std::endl; 
        std::cout << "222" << std::endl;
        std::cout << "333" << std::endl;
        std::cout << "444" << std::endl;
    std::cout << "555" << std::endl;
}