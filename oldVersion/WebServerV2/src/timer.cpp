#include "../lib/timer.h"

TimeManager::TimeManager(int heapNum) : m_heap(heapNum) {}
TimeManager::~TimeManager() {}

// 私有成员函数
void TimeManager::m_del(int i)
{
    // 删除指定小根堆中指定的节点
    if (i < 0 || i >= m_heap.size())
    {
        std::cout << "location:timer.cpp:m_del,"
                  << "error:invalid i"
                  << std::endl;
        return;
    }
    int j = i;
    int n = m_heap.size() - 1;
    if (j < n)
    {
        swapNode(j, n);
        if (!sink(j, n))
        {
            swim(j);
        }
    }
    /* 删除队尾元素 */
    m_ref.erase(m_heap[n].m_id);
    m_heap.pop_back();
}
void TimeManager::swim(int i)
{
    if (i < 0 || i >= m_heap.size())
    {
        std::cout << "location:timer.cpp:swim,"
                  << "error:invalid i"
                  << std::endl;
        return;
    }
    int j = (i - 1) / 2;
    while (j >= 0)
    {
        if (m_heap[j] < m_heap[i])
        {
            break;
        }
        swapNode(j, i);
        i = j;
        j = (i - 1) / 2;
    }
}
bool TimeManager::sink(int i, int n)
{
    if (i < 0 || i >= m_heap.size())
    {
        std::cout << "location:timer.cpp:sink,"
                  << "error:invalid i"
                  << std::endl;
        return false;
    }
    if (n < 0 || n > m_heap.size())
    {
        std::cout << "location:timer.cpp:sink,"
                  << "error:invalid n"
                  << std::endl;
        return false;
    }
    int j = i;
    int k = j * 2 + 1;
    while (k < n)
    {
        if (k + 1 < n && m_heap[k + 1] < m_heap[k])
        {
            k++;
        }
        if (m_heap[j] < m_heap[k])
        {
            break;
        }
        swapNode(j, k);
        j = k;
        k = j * 2 + 1;
    }
    return j > i;
}
void TimeManager::swapNode(int i, int j)
{
    Timenode tmp = std::move(m_heap[i]);
    m_heap[i] = std::move(m_heap[j]);
    m_heap[j] = std::move(tmp);
}

// 公有成员函数
void TimeManager::addTimer(int id, int timeout, const _timeoutCallBack &cb)
{
    if (id < 0)
    {
        std::cout << "location:timer.cpp:addTimer,"
                  << "error:invalid id"
                  << std::endl;
        return;
    }
    int i;
    if (m_ref.count(id) == 0)
    {
        // new node
        i = m_heap.size(); // i == next valid pos in the m_heap
        m_ref[id] = i;
        m_heap.push_back({id, _clock::now() + _ms(timeout), cb});
        /* 更新小根堆 */
        swim(i);
    }
    else
    {
        i = m_ref[id];
        m_heap[i].m_expire = _clock::now() + _ms(timeout);
        m_heap[i].m_cb = cb;
        if (!sink(i, m_heap.size()))
        {
            swim(i);
        }
    }
}
void TimeManager::handleExpiredEvent()
{
    if(m_heap.empty())
    {
        return;
    }
    while(!m_heap.empty())
    {
        Timenode node = m_heap.front();
        if(std::chrono::duration_cast<_ms>(node.m_expire - _clock::now()).count() > 0)
        {
            /*
             timeout是预先设定的socket可以持续的时间
             每一个socket的时间等于当前时间+预设时间
             */
            break;
        }
        node.m_cb();
        pop();
    }
}
int TimeManager::getNextHandle()
{
    // 先去小根堆看一下有没有过期的socket通信。如果有，关闭该socket通信
    handleExpiredEvent();
    int res = -1;
    if(!m_heap.empty())
    {
        // 获取小根堆中第一个还没有过期的结点的时间，单位是us
        res = std::chrono::duration_cast<_ms>(m_heap.front().m_expire - _clock::now()).count();
        if(res < 0){res = 0;}
    }
    return res;
}
void TimeManager::updateTimer(int id, int timeout)
{
    if (m_heap.empty() || m_ref.count(id) == 0)
    {
        std::cout << "location:timer.cpp:work,"
                  << "error:invalid id or empty heap"
                  << std::endl;
        return;
    }
    int i = m_ref[id];
    m_heap[i].m_expire = _clock::now() + _ms(timeout);
    if(!sink(i, m_heap.size()))
    {
        swim(i);
    }
}
// 删除指定id节点，并且用指针触发处理函数
void TimeManager::work(int id)
{
    /* 删除指定id结点，并触发回调函数 */
    if (m_heap.empty() || m_ref.count(id) == 0)
    {
        std::cout << "location:timer.cpp:work,"
                  << "error:invalid id or empty heap"
                  << std::endl;
        return;
    }
    int i = m_ref[id];
    Timenode node = std::move(m_heap[i]);
    node.m_cb();
    m_del(i);
}

void TimeManager::pop()
{
    if(m_heap.empty())
    {
        std::cout << "location:timer.cpp:pop,"
                  << "error:empty heap"
                  << std::endl;
        return;
    }
    m_del(0);
}
void TimeManager::clear()
{
    m_heap.clear();
    m_ref.clear();
}