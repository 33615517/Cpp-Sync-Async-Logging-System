#include <iostream>
//饿汉模式：以空间换时间
// class Singleton
// {

// private:
//     static Singleton _eton;
//     Singleton() : _data(99) {
//         std::cout << "单例对象构造!" << std::endl;
//     }
//      Singleton(const Singleton&) = delete;
//     ~Singleton() {}
// private:
//     int _data;
// public:
//     static Singleton &getInstance()
//     {
//         return _eton;
//     }
//     int getDate()
//     {
//         return _data;
//     }

// };
// Singleton Singleton::_eton;

/*懒汉模式：懒加载--延迟加载的思想---一个对象在第一次使用时再进行实例化*/

class Singleton
{
private:
    Singleton() : _data(99) {
        std::cout << "单例对象构造!" << std::endl;
    }
     Singleton(const Singleton&) = delete;
    ~Singleton() {}
    private: 
        int _data;
    public:
    static Singleton &getInstance()
    {
        static Singleton _eton;
        return _eton;
    }
    int getData()
    {
        return _data;
    }
};
int main()
{

   std::cout << Singleton::getInstance().getData() << std::endl;

    return 0;
}