/*
    1.定义枚举类，枚举出日志等级
    2.提供转换接口：将枚举转换为对应字符串


*/
#ifndef __M_LEVEL_H__
#define __M_LEVEL_H__
namespace bitlog
{
    class LogLevel
    {
        
     public:
        enum class Level
        {
            UNKNOW=0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };
        static const char *toString(LogLevel::Level level)
        {
            switch (level)
            {
            case LogLevel::Level::DEBUG:
                return "DEBUG";
            case LogLevel::Level::INFO:
                return "INFO";
            case LogLevel::Level::WARN:
                return "WARN";
            case LogLevel::Level::ERROR:
                return "ERROR";
            case LogLevel::Level::FATAL:
                return "FATAL";
            case LogLevel::Level::OFF:
                return "OFF";
            default:
                return "UNKNOW";
            }
        }
    };
}
#endif