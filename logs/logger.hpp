/*完成日志器模块
    1.抽象日志基类
    2.派生出不同的子类（同步日志器&异步日志器）

*/
#ifndef __M_LOGGER_H__
#define __M_LOGGER_H__
#include "sink.hpp"
#include "level.hpp"
#include "format.hpp"
#include "util.hpp"
#include <atomic>
#include <mutex>
#include <cstdarg>
namespace bitlog
{
    // 日志器抽象基类：负责接收日志请求，并保存格式器和输出目标。
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>; // 日志器的共享指针类型。
        Logger(const std::string &logger_name, LogLevel::Level limit_level, Formatter::ptr formatter, const std::vector<LogSink::ptr> &sinks)
            : _logger_name(logger_name), _limit_level(limit_level), _formatter(formatter), _sinks(sinks) {
              };
        /*完成构造日志消息对象过程并进行格式化，得到格式化后的日志消息字符串---然后进行落地输出*/
        // 记录 DEBUG 级别日志。
        void debug(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造日志消息对象，进行日志的格式化，最终落地
            // 1.判断当前日志等级是否达到输出等级要求
            if (_limit_level.load() > LogLevel::Level::DEBUG)
            {
                return;
            }

            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            //  处理日志消息
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            va_end(ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!\n,日志消息格式化失败" << std::endl;
                return;
            }
            serialize(LogLevel::Level::DEBUG, file, line, fmt, res);
            // 释放内存
            free(res);
        }
        // 记录 INFO 级别日志。
        void info(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造日志消息对象，进行日志的格式化，最终落地
            // 1.判断当前日志等级是否达到输出等级要求
            if (_limit_level.load() > LogLevel::Level::INFO)
            {
                return;
            }

            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            //  处理日志消息
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            va_end(ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!\n,日志消息格式化失败" << std::endl;
                return;
            }
            serialize(LogLevel::Level::INFO, file, line, fmt, res);
            // 释放内存
            free(res);
        }
        // 记录 WARN 级别日志。
        void warn(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造日志消息对象，进行日志的格式化，最终落地
            // 1.判断当前日志等级是否达到输出等级要求
            if (_limit_level.load() > LogLevel::Level::WARN)
            {
                return;
            }

            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            //  处理日志消息
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            va_end(ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!\n,日志消息格式化失败" << std::endl;
                return;
            }
            serialize(LogLevel::Level::WARN, file, line, fmt, res);
            // 释放内存
            free(res);
        }
        // 记录 ERROR 级别日志。
        void error(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造日志消息对象，进行日志的格式化，最终落地
            // 1.判断当前日志等级是否达到输出等级要求
            if (_limit_level.load() > LogLevel::Level::ERROR)
            {
                return;
            }

            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            //  处理日志消息
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            va_end(ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!\n,日志消息格式化失败" << std::endl;
                return;
            }
            serialize(LogLevel::Level::ERROR, file, line, fmt, res);
            // 释放内存
            free(res);
        }
        // 记录 FATAL 级别日志。
        void fatal(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造日志消息对象，进行日志的格式化，最终落地
            // 1.判断当前日志等级是否达到输出等级要求
            if (_limit_level.load() > LogLevel::Level::FATAL)
            {
                return;
            }

            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            //  处理日志消息
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            va_end(ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!\n,日志消息格式化失败" << std::endl;
                return;
            }
            serialize(LogLevel::Level::FATAL, file, line, fmt, res);
            // 释放内存
            free(res);
        }

    protected:
        void serialize(LogLevel::Level level, const std::string &file, size_t line, const std::string &fmt, char *str)
        {
            // 3.构造日志消息LogMsg对象
            LogMsg msg(level, line, file, _logger_name, str);
            // 4.通过格式化工具对LogMsg对象进行格式化，得到格式化后的日志消息字符串
            std::stringstream ss;
            _formatter->format(ss, msg);
            // 5.进行日志落地输出
            log(ss.str().c_str(), ss.str().size());
        }
        // 将已经格式化的日志数据交给具体日志器处理。
        virtual void log(const char *date, size_t size) = 0;

    protected:
        std::mutex _mutex;                         // 保护同步写日志过程的互斥锁。
        std::string _logger_name;                  // 当前日志器名称。
        std::atomic<LogLevel::Level> _limit_level; // 最低输出等级。
        Formatter::ptr _formatter;                 // 日志格式化器。
        std::vector<LogSink::ptr> _sinks;          // 日志需要写入的所有目标。
    };
    // 同步日志器：在调用线程中直接完成日志输出。
    class SyncLogger : public Logger
    {
    public:
        using ptr = std::shared_ptr<SyncLogger>; // 同步日志器的共享指针类型。
        SyncLogger(const std::string &logger_name, LogLevel::Level limit_level, Formatter::ptr formatter, const std::vector<LogSink::ptr> &sinks)
            : Logger(logger_name, limit_level, formatter, sinks) {
              };

    private:
        // 同步日志器，是将日志直接通过落地模块句柄进行日志落地输出
        //  将一条日志依次写入所有落地器。
        void log(const char *date, size_t size) override
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_sinks.empty())
            {
                return;
            }
            for (auto &sink : _sinks)
            {
                sink->log(date, size);
            }
        }
    };

    enum class LoggerType
    {
        LOGGER_SYNC,  // 同步日志器
        LOGGER_ASYNC, // 异步日志器
    };
    /*使用建造者模式来建造日志器，而不是让用户直接去构造日志器，简化用户的使用复杂度*/
    // 1.抽象一个日志器建造者类（完成日志器对象所需零部件的构造&日志器的构建）
    //  1.1。设置日志器类型
    //  1.2.将不同日志器的创建放在同一个日志器建造者类中完成
    class LoggerBuilder
    {
    public:
        using ptr = std::shared_ptr<LoggerBuilder>; // 日志器建造者的共享指针类型。
        LoggerBuilder():
            _logger_type(LoggerType::LOGGER_SYNC),
            _limit_level(LogLevel::Level::DEBUG)
            {}
        ~LoggerBuilder() = default;
        // 设置日志器类型
        void buildLoggerType(LoggerType logger_type)
        {
            _logger_type = logger_type;
        }
        // 设置日志器名称
        void buildLoggerName(const std::string &logger_name)
        {
            _logger_name = logger_name;
        }
        // 设置日志器的最低输出等级
        void buildLimitLevel(LogLevel::Level limit_level)
        {
            _limit_level = limit_level;
        }
        // 设置日志器的格式化规则
        void buildFormatter(const std::string &pattern)
        {
            _formatter = std::make_shared<Formatter>(pattern);
        }
        // 设置日志器的落地目标
        template <typename SinkType, typename... Args>
        void buildSinks(Args &&...args)
        {
            LogSink::ptr psink = SinkFactory<SinkType>::create(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }
        // 构建日志器对象
        virtual Logger::ptr build() = 0;

    protected:
        LoggerType _logger_type;            // 日志器类型
        std::string _logger_name;         // 日志器名称
        LogLevel::Level _limit_level;     // 日志器最低输出等级
        Formatter::ptr _formatter;        // 日志器格式化规则
        std::vector<LogSink::ptr> _sinks; // 日志器落地目标
    };
    // 2.派生出具体的建造者类---局部日志器的建造者 & 全局日志器的建造者(后面添加了全局单例管理器之后，将日志器添加到全局单例管理器中，方便全局获取日志器)
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(_logger_name.empty() == false && "日志器名称不能为空");
            if(_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if(_sinks.empty())
            {
                buildSinks<StdoutSink>();
            }
            if(_logger_type == LoggerType::LOGGER_SYNC)
            {
                return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
            }
            else
            {
                // 目前只实现了同步日志器，异步日志器暂未实现
                assert(false && "异步日志器暂未实现");
                return nullptr;
            }
        }
    };
}

#endif