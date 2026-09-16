#include "../logs/duallog.h"
#include<unistd.h>

// 获取指定的全局日志器，并依次输出各个等级的测试日志。
void test_log(const std::string &logger_name)
{
    DUALLOG_DEBUG("%s","测试开始");
    duallog::Logger::ptr logger = duallog::LoggerManager::getInstance().getLogger(logger_name);
    logger->debug("%s","测试日志");
    logger->info("%s","测试日志");
    logger->warn("%s","测试日志");
    logger->error("%s","测试日志");
    logger->fatal("%s","测试日志");
    DUALLOG_INFO("%s","测试完毕");
}
// 测试日志消息格式化和按秒滚动写文件。
int main()
{
    std::unique_ptr<duallog::LoggerBuilder> builder(new duallog::GlobalLoggerBuilder());
    builder->buildLoggerName("async_log");
    builder->buildLimitLevel(duallog::LogLevel::Level::DEBUG);
    builder->buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%c][%f:%l][%p]%T%m%n");
    builder->buildLoggerType(duallog::LoggerType::LOGGER_SYNC);
   // builder->buildEnableUnSafeAsync();
    builder->buildSinks<duallog::StdoutSink>();
    builder->buildSinks<duallog::FileSink>("./logfile/async_log");
    builder->buildSinks<duallog::RollBySizeSink>("./logfile/rool-", 1024 * 1024);
    duallog::Logger::ptr logger = builder->build();
    test_log("async_log");
    return 0;
}
