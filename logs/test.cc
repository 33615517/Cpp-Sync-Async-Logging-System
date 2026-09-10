#include"util.hpp"
using namespace bitlog;
using namespace bitlog::util;
int main()
{
    std::cout<<bitlog::util::Date::getTime()<<std::endl;
    std::string pathname ="./abc/bcd/a.txt";
    bitlog::util::File::createDirectory(bitlog::util::File::path(pathname));


    return 0;
}