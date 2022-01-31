#include "../lib/safequeue.h"

template<typename T>
bool Safequeue<T>::empty()
{
    std::unique_lock<std::mutex> lk(m_mtx);
    return m_queue.empty();
}
template<typename T>
int Safequeue<T>::size()
{
    std::unique_lock<std::mutex> lk(m_mtx);
    return m_queue.size();
}
template<typename T>
void Safequeue<T>::enqueue(T &t)
{
    std::unique_lock<std::mutex> lk(m_mtx);
    m_queue.push(t);
}
template<typename T>
bool Safequeue<T>::dequeue(T &t)
{
    std::unique_lock<std::mutex> lk(m_mtx);
    if(m_queue.empty())
    {
        return false;
    }
    t = std::move(m_queue.front());
    m_queue.pop();
    return true;
}
    