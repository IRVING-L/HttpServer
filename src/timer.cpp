#include "timer.h"
#include "log.h"
TimeManager::TimeManager() {}
TimeManager::~TimeManager() {}

// 私有成员函数
void TimeManager::m_del(int i)
{
    // 删除指定小根堆中指定的节点
    // std::cout << "TimeManager::m_del(int i)" << i  << std::endl;
    if (i < 0 || i > static_cast<int>(m_heap.size()))
    {
        LOG_DEBUG("invalid i:%d", i);
        return;
    }
    int j = i; // 要删除的结点坐标
    int n = m_heap.size() - 1; // 最后一个结点的坐标
    // LOG_DEBUG("j:%d, n:%d", j, n);
    if (j < n)
    {
        swapNode(j, n);
        if (!sink(j, n))
        {
            swim(j);
        }
    }
    /* 删除队尾元素 */
    // LOG_DEBUG("fd:%d",m_heap[n].m_id)
    m_ref.erase(m_heap[n].m_id);
    // LOG_DEBUG("删除之前m_heap大小:%d", m_heap.size());
    m_heap.pop_back();
    // LOG_DEBUG("删除之后m_heap大小:%d", m_heap.size());
}
void TimeManager::swim(int i)
{
    // printf("TimeManager::swim(int %d)\n", i);
    if (i < 0 || i >= static_cast<int>(m_heap.size()))
    {
        LOG_DEBUG("invalid i:%d", i);
        return;
    }
    if(i == 0){return;}
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
    if (i < 0 || i >= static_cast<int>(m_heap.size()))
    {
        LOG_DEBUG("invalid i:%d", i);
        return false;
    }
    if (n < 0 || n > static_cast<int>(m_heap.size()))
    {
        LOG_DEBUG("invalid n");
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
    // LOG_DEBUG(" ");
    std::swap(m_heap[i], m_heap[j]);
    m_ref[m_heap[i].m_id] = i;
    m_ref[m_heap[j].m_id] = j;
}

// 公有成员函数
void TimeManager::addTimer(int id, int timeout, const _timeoutCallBack &cb)
{
    // printf("function:TimeManager::addTimer(int %d, int %d, const _timeoutCallBack &cb)\n", id, timeout);
    if (id < 0)
    {
        LOG_WARN("invalid id");
        return;
    }

    // LOG_DEBUG("fd:%d", id);
    int i;
    if (m_ref.count(id) == 0)
    {
        // new node
        i = m_heap.size(); // i == next valid pos in the m_heap
        m_ref[id] = i;
        m_heap.push_back(Timenode(id, _clock::now() + _ms(timeout), cb));
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
    // printf("退出addTImer\n");
}
void TimeManager::handleExpiredEvent()
{
    // printf("TimeManager::handleExpiredEvent()\n");
    if (m_heap.empty())
    {
        return;
    }
    while (!m_heap.empty())
    {
        // printf("m_heap不为空，正在处理过时连接\n");
        Timenode node = m_heap.front();
        int64_t resTime = std::chrono::duration_cast<_ms>(node.m_expire - _clock::now()).count();
        if ( resTime > 0)
        {
            /*
             timeout是预先设定的socket可以持续的时间
             每一个socket的时间等于当前时间+预设时间
             */
            // printf("当前连接还未超时,fd:%d, resTime:%ld\n", node.m_id, resTime);
            break;
        }
        //std::cout << "定时器关闭客户端连接：\t";
        node.m_cb();
        pop();
    }
}
int TimeManager::getNextHandle()
{
    // 先去小根堆看一下有没有过期的socket通信。如果有，关闭该socket通信
    // printf("TimeManager::getNextHandle()\n");
    handleExpiredEvent();
    int64_t res = -1;
    if (!m_heap.empty())
    {
        // 获取小根堆中第一个还没有过期的结点的时间，单位是us
        res = std::chrono::duration_cast<_ms>(m_heap.front().m_expire - _clock::now()).count();
        if (res < 0)
        {
            // 为什么会出现这种情况？调用完handleExpiredEvent清理掉了已经超时的连接，根节点的连接，
            // 程序运行到这也是需要一段时间的（几ms的可能性），说不定恰好在这就超时了
            res = 0;
        }
    }
    return res;
}
void TimeManager::updateTimer(int id, int timeout)
{
    /*
    由于客户端的浏览器会在得到访问资源后，主动断开，所以这个if检查是很有必要的
    */
    if (m_heap.empty())
    {
        LOG_DEBUG("empty heap");
        return;
    }
    if (m_ref.count(id) == 0)
    {
        LOG_DEBUG("invalid id:%d", id);
        return;
    }
    int i = m_ref[id];
    m_heap[i].m_expire = _clock::now() + _ms(timeout);
    if (!sink(i, m_heap.size()))
    {
        swim(i);
    }
}
// 删除指定id节点，并且用指针触发处理函数
void TimeManager::work(int id)
{
    /* 删除指定id结点，并触发回调函数 */
    //std::cout << "TimeManager::work(int id)" << std::endl;
    if (m_heap.empty() || m_ref.count(id) == 0)
    {
        LOG_DEBUG("invalid id or empty heap");
        return;
    }
    int i = m_ref[id];
    Timenode node = std::move(m_heap[i]);
    node.m_cb();
    m_del(i);
}

void TimeManager::pop()
{
    if (m_heap.empty())
    {
        LOG_DEBUG("empty heap");
        return;
    }
    m_del(0);
}
void TimeManager::clear()
{
    m_heap.clear();
    m_ref.clear();
}