#include "../logs/bitlog.h"
#include <unistd.h>
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>
// 使用指定日志器启动多个线程，并统计日志提交速度。
void bench_log(const std::string &logger_name, size_t thr_count, size_t msg_count, size_t msg_size)
{
    // 1.获取日志器
    bitlog::Logger::ptr logger = bitlog::LoggerManager::getInstance().getLogger(logger_name);
    std::cout << "测试日志：" << msg_count << "条，总大小：" << (msg_count * msg_size) << "字节(KB)，线程数量：" << thr_count << "，日志数量：" << msg_count << "，日志大小：" << msg_size << std::endl;
    // 2.组织指定长度的日志消息
    std::string msg(msg_size - 1, 'a'); // 少一个字节是为了给字符串结尾的'\0'留出空间
    // 3.创建指定数量的线程
    std::vector<std::thread> threads;
    std::vector<double> costs(thr_count, 0.0);
    size_t msg_prt_thr = msg_count / thr_count; // 总日志数量/线程数量=每个线程的日志数量
    for (size_t i = 0; i < thr_count; i++)
    {
        threads.emplace_back([&, i, msg_count]()
                             {
            //4.线程函数内部开始计时
            auto start = std::chrono::steady_clock::now();
            for(size_t j = 0; j < msg_prt_thr; j++)
            {
                // logger->debug("%s", msg.c_str());
                // logger->info("%s", msg.c_str());
                // logger->warn("%s", msg.c_str());
                // logger->error("%s", msg.c_str());
                logger->fatal("%s", msg.c_str());
            }
            //5.线程函数内部结束计时
            auto end = std::chrono::steady_clock::now();
            //6.以秒为单位统计并输出每个线程的耗时
            std::chrono::duration<double> duration = end - start;
            costs[i] = duration.count();
            std::cout << "Thread" << i <<": " << std::this_thread::get_id() << "\t输出日志数量：" << msg_prt_thr <<"，耗时：" << duration.count() << "秒" << std::endl; });
    }
    // 7.等待所有线程完成
    for (auto &t : threads)
    {
        t.join();
    }
    // 8.计算总耗时：在多线程中每个线程都会消耗时间，但是线程是并发处理的，所以总耗时应该是所有线程耗时的平均值，而不是简单的累加。
    double max_cost = costs[0];
    for (double c : costs)
    {
        max_cost = std::max(max_cost, c);
    }
    double msg_per_sec = msg_count / max_cost;
    double size_per_sec = msg_per_sec * msg_size / 1024.0;
    // 9.进行输出打印
    std::cout << "总日志数量：" << msg_count << "，总耗时：" << max_cost << "秒，平均每秒输出日志数量：" << msg_per_sec << "条/秒，平均每秒输出日志大小：" << size_per_sec << "KB/秒" << std::endl;
}
// 创建同步文件日志器并执行性能测试。
void sync_bench()
{
    std::unique_ptr<bitlog::LoggerBuilder> builder(
        new bitlog::GlobalLoggerBuilder()
    );

    builder->buildLoggerName("sync_logger");
    builder->buildLimitLevel(bitlog::LogLevel::Level::DEBUG);
    builder->buildFormatter(
        "[%d{%Y-%m-%d %H:%M:%S}][%c][%f:%l][%p]%T%m%n"
    );
    builder->buildLoggerType(bitlog::LoggerType::LOGGER_SYNC);
    builder->buildSinks<bitlog::FileSink>("./logfile/sync.log");

    bitlog::Logger::ptr logger = builder->build();

    //bench_log("sync_logger", 1, 2000000, 100);
    bench_log("sync_logger", 3, 2000000, 100);
}
// 创建异步文件日志器并执行性能测试。
void async_bench()
{
     std::unique_ptr<bitlog::LoggerBuilder> builder(
        new bitlog::GlobalLoggerBuilder()
    );

    builder->buildLoggerName("async_logger");
    builder->buildLimitLevel(bitlog::LogLevel::Level::DEBUG);
    builder->buildFormatter(
        "[%d{%Y-%m-%d %H:%M:%S}][%c][%f:%l][%p]%T%m%n"
    );
    builder->buildLoggerType(bitlog::LoggerType::LOGGER_ASYNC);
   // builder->buildEnableUnSafeAsync();// 开启不安全的异步模式----主要是为了将实际落地时间和日志输出时间分离，避免落地慢导致日志输出慢的情况
    builder->buildSinks<bitlog::FileSink>("./logfile/async.log");

    bitlog::Logger::ptr logger = builder->build();

    //bench_log("sync_logger", 1, 2000000, 100);
    bench_log("async_logger", 3, 2000000, 100);
}
int main()
{
    sync_bench();
    //async_bench();
    return 0;
}
