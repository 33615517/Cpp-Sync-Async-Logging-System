#include "../logs/duallog.h"
#include<unistd.h>
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
class RollByTimeFileSink : public duallog::LogSink
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
        _cur_gap = duallog::util::Date::now() / _gap_size;  // 获取当前是第几时间段
        std::string newfilename = createNewFile(); // 当前时间段对应的文件名。
        duallog::util::File::createDirectory(duallog::util::File::path(newfilename));
        _ofs.open(newfilename, std::ios::binary | std::ios::app);
        assert(_ofs.is_open() && "日志文件打开失败，请检查文件路径是否正确");
    }
    // 判断当前时间是否为当前文件的时间段，如果不是则创建新文件
    void log(const char *date, size_t size) override
    {
        time_t t = duallog::util::Date::now();   // 本次写日志时的时间戳。
        size_t cur_gap = t / _gap_size; // 当前时间所属的时间段编号。
        if (cur_gap != _cur_gap)
        {
            // 1.关闭当前文件
            _ofs.close();
            // 2.创建新文件
            std::string newfilename = createNewFile();
            duallog::util::File::createDirectory(duallog::util::File::path(newfilename));
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
        time_t t = duallog::util::Date::now(); // 当前 Unix 时间戳。
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
int main()
{
    std::unique_ptr<duallog::LoggerBuilder> builder(new duallog::GlobalLoggerBuilder());
    builder->buildLoggerName("async_log");
    builder->buildLimitLevel(duallog::LogLevel::Level::WARN);
    builder->buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%c][%f:%l][%p]%T%m%n");
    builder->buildLoggerType(duallog::LoggerType::LOGGER_ASYNC);
   // builder->buildEnableUnSafeAsync();
    builder->buildSinks<duallog::StdoutSink>();
    builder->buildSinks<duallog::FileSink>("./logfile/async_log");
    builder->buildSinks<RollByTimeFileSink>("./logfile/rool-", TimeGap::GAP_SECOND);
    duallog::Logger::ptr logger = builder->build();
    DUALLOG_DEBUG("%s","测试日志");
    DUALLOG_INFO("%s","测试日志");
    DUALLOG_WARN("%s","测试日志");
    DUALLOG_ERROR("%s","测试日志");
    DUALLOG_FATAL("%s","测试日志");
    size_t cur = duallog::util::Date::now();
    while (duallog::util::Date::now() - cur < 10)
    {
        logger->fatal("测试日志-%zu",duallog::util::Date::now());
        usleep(1000);
    }
    
    return 0;
}
