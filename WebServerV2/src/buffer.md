<!--  -->## Buffer.h
在这个项目中，客户端连接发来的HTTP请求以及回复给客户端所请求的资源，都需要缓冲区的存在。在【牛客网】的项目中，使用了两个char数组用于读取和写入的缓冲区，同时辅以很多指针来完成缓冲区的读写操作。
写这个头文件的目的，正是将原有的【牛客网】项目的涉及缓冲区的纷繁的代码整合一下，用C++面向对象的思想创建一个Buffer类，并在类中封装对应的函数和接口
下面说一下缓冲区需要完成的功能：

向【缓冲区】中写入数据：
- 写入char类型的一块数据
- 写入string类型的一块数据
- 写入void *类型的一块数据
将【缓冲区】中的数据发送出去
- 通过write/send发送
- 通过writev分散写函数发送
上面便是缓冲区对外需要能够完成的功能，对内，为了能够正确，有效的完成上面的功能，还需要一些其他函数

自动增长扩容
更加性能的做法是利用C++std::move，自己实现扩容的函数：寻找一块更大的内存，并将原来内存中的数据移动过去，并且释放之前的内存。

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
由于buffer缓冲区既要作为读缓冲区，也要作为写缓冲区，所以我们既需要指示当前读到哪里了，也需要指示当前写到哪里了。所以在buffer缓冲区里面设置变量：
```cpp
size_t m_readPos;
size_t m_writePos;
```
由于我的服务器项目，目前是每一个socket通信连接享有一个单独的缓冲区，所以我并没有像其他人那样给这两个变量设置成std::atomic原子类型

#### buffer所需要的方法
读写接口
缓冲区最重要的就是读写接口，主要可以分为与客户端直接IO交互所需要的读写接口，以及收到客户端HTTP请求后，我们在处理过程中需要对缓冲区的读写接口。

**与客户端直接IO的读写接口**
```cpp
ssize_t readFd(int fd,int* Errno);
ssize_t writeFd(int fd,int* Errno);
```
这个功能直接用read()/write()、readv()/writev()函数来实现。

最后无论是读还是写，结束之后都需要更新读指针和写指针的位置。

**处理HTTP连接过程中需要的读写接口**
需要读buffer里面的数据，一般情况下也需要定义方法。但是在这里，我们用STL提供的对vector的方法和对string的支持就可以实现这些功能。所以，我们这部分主要需要实现向buffer缓冲区中添加数据的方法。
```cpp
void append(const char* str,size_t len);
void append(const std::string& str);
void append(const void* data,size_t len);
```
根据后续功能的需要，写了各种需要的实现。其中的具体功能可以参考具体的代码实现。

在往buffer缓冲区中添加数据也需要考虑超过容量的情况，也就是我们还需要实现这种情况下怎么动态扩容，怎么保证能够写入超过现有容量的数据，怎么分配新的空间。

也就是以下方法：
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