# C++ 同步/异步日志系统

这是一个使用 C++11 编写的轻量级日志系统，支持同步日志、异步日志、日志等级过滤、自定义输出格式、多种日志落地方式以及全局日志器管理。

## 主要功能

- 支持 `DEBUG`、`INFO`、`WARN`、`ERROR`、`FATAL` 等日志等级。
- 支持同步日志器和异步日志器。
- 异步模式使用双缓冲区和独立后台线程批量落地日志。
- 支持安全异步模式和允许自动扩容的非安全异步模式。
- 支持标准输出、普通文件和按文件大小滚动输出。
- 支持自定义日志格式。
- 使用建造者模式创建日志器。
- 使用单例管理器按名称管理全局日志器。
- 提供按时间滚动日志文件的扩展示例和简单性能测试。

## 项目结构

```text
LOG/
├── logs/               日志系统核心代码
│   ├── bitlog.h        对外头文件和日志宏
│   ├── logger.hpp      同步/异步日志器、建造者和日志器管理器
│   ├── looper.hpp      异步工作线程和双缓冲调度
│   ├── buffer.hpp      异步日志缓冲区
│   ├── message.hpp     日志消息结构
│   ├── format.hpp      日志格式化模块
│   ├── sink.hpp        日志落地模块
│   ├── level.hpp       日志等级定义
│   └── util.hpp        时间、文件和目录工具
├── example/            基本使用示例
├── extend/             按时间滚动文件的扩展示例
├── bench/              同步/异步性能测试
└── practice/           项目中使用到的设计模式练习
```

## 环境要求

- Linux 或其他兼容 POSIX 接口的系统
- 支持 C++11 的 GCC/G++
- pthread 线程库

项目使用了 `vasprintf`、`localtime_r`、`mkdir` 等 Linux/POSIX 接口。

## 快速开始

只需要包含 `logs/bitlog.h`。下面创建一个异步日志器，同时将日志输出到终端和文件：

```cpp
#include "logs/bitlog.h"

int main()
{
    std::unique_ptr<bitlog::LoggerBuilder> builder(
        new bitlog::GlobalLoggerBuilder());

    builder->buildLoggerName("app_logger");
    builder->buildLoggerType(bitlog::LoggerType::LOGGER_ASYNC);
    builder->buildLimitLevel(bitlog::LogLevel::Level::DEBUG);
    builder->buildFormatter(
        "[%d{%Y-%m-%d %H:%M:%S}][%c][%f:%l][%p]%T%m%n");

    builder->buildSinks<bitlog::StdoutSink>();
    builder->buildSinks<bitlog::FileSink>("./logfile/app.log");

    bitlog::Logger::ptr logger = builder->build();

    logger->debug("用户编号：%d", 1001);
    logger->info("程序启动成功");
    logger->warn("这是一条警告日志");
    return 0;
}
```

使用全局日志器时，也可以通过名称获取：

```cpp
auto logger = bitlog::getLogger("app_logger");
if (logger)
{
    logger->info("通过名称获取日志器");
}
```

## 同步与异步模式

同步日志器会在调用线程中直接完成日志落地：

```cpp
builder->buildLoggerType(bitlog::LoggerType::LOGGER_SYNC);
```

异步日志器会先把格式化后的日志写入缓冲区，再由后台线程完成落地：

```cpp
builder->buildLoggerType(bitlog::LoggerType::LOGGER_ASYNC);
```

异步日志器默认使用安全模式，生产缓冲区空间不足时会等待。性能测试时可以启用允许缓冲区持续扩容的模式：

```cpp
builder->buildEnableUnSafeAsync();
```

非安全模式可能占用较多内存，更适合 benchmark，不建议在没有内存限制措施的生产环境中使用。

## 日志落地方式

### 输出到终端

```cpp
builder->buildSinks<bitlog::StdoutSink>();
```

### 输出到普通文件

```cpp
builder->buildSinks<bitlog::FileSink>("./logfile/app.log");
```

### 按文件大小滚动

下面表示单个文件达到约 1 MB 后切换到新文件：

```cpp
builder->buildSinks<bitlog::RollBySizeSink>(
    "./logfile/app-", 1024 * 1024);
```

一个日志器可以同时添加多个落地目标。

## 格式化规则

示例格式：

```text
[%d{%Y-%m-%d %H:%M:%S}][%c][%f:%l][%p]%T%m%n
```

| 标记 | 含义 |
| --- | --- |
| `%d{格式}` | 日志时间，内部格式遵循 `strftime` |
| `%t` | 当前实现输出源码行号 |
| `%c` | 日志器名称 |
| `%f` | 源文件名称 |
| `%l` | 源代码行号 |
| `%p` | 日志等级 |
| `%T` | 制表符 |
| `%m` | 日志正文 |
| `%n` | 换行符 |
| `%%` | 普通百分号 |

## 默认 root 日志器

系统会自动创建名为 `root` 的同步日志器。下面这些大写宏始终使用 root 日志器：

```cpp
DEBUG("debug message");
INFO("info message");
WARN("warn message");
ERROR("error message");
FATAL("fatal message");
```

如果希望使用自己创建的日志器及其等级、格式器和落地目标，应通过日志器指针调用：

```cpp
logger->info("message");
```

## 编译运行

运行基本示例：

```bash
cd example
make
./test
```

运行按时间滚动文件的扩展示例：

```bash
cd extend
make
./test
```

运行性能测试：

```bash
cd bench
make
./bench
```

清理对应目录中的可执行文件：

```bash
make clean
```

手动编译时建议使用：

```bash
g++ -std=c++11 -pthread test.cc -o test
```

## 性能测试说明

`bench/bench.cc` 中提供了：

- `sync_bench()`：测试同步文件日志器。
- `async_bench()`：测试异步文件日志器。
- `bench_log()`：创建指定数量的线程并统计日志提交耗时。

可以在 `main()` 中选择需要执行的测试。异步测试统计的主要是前台线程提交日志的耗时，后台文件落地可能仍在继续，因此同步和异步结果代表的工作范围并不完全相同。

## 扩展日志落地器

自定义落地器只需要继承 `bitlog::LogSink` 并实现 `log()`：

```cpp
class CustomSink : public bitlog::LogSink
{
public:
    void log(const char *data, size_t size) override
    {
        // 将 data 指向的 size 个字节写入自定义目标
    }
};
```

然后通过建造者添加：

```cpp
builder->buildSinks<CustomSink>();
```

`extend/test.cc` 中的 `RollByTimeFileSink` 展示了按时间滚动日志文件的完整实现。

## 使用注意

- 全局日志器名称应保持唯一。
- `getLogger()` 找不到名称时会返回空指针，使用前应进行判断。
- 文件落地器会自动创建目标文件所在的目录。
- 异步日志器销毁时会等待后台线程结束，并处理缓冲区中的剩余日志。
