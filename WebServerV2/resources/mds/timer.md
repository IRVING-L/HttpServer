## 定时器的介绍
为了提高Web服务器的效率，我们考虑给每一个HTTP连接加一个定时器。

定时器给每一个HTTP连接设置一个过期时间，然后我们定时清理超过过期时间的连接，会减少服务器的无效资源的耗费，提高服务器的运行效率。

我们还需要考虑一下如何管理和组织这些定时器。设置定时器的主要目的是为了清理过期连接，为了方便找到过期连接，首先考虑使用优先队列，按过期时间排序，让过期的排在前面就可以了。但是这样的话，虽然处理过期连接方便了，当时没法更新一个连接的过期时间。

最后，选择一个折中的方法。用vector容器存储定时器，然后在这之上实现堆结构，这样各个操作的代价就得到了相对均衡。

## 定时器的组成
#### 定时器结点
为了实现定时器的功能，我们首先需要辨别每一个HTTP连接，每一个HTTP连接会有一个独有的描述符（socket），我们可以用这个描述符来标记这个连接，记为id。同时，我们还需要设置每一个HTTP连接的过期时间。

为了后面处理过期连接的方便，我们给每一个定时器里面放置一个回调函数，用来关闭过期连接。

为了便于定时器结点的比较，主要是后续堆结构的实现方便，我们还需要重载比较运算符。

~~~cpp
typedef std::chrono::milliseconds _ms;
typedef std::chrono::high_resolution_clock _clock;
typedef _clock::time_point _timeStamp;
typedef std::function<void()> _timeoutCallBack;

class TimerNode{
public:
    int id;             //用来标记定时器
    TimeStamp expire;   //设置过期时间
    TimeoutCallBack cb; //设置一个回调函数用来方便删除定时器时将对应的HTTP连接关闭

    //需要的功能可以自己设定
    bool operator<(const TimerNode& t)
    {
        return expire<t.expire;
    }
};

~~~
上面的代码我们使用了自C++11之后的时间库**chrono**。这里简单介绍一下chrono库的使用方式

chrono时间库函数最大可以精确到纳秒级别的计时。使用上相较于C语言的time.h库函数，没有那么简便
本文对chrono时间库函数的使用如下
~~~cpp
/*
_clock::now()：获取当前时间
_ms(timeout)：timeout个微秒
_clock::now() + _ms(timeout)这句代码是将 获取到的当前时间增加指定的微秒
最后通过.count()函数返回结果。因为now()获取到的时间，无法直接显示*/
(_clock::now() + _ms(timeout)).count();
~~~
这次代码使用的是high_resolution_clock高精度时钟类。chrono库中还有另外两个普通的时间类： system_clock、steady_clock。

从我个人的对chrono库的理解，使用该库的要点如下：
- 选定一个时钟类（high_resolution_clock、system_clock、steady_clock)
- 使用时钟类成员time_point 创建一个对象，并通过时钟类成员now成员获取当前时间
- 结果处理。一般来说，通过两个时间戳time_point相减就可以得到持续时间tt。该时间无法直接查看，需要使用duration_cast<T>()进行时间转换，转换的方式可以是：
~~~cpp
    - 这个是精确到纳秒的浮点数时间，单位是s
    duration<double> tt1 = duration_cast<duration<double>>(tt); 
    - 可以使用库汇总预定义好的时间刻度：时、分、秒、毫秒、微秒、纳秒.
    hours tt1 = duration_cast<hours>(tt);
    - 打印输出时间
    cout << tt1.count() << endl;
~~~
#### 定时器的管理
我们定义一个类 TimerManager来管理定时器。然后考虑一下这儿结构的构成。

实现堆结构必须的组件
首先是用来存储定时器的存储实体，用vector就可以：

`std::vector<TimerNode>heap_;`
然后在vector的基础上实现堆结构还需要下列方法：
```cpp
void addTimer(int id,int timeout,const TimeoutCallBack& cb);//添加一个定时器
void del_(size_t i);//删除指定定时器
void siftup_(size_t i);//向上调整
bool siftdown_(size_t index,size_t n);//向下调整
void swapNode_(size_t i,size_t j);//交换两个结点位置
```
对于堆结构，主要就是往堆里添加一个结点和删除一个结点，对应的就是addTimer方法和del_方法，但是实现这两个方法同时还需要实现交换两个结点的 swapNode_方法和向上调整向下调整的方法。

提供给外界的接口
主要包括下面的方法：
```cpp
void addTimer(int id,int timeout,const TimeoutCallBack& cb);
//处理过期的定时器
void handle_expired_event();
//下一次处理过期定时器的时间
int getNextHandle();

void update(int id,int timeout);
//删除制定id节点，并且用指针触发处理函数
void work(int id);
```
添加定时器的方法也需要暴露给上层，还有就是最主要的处理过期连接的方法handle_expired_event，以及获取下一次处理的时间的方法int getNextHandle。还有就是在HTTP连接的处理过程中需要的对某一个连接对应定时器的过期时间做出改变所需要的update方法和处理过期时间过程中需要调用的work方法。

构造和析构函数
构造的时候初始化一下vector，析构的时候清理vector。

还可能有一些在实现这些方法的过程中提取出来的一个子模块所构成的方法。