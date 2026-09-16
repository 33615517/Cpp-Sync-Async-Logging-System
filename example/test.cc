#include "../logs/bitlog.h"
#include<unistd.h>

void test_log(const std::string &logger_name)
{
    DEBUG("%s","测试开始");
    bitlog::Logger::ptr logger = bitlog::LoggerManager::getInstance().getLogger(logger_name);
    logger->debug("%s","测试日志");
    logger->info("%s","测试日志");
    logger->warn("%s","测试日志");
    logger->error("%s","测试日志");
    logger->fatal("%s","测试日志");
    INFO("%s","测试完毕");
}
// 测试日志消息格式化和按秒滚动写文件。
int main()
{
    std::unique_ptr<bitlog::LoggerBuilder> builder(new bitlog::GlobalLoggerBuilder());
    builder->buildLoggerName("async_log");
    builder->buildLimitLevel(bitlog::LogLevel::Level::DEBUG);
    builder->buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%c][%f:%l][%p]%T%m%n");
    builder->buildLoggerType(bitlog::LoggerType::LOGGER_SYNC);
   // builder->buildEnableUnSafeAsync();
    builder->buildSinks<bitlog::StdoutSink>();
    builder->buildSinks<bitlog::FileSink>("./logfile/async_log");
    builder->buildSinks<bitlog::RollBySizeSink>("./logfile/rool-", 1024 * 1024);
    bitlog::Logger::ptr logger = builder->build();
    test_log("async_log");
    return 0;
}
