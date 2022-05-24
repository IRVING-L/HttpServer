<center><font color=red size=6>WebServer项目详解</font></center>

用C++实现的高性能WEB服务器，暂时还没使用webbench进行压力测试

#### 功能
- 利用I/O多路复用技术的`Epoll`与`线程池`实现【单Reactor、多线程】的高并发服务器模型；
- 利用有限状态机和正则解析HTTP请求报文；
- 利用STL标准库容器封装char，实现自定义的缓冲区；
- 基于堆结构实现的定时器，关闭超时的非活跃网络连接；
- 使用MySQL数据库作为用户登陆信息的落地存储；
- 支持GET、POST请求

#### 项目详解

[WebServer项目——webserver详解](/docs/mds/webserver.md)  
[WebServer项目——httpconnect详解](/docs/mds/httpconnect.md)  
[WebServer项目——buffer详解](/docs/mds/buffer.md)  
[WebServer项目——epoller详解](/docs/mds/epoller.md)  
[WebServer项目——timer详解](/docs/mds/timer.md)  
[WebServer项目——threadpool详解](/docs/mds/threadpool.md)  
[WebServer项目——MySQL数据库使用详解](/docs/mds/mysql.md)  
[WebServer项目——日志库Log详解](/docs/mds/log.md)  

#### 环境要求
- Linux
- C++11
- MySQL

#### 项目启动
拷贝整个项目文件，在终端中使用

```cpp
// 编译程序
sudo ./autobuild.sh
/* ./可执行文件名 端口号 */
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

|                  |  10   |  100  | 1000  | 10000 |
| :--------------: | :---: | :---: | :---: | :---: |
| [markparticle](https://github.com/markparticle/WebServer/) |  4657  |  4624  |  4549  |   4225  |
|   [老猫轩仔](https://www.agedcat.com/programming_language/cpp/537.html)   | 13530 | 13690 | 13688 |  12345  |
|    [本项目/带日志输出](https://github.com/IRVING-L/HttpServer)    |   3340     |    4233    |    4280    |    2500    |
|    [本项目/不带日志输出](https://github.com/IRVING-L/HttpServer)    |    4413    |      4484  |    4264    |    3667    |
*横坐标为并发客户端数量；表格数据为QPS，单位page/s*
- 和【老猫轩仔】的QPS相比，差的有点离谱。。。


#### 致谢
@[老猫轩仔](https://www.agedcat.com/programming_language/cpp/537.html)
@[markparticle](https://github.com/markparticle/WebServer/)
