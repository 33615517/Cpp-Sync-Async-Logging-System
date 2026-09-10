#include"util.hpp"
#include "level.hpp"
using namespace bitlog;
using namespace bitlog::util;
int main()
{
    std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::DEBUG)<<std::endl;
    std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::INFO)<<std::endl;
    std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::WARN)<<std::endl;
    std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::ERROR)<<std::endl;
    std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::FATAL)<<std::endl;
    std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::OFF)<<std::endl;



    // std::cout<<bitlog::util::Date::getTime()<<std::endl;
    // std::string pathname ="./abc/bcd/a.txt";
    // bitlog::util::File::createDirectory(bitlog::util::File::path(pathname));


    return 0;
}