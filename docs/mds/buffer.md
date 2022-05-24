## Buffer.h
在这个项目中，客户端连接发来的HTTP请求以及回复给客户端所请求的资源，都需要缓冲区的存在。在【牛客网】的项目中，使用了两个char数组用于读取和写入的缓冲区，同时辅以很多指针来完成缓冲区的读写操作。
写这个头文件的目的，正是将原有的【牛客网】项目的涉及缓冲区的纷繁的代码整合一下，用C++面向对象的思想创建一个Buffer类，并在类中封装对应的函数和接口
下面说一下缓冲区需要完成的功能：

向【缓冲区】中写入数据：
- 写入char类型的一块数据
- 写入string类型的一块数据
- 写入void *类型的一块数据

在网络通信中，接收和发送数据（针对的是非阻塞I/O读写）
- 通过read/recv/readv接收数据
- 通过write/send/writev发送

上面便是缓冲区对外需要能够完成的功能，对内，为了能够正确，有效的完成上面的功能，还需要一些其他函数


### buffer缓冲区的组成
省去每一个类必有的构造和析构函数，还需要以下部分：

#### buffer的存储实体
缓冲区的最主要需要是读写数据的存储，也就是需要一个存储的实体。
```cpp
// char数组的头指针
char *m_elements;
// char数组的尾指针
char *m_end;
```
#### buffer所需要的变量
由于buffer缓冲区既要作为读缓冲区，也要作为写缓冲区。对于我们使用**new**开辟出来的一块内存，既需要指示当前读到哪里了，也需要指示当前写到哪里了。所以在buffer缓冲区里面设置变量：
```cpp
size_t m_readPos;
size_t m_writePos;
```
由于我的服务器项目，目前是每一个socket通信连接享有一个单独的缓冲区，所以我并没有像其他人那样给这两个变量设置成std::atomic原子类型

#### buffer所需要的方法
- 读写接口
缓冲区最重要的就是读写接口：
    - 主要可以分为与客户端进行网络I/O交互所需要的读写接口
    - 以及收到客户端HTTP请求后，我们在处理过程中需要对缓冲区的添加数据、读取数据的接口。

**与客户端交互的网络I/O的读写接口**
```cpp
ssize_t readFd(int fd,int* Errno);
ssize_t writeFd(int fd,int* Errno);
```
这个功能一般是对read()/write()、readv()/writev()函数进行二次封装实现。

值得注意的是，涉及网络I/O的交互，需要明确是否为阻塞I/O。就拿本项目来说，采用的是同步的非阻塞I/O，并且是基于**epoll**的边缘触发`ET`模式的交互逻辑。在readFd函数中，是需要将原生的`read`函数放入一个while循环中，直到无法从内核缓冲区中读取到数据为止。

但是陈硕大神的Muduo网络库，其readFd是基于epoll的水平触发模式。水平触发模式在内核缓冲区数据未读完前不断的通过epoll_wait告知程序进行数据读取。传统意义上来讲，水平触发导致的多次epoll_wait调用相较于while中多次read，其开销是更大的，所以一般推崇ET模式。

但具体的测试我还没有做，有时间有机会再挖掘一下这方面的知识。

最后无论是读还是写，结束之后都需要更新读指针和写指针的位置。

**处理HTTP连接过程中需要的读写接口**
需要读buffer里面的数据，那就需要定义对应的方法。但是在这里，我们用STL提供的对vector的方法和对string的支持就可以实现这些功能。所以，我们这部分主要需要实现向buffer缓冲区中添加数据的方法。
```cpp
void append(const char* str,size_t len);
void append(const std::string& str);
void append(const void* data,size_t len);
```
根据后续功能的需要，写了各种需要的实现。其中的具体功能可以参考具体的代码实现。

在往buffer缓冲区中添加数据也需要考虑超过容量的情况，也就是我们还需要实现这种情况下怎么动态扩容。（**是的，缓冲区需要满足自动扩容的功能，以便于能够存储较大的文件数据**）

怎么保证能够写入超过现有容量的数据，怎么分配新的空间。也就是以下方法：
```cpp
void ensureWriteable(size_t len);
void allocateSpace(size_t len);
```

完整代码实现：
```cpp
#ifndef __BUFFERV2_H__
#define __BUFFERV2_H__
#include <iostream>
#include <string>
#include <alloca.h>
#include <unistd.h>
#include <assert.h>
#include <strings.h> /* bzero */
#include <sys/types.h>
#include <sys/socket.h> /* recv */
#define BUF_SIZE_INIT 128
class Buffer
{
public:
    Buffer(int bufsize = BUF_SIZE_INIT);
    ~Buffer();
    // 向缓冲区写入数据
    void append(const char *str, size_t len);
    void append(const std::string &str);
    void append(const void *data, size_t len);
    // void append(const Buffer &buf);
    // 将缓冲区中的数据，发送到socket通信的对端
    ssize_t writeFd(int sockfd, int *Errno);
    // 从socket通信的对端，接收数据到缓冲区
    ssize_t recvFd(int sockfd, int *Errno);

    size_t _size() const;
    //缓存区中可以读取的字节数
    size_t writeableBytes() const;
    //缓存区中可以写入的字节数
    size_t readableBytes() const;
    //缓存区中已经读取的字节数
    size_t readBytes() const;
    //缓冲区中已经写入的字节数
    size_t writeBytes() const;
    // 缓冲区头。有了缓冲区头+写入的数据长度，便可以在类外使用writev函数进行写入
    const char *beginPtr() const;
    // 将缓冲区中的内容输出成string
    std::string _all2str();

private:
    // 返回缓冲区写入指针的位置
    char *curWritePtr();
    // 检查空间是否足够
    void ensureWriteable(size_t len);
    // 获得更多内存并拷贝已有元素
    void allocateSpace(size_t len);
    // 销毁元素并释放内存
    void _free();
    // 将指针和变量初始化
    void _init();
    // 底层的数据结构，char数组
    // char数组的头指针
    char *m_elements;
    // char数组的尾指针
    char *m_end;
    // string
    std::string m_str;

    size_t m_readPos;
    size_t m_writePos;
};

#endif

```

————时间分割线————
2022年2月11日

在复盘了整个项目之后，我发现自定义缓冲区还是有必要的，它能在一定程序上，减少我们调用例如`vsnprintf`这种很少使用的函数。

在V1版本中，我想向字符串数组中写入一段字符数据，由于输入的字符串数量不定，就需要使用`vsnprintf`和`va_list`这两个函数/宏。我花了不少时间了解他们的特性和使用方法，我相信没接触过这俩家伙的同学们都会花一定得时间才能理解这两个函数怎么用。而且这俩是C的库函数，我们的初衷是利用这个项目尽可能的锻炼C++编程的能力。

但是在V2我们自定义的缓冲区中，我们实现了对外接口append函数，我们使用了【函数重载】定义了三个同名但是接收形参类型不同的函数。其中接收形参为std::string类型的，在调用该函数时，利用C++的std::string 对象可以相加的特点，合理的使用+运算符便可以实现不定参数的输入，非常的方便。而这样的调用方式正是C++ programmer所熟悉的。

————时间分割线————
2022年4月20日

思考：在我的buffer中，一次read最多只能读取64KB左右的数据。这是否表明，我一次http传输（例如POST）最大只能有64KB？假如我要传输一张8MB的照片能否做到？

回答：能够做到。
- 首先，我们需要明确一点，无论是使用read，还是recv，或者是readv读取数据，都不是直接从socket端口接收数据。read函数是将操作系统从网口接收并保存在内核缓冲区的数据——》拷贝到用户态。所以，这里应该考虑，内核缓冲区，一次性最大能够将多大容积的数据拷贝给用户区。
- 其次，再说回这项目。在[buffer.cpp:readFd]()函数中，调用了readv函数进行分散读取，一次性最大读取的上限是64KB。然后，在[httpconnect.cpp:readBuffer]()函数中，使用while循环多次调用[buffer.cpp:readFd]()直到把内核缓冲区中的数据读取干净。要知道，虽然调用函数进行读取是需要花费时间的，但是网络上的数据发送也是要花时间的！例如，千兆网卡传输8MB的照片，也需要花费至少62.5ms。62.5ms应该是足够完成读取的。
- 你想，64KB的网络传输耗时为500us。而我们将64KB的数据读取写入到Buffer中去，应该是花不了500us的。也就是说，读取写入的速度高于网络传输的速度，网络数据包来了就给读走了，内核缓冲区无法堆积超过64KB的数据，也就不会发生读取到的字节数，大于提供内存的容量造成数据丢失问题了。

