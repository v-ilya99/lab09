#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class c_queue
{
public:
    T pop()
    {
        std::unique_lock<std::mutex> mlock(m_mutex);
        while (m_queue.empty())
        {
            m_cond.wait(mlock);
        }
        auto val = m_queue.front();
        m_queue.pop();
        return val;
    }

    void pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(m_mutex);
        while (m_queue.empty())
        {
            m_cond.wait(mlock);
        }
        item = m_queue.front();
        m_queue.pop();
    }

    void push(const T& item)
    {
        std::unique_lock<std::mutex> mlock(m_mutex);
        m_queue.push(item);
        mlock.unlock();
        m_cond.notify_one();
    }
    c_queue()=default;
    c_queue(const c_queue&) = delete;
    c_queue& operator=(const c_queue&) = delete;
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

