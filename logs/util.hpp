#ifndef _M_UTIL_H_
#define _M_UTIL_H_

/*实用工具类的时间：
    1.获取系统时间
    2.判断文件是否存在
    3.获取文件所在路径
    4。创建目录
*/
#include <sys/stat.h>
#include <ctime>
#include <string>
#include <iostream>
#include <unistd.h>
namespace bitlog
{
    namespace util
    {
        class Date
        {
        public:
            static size_t getTime()
            {
                return (size_t)time(nullptr);
            }
        };
        class File
        {
        public:
            static bool exists(const std::string &pathname)
            {
                // return access(pathname.c_str(), F_OK) == 0;
                struct stat st;
                if (stat(pathname.c_str(), &st) == 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            static std::string path(const std::string &pathname)
            {
                size_t pos = pathname.find_last_of("/\\");
                if (pos == std::string::npos)
                {
                    return ".";
                }
                else
                {
                    return pathname.substr(0, pos + 1);
                }
            }
            static void createDirectory(const std::string &pathname)
            {
                size_t pos = 0, idx = 0;
                while (idx < pathname.size())
                {
                    pos = pathname.find_first_of("/\\", idx);
                    if (pos == std::string::npos)
                    {
                        mkdir(pathname.c_str(), 0777);
                    }
                    std::string parent_dir = pathname.substr(0, pos + 1);
                    // if(parent_dir == "." || parent_dir == "..")
                    // {
                    //     idx = pos + 1;
                    //     continue;
                    // }
                    if (exists(parent_dir) == true)
                    {
                        idx = pos + 1;
                        continue;
                    }
                    mkdir(parent_dir.c_str(), 0777);
                    idx = pos + 1;

                }
            }
        };
    }
}
#endif