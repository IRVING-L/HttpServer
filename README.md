<center><font  face="黑体" size=20 color = bluie>WebServer详解</font></center>

使用C++语言，编写了一个基于Linux的HttpServer，能够实现上万的QPS。

项目地址：
- [github仓库](https://github.com/IRVING-L/HttpServer)
- [gitee仓库](https://gitee.com/ljunsang/http-server)



# 功能
- 利用I/O多路复用技术的`Epoll`与`线程池`实现【单Reactor、多线程】的高并发服务器模型；
- 利用有限状态机和正则解析HTTP请求报文；
- 利用STL标准库容器封装char，实现自定义的缓冲区；
- 基于堆结构实现的定时器，关闭超时的非活跃网络连接；
- 支持GET请求

# 项目详解

[WebServer项目——threadpool详解](https://blog.csdn.net/qq_42518941/article/details/123487928)

~~[WebServer项目——webserver详解]()
[WebServer项目——httpconnect详解]()
[WebServer项目——buffer详解]()
[WebServer项目——epoller详解]()
[WebServer项目——timer详解]()~~ 

#### 环境要求
- Linux
- C++11

#### 项目启动
拷贝整个项目文件，并在WebServerV3/文件夹下，运行终端，在终端中使用

```cpp
./可执行文件名 端口号
./myserver 9999
```

#### 性能表现

**压力测试**
```cpp
./webbench-1.5/webbench -c 10 -t 10 http://ip:port/
./webbench-1.5/webbench -c 100 -t 10 http://ip:port/
./webbench-1.5/webbench -c 1000 -t 10 http://ip:port/
./webbench-1.5/webbench -c 10000 -t 10 http://ip:port/
```

- 测试环境：Ubuntu18, CPU i7-9700 3.00GHz, 内存 16GB

**QPS对比**
用我自己的测试环境跑了一下【[markparticle](https://github.com/markparticle/WebServer/)】和【[老猫轩仔](https://www.agedcat.com/programming_language/cpp/537.html)】的web服务器，并将测试情况记录如下表

|                                                              |  10   |  100  | 1000  | 10000 |
| :----------------------------------------------------------: | :---: | :---: | :---: | :---: |
|  [markparticle](https://github.com/markparticle/WebServer/)  | 8929  | 9126  | 9209  |  155  |
| [老猫轩仔](https://www.agedcat.com/programming_language/cpp/537.html) | 12243 | 11967 | 11926 |  132  |
|       [本项目](https://github.com/IRVING-L/HttpServer)       | 7666  | 8648  | 9128  | 11220 |

- （*20000个客户端10s的高并发访问下，QPS一度达到12210。*）
- （*`老猫轩仔`的服务器在压力测试过程中会出现无法访问的情况。*）
- （*`markparticle`的服务器配置了MySQL，操作上有所不同，暂时没有使用自己的机器进行测试。*）
> 【老猫轩仔】的服务器和我有两点主要的不同：异步读写和定时器管理。就上述的测试结果来看，我猜测在客户端数量较少的时候，使用定时器能够提高服务器的QPS，但是当客户端数量上万之后，频繁的管理定时器，拖累了服务器的响应速度。QPS在上万客户端访问时，降到了百分之一。

#### 致谢
@[老猫轩仔](https://www.agedcat.com/programming_language/cpp/537.html)
[](https://blog.csdn.net/qq_42518941/article/details/123487928)