/*实现异步工作器*/
#ifndef __M_LOOPER_H__
#define __M_LOOPER_H__

#include "buffer.hpp"
#include <condition_variable>
#include <thread>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>
namespace bitlog
{
    using Functor = std::function<void(Buffer &)>; // 定义函数对象类型，表示异步工作器的工作函数。
    enum class AsyncType
    {
        ASYNC_SAFE,  // 安全的异步工作器，表示缓冲区满了则阻塞，避免资源耗尽的风险
        ASYNC_UNSAFE // 不考虑资源耗尽的风险，缓冲区满了则无限扩容，适用于极限测试
    };
    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;
        AsyncLooper(const Functor &cb,
                    AsyncType loop_type = AsyncType::ASYNC_SAFE)
            : _callBack(cb),
              _async_type(loop_type),
              _stop(false),
              _thread(&AsyncLooper::threadEntry, this)
        {
        }
        ~AsyncLooper()
        {
            stop();
        }
        void stop()
        {
            _stop = true;
            _cond_pro.notify_all();
            _cond_con.notify_all();
            if (_thread.joinable())
            {
                _thread.join();
            }
        }
        void push(const char *data, size_t len)
        {
            // 1.无限扩容-非安全 2.固定大小-生产缓冲区中满了就阻塞
            std::unique_lock<std::mutex> lock(_mutex);
            // 条件变量空值，若缓冲区剩余空间大小大于数据长度，则直接写入数据；否则阻塞等待，直到缓冲区有足够的空间可写入数据。
            if (_async_type == AsyncType::ASYNC_SAFE)
                _cond_pro.wait(lock, [this, len]
                               { return _pro_buf.writeAbleSize() >= len || _stop; });
            if (_stop)
            {
                return;
            }
            // 能够走下来，说明缓冲区有足够的空间可写入数据
            _pro_buf.push(data, len);
            // 唤醒消费者线程，通知其缓冲区中有数据可读。
            _cond_con.notify_one();
        }

    private:
        // 线程入口函数--对消费者缓冲区中数据进行处理，处理完毕后，初始化缓冲区，交换缓冲区
        void threadEntry()
        {
            while (true)
            {
                // 为互斥锁设置一个生命周期，当缓冲区交换完毕后就解锁（并不对数据的处理过程加锁保护）
                {

                    // 1.判断生产缓冲区有没有数据，有则交换，无则阻塞
                    std::unique_lock<std::mutex> lock(_mutex);
                    _cond_con.wait(lock, [this]
                                   { return !_pro_buf.empty() || _stop; });
                    if (_stop && _pro_buf.empty())
                    {
                        break;
                    }

                    _pro_buf.swap(_con_buf);
                    // 2.唤醒生产者线程，通知其缓冲区中有足够的空间可写入数据。
                    _cond_pro.notify_all();
                }
                // 交换缓冲区
                // 3.被唤醒后，对消费者缓冲区中的数据进行处理
                _callBack(_con_buf);

                // 4.初始化消费缓冲区
                // 5.唤醒生产者线程，通知其缓冲区中有足够的空间可写入数据。
                _con_buf.reset();
            }
        }

    private:
        Functor _callBack; // 具体对缓冲区数据进行处理的回调函数，由异步工作器的使用者提供。
    private:
        AsyncType _async_type; // 异步工作器的类型，安全的异步工作器还是不安全的异步工作器。
        std::atomic<bool> _stop;
        Buffer _pro_buf;                   // 生产者缓冲区
        Buffer _con_buf;                   // 消费者缓冲区
        std::mutex _mutex;                 // 互斥锁
        std::condition_variable _cond_pro; //
        std::condition_variable _cond_con; //
        std::thread _thread;               // 异步工作器对应的工作线程
    };
}

#endif