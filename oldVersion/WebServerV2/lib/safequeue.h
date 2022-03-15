#ifndef __SAFEQUEUE_H__
#define __SAFEQUEUE_H__
#include <mutex>
#include <queue>
template <typename T>
class Safequeue
{
public:
    Safequeue() = default;
    ~Safequeue() = default;

    bool empty();
    int size();
    void enqueue(T &t);
    bool dequeue(T &t);
private:
    std::queue<T> m_queue;
    std::mutex m_mtx;
};

#endif