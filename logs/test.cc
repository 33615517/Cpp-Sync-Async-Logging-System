#include "util.hpp"
#include "level.hpp"
using namespace bitlog;
using namespace bitlog::util;
#include "format.hpp"
#include "sink.hpp"
#include "logger.hpp"
#include "buffer.hpp"
/*扩展一个以时间作为入职文件滚动切换类型的日志落地模块
    1.以时间进行文件滚动，实际上是以时间段进行滚动
        实现思想：以当前系统时间，取模时间段大小，可以得到当前时间段是第几个时间段
            每次以当前系统时间取模，判断与当前文件的时间段是否一致，不一致代表不是同一个时间段
        time(nullptr)%60 当前就是第n个60秒
        time(nullptr)%3600 当前就是第n个3600秒
        time(nullptr)%86400 当前就是第n个86400秒

*/
// 时间滚动周期。
enum class TimeGap
{
    GAP_SECOND, // 每秒切换一个日志文件。
    GAP_MINUTE, // 每分钟切换一个日志文件。
    GAP_HOUR,   // 每小时切换一个日志文件。
    GAP_DAY     // 每天切换一个日志文件。
};
// 测试用时间滚动落地器：进入新的时间段后切换日志文件。
class RollByTimeFileSink : public bitlog::LogSink
{
public:
    // 设置文件名前缀和滚动周期，并打开当前时间段的日志文件。
    RollByTimeFileSink(const std::string &filename, TimeGap gap) : _basename(filename)
    {
        switch (gap)
        {
        case TimeGap::GAP_SECOND:
            _gap_size = 1;
            break;
        case TimeGap::GAP_MINUTE:
            _gap_size = 60;
            break;
        case TimeGap::GAP_HOUR:
            _gap_size = 3600;
            break;
        case TimeGap::GAP_DAY:
            _gap_size = 86400;
            break;
        }
        _cur_gap = util::Date::now() / _gap_size;  // 获取当前是第几时间段
        std::string newfilename = createNewFile(); // 当前时间段对应的文件名。
        bitlog::util::File::createDirectory(bitlog::util::File::path(newfilename));
        _ofs.open(newfilename, std::ios::binary | std::ios::app);
        assert(_ofs.is_open() && "日志文件打开失败，请检查文件路径是否正确");
    }
    // 判断当前时间是否为当前文件的时间段，如果不是则创建新文件
    void log(const char *date, size_t size) override
    {
        time_t t = util::Date::now();   // 本次写日志时的时间戳。
        size_t cur_gap = t / _gap_size; // 当前时间所属的时间段编号。
        if (cur_gap != _cur_gap)
        {
            // 1.关闭当前文件
            _ofs.close();
            // 2.创建新文件
            std::string newfilename = createNewFile();
            bitlog::util::File::createDirectory(bitlog::util::File::path(newfilename));
            // 3.打开新文件
            _ofs.open(newfilename, std::ios::binary | std::ios::app);
            assert(_ofs.is_open() && "日志文件打开失败，请检查文件路径是否正确");
            // 4.重置当前文件时间段
            _cur_gap = cur_gap;
        }
        _ofs.write(date, size);
        assert(_ofs.good() && "日志文件写入失败，请检查文件路径是否正确");
    }

private:
    // 使用文件名前缀和当前时间生成新日志文件名。
    std::string createNewFile()
    {
        // 获取系统时间，以时间来构造文件名扩展名
        time_t t = util::Date::now(); // 当前 Unix 时间戳。
        struct tm lt;                 // 当前本地时间的各个字段。
        localtime_r(&t, &lt);

        std::string newfilename = _basename + std::to_string(lt.tm_year + 1900) + "-" + std::to_string(lt.tm_mon + 1) + "-" + std::to_string(lt.tm_mday) + "-" + std::to_string(lt.tm_hour) + "-" + std::to_string(lt.tm_min) + "-" + std::to_string(lt.tm_sec) + ".log"; // 拼接得到完整文件名。

        return newfilename;
    }

private:
    std::string _basename; // 滚动日志文件的路径和名称前缀。
    std::ofstream _ofs;    // 当前日志文件的输出流。
    size_t _cur_gap;       // 当前是第几个时间段
    size_t _gap_size;      // 时间段的大小
};
// 测试日志消息格式化和按秒滚动写文件。
int main()
{
    // 读取文件数据，一点一点写入缓冲区，最终将缓冲区数据写入文件，判断生成的新文件与源文件是否一致
    std::ifstream ifs("./logfile/test.log", std::ios::binary);
    if (!ifs.is_open())
    {
        std::cerr << "Failed to open test.log" << std::endl;
        return -1;
    }
    ifs.seekg(0, std::ios::end);
    size_t file_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::string body;
    body.resize(file_size);
    ifs.read(&body[0], file_size);
    if (!ifs.good())
    {
        std::cerr << "Failed to read test.log" << std::endl;
        return -1;
    }

    ifs.close();
    bitlog::Buffer buffer;
    for (int i = 0; i < body.size(); i++)
    {
        buffer.push(&body[i], 1);
    }
    std::ofstream ofs("./logfile/test_copy.log", std::ios::binary);
    while (!buffer.empty())
    {
        ofs.write(buffer.begin(), 1);
        buffer.moveReader(1);
    }
    // ofs.write(buffer.begin(), buffer.readAbleSize());
    ofs.close();

    // std::string logger_name = "sync_logger";
    // bitlog::LogLevel::Level limit = bitlog::LogLevel::Level::WARN;
    // bitlog::Formatter::ptr fmt(new bitlog::Formatter("[%d{%H:%M:%S}][%c][%f:%l][%p]%T%m%n"));
    // bitlog::LogSink::ptr stdout_lsp = bitlog::SinkFactory<bitlog::StdoutSink>::create();
    // bitlog::LogSink::ptr file_lsp = bitlog::SinkFactory<bitlog::FileSink>::create("./logfile/test.log");
    // bitlog::LogSink::ptr roll_lsp = bitlog::SinkFactory<bitlog::RollBySizeSink>::create("./logfile/rool-", 1024 * 1024);
    // std::vector<bitlog::LogSink::ptr> sinks = {stdout_lsp, file_lsp, roll_lsp};
    // bitlog::Logger::ptr logger(new bitlog::SyncLogger(logger_name, limit, fmt, sinks));

    // std::unique_ptr<bitlog::LoggerBuilder> builder(new bitlog::LocalLoggerBuilder());
    // builder->buildLoggerName("sync_logger");
    // builder->buildLimitLevel(bitlog::LogLevel::Level::WARN);
    // builder->buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%c][%f:%l][%p]%T%m%n");
    // builder->buildLoggerType(bitlog::LoggerType::LOGGER_SYNC);
    // builder->buildSinks<bitlog::StdoutSink>();
    // builder->buildSinks<bitlog::FileSink>("./logfile/test.log");
    // builder->buildSinks<bitlog::RollBySizeSink>("./logfile/rool-", 1024 * 1024);
    // bitlog::Logger::ptr logger = builder->build();

    // logger->debug(__FILE__,__LINE__,"%s","测试日志");
    // logger->info(__FILE__,__LINE__,"%s","测试日志");
    // logger->warn(__FILE__,__LINE__,"%s","测试日志");
    // logger->error(__FILE__,__LINE__,"%s","测试日志");
    // logger->fatal(__FILE__,__LINE__,"%s","测试日志");
    // size_t cursize = 0, count = 0;
    // std::string str= "测试日志-";
    // while (cursize < 1024 * 1024 * 10)
    // {
    //     logger->fatal(__FILE__,__LINE__,"测试日志-%d",count++);
    //     cursize+=20;
    // }
    // bitlog::LogMsg msg(bitlog::LogLevel::Level::DEBUG, 10, "test.cc", "root", "this is a test log"); // 测试日志消息。
    // bitlog::Formatter format;              // 使用默认格式规则的格式化器。
    // std::string str = format.format(msg);  // 已经格式化、可以直接写出的日志文本。
    // // bitlog::LogSink::ptr stdout_lsp = bitlog::SinkFactory<bitlog::StdoutSink>::create();
    // // bitlog::LogSink::ptr file_lsp = bitlog::SinkFactory<bitlog::FileSink>::create("./logfile/test.log");
    // // bitlog::LogSink::ptr roll_lsp = bitlog::SinkFactory<bitlog::RollBySizeSink>::create("./logfile/rool-", 1024 * 1024);

    // bitlog::LogSink::ptr roll_time_lsp = bitlog::SinkFactory<RollByTimeFileSink>::create("./logfile/rolltime-", TimeGap::GAP_SECOND); // 按秒滚动的日志落地器。
    // time_t old = bitlog::util::Date::now(); // 测试开始时间。
    // while (bitlog::util::Date::now() < old + 5)
    // {
    //     roll_time_lsp->log(str.c_str(), str.size());
    // }
    // stdout_lsp->log(str.c_str(), str.size());
    // file_lsp->log(str.c_str(), str.size());
    // size_t cursize = 0;
    // size_t count = 0;
    // while (cursize < 1024 * 1024 * 10)
    // {
    //     std::string tmp = str + std::to_string(count++);
    //     roll_lsp->log(tmp.c_str(), tmp.size());
    //     cursize += tmp.size();
    // }

    // std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::DEBUG)<<std::endl;
    // std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::INFO)<<std::endl;
    // std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::WARN)<<std::endl;
    // std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::ERROR)<<std::endl;
    // std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::FATAL)<<std::endl;
    // std::cout<<bitlog::LogLevel::toString(bitlog::LogLevel::Level::OFF)<<std::endl;
    // std::cout<<bitlog::util::Date::getTime()<<std::endl;
    // std::string pathname ="./abc/bcd/a.txt";
    // bitlog::util::File::createDirectory(bitlog::util::File::path(pathname));

    return 0;
}
