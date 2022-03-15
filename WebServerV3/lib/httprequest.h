#ifndef __HTTPREQUEST_H__
#define __HTTPREQUEST_H__
#include <arpa/inet.h> //sockaddr_in
#include <sys/uio.h>   //readv/writev
#include <iostream>
#include <sys/types.h>
#include <assert.h>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <regex>
#include "buffer.h"
/*
这个类需要完成的功能/任务：
- 完成对从socket通信对端发送来的http请求报文的解析：
    - 状态行解析
        - 请求方法
        - 访问资源的地址
        - HTTP协议版本
    - 首部字段解析
        - 长/短连接
        - 请求报文长度
    - 报文主体解析（GET关键字没有）
*/
class Httprequest
{
    // 私有成员变量：枚举类型
    enum class PARSE_STATE : int;
    enum class HTTP_CODE : int;

public:
    Httprequest();
    ~Httprequest() = default;

    // 成员函数
    void _init();

    bool parse(Buffer &buff); //解析HTTP请求

    //获取HTTP信息
    std::string path() const;
    std::string method() const;
    std::string version() const;
    // std::string getPost(const std::string &key) const;
    // std::string getPost(const char *key) const;
    bool isKeepAlive() const;

private:
    bool _parseRequestLine(const std::string &line);   //解析请求行
    void _parseRequestHeader(const std::string &line); //解析请求头
    void _parseDataBody(const std::string &line);      //解析数据体
    void _parsePath();                                 //解析请求资源的网址
    void _parsePost();                                 //针对post方法，解析上传的内容

    PARSE_STATE m_state;
    std::string m_method, m_path, m_version, m_body;
    std::unordered_map<std::string, std::string> m_header; // 首部字段的哈希表
    std::unordered_map<std::string, std::string> m_post;

    static const std::unordered_set<std::string>DEFAULT_HTML; // 用于查找网页的哈希表
};

#endif