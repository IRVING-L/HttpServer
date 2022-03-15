#include "../lib/httprequest.h"

// 枚举类型的变量定义
enum class Httprequest::PARSE_STATE : int
{
    REQUEST_LINE,
    HEADERS,
    BODY,
    FINISH
};
enum class Httprequest::HTTP_CODE : int
{
    NO_REQUEST = 0,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURSE,
    FORBIDDENT_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};
const std::unordered_set<std::string> Httprequest::DEFAULT_HTML{
    "/index", "/welcome", "/video", "/picture"};
Httprequest::Httprequest() { _init(); }

// 成员函数
void Httprequest::_init()
{
    //printf("function:Httprequest::_init()\n");
    m_method = m_path = m_version = m_body = "";
    m_state = PARSE_STATE::REQUEST_LINE;
    m_header.clear();
    m_post.clear();
}

// 解析请求报文
bool Httprequest::parse(Buffer &buff)
{
    //printf("function:Httprequest::parse(Buffer &buff)\n");
    const char CRLF[] = "\r\n";
    if (buff.readableBytes() <= 0)
    {
        std::cout << "location:httprequest.cpp:pasre,"
                  << "error:empty buff" << std::endl;
        return false;
    }
    while (buff.readableBytes() && m_state != PARSE_STATE::FINISH)
    {
        const char *lineEnd = std::search(buff.curReadPtr(), buff.curWritePtr(), CRLF, CRLF + 2);
        std::string line(buff.curReadPtr(), lineEnd); // 一行内容
        // 有限状态机
        switch (m_state)
        {
        case PARSE_STATE::REQUEST_LINE:
            if (!_parseRequestLine(line))
            {
                return false;
            }
            // 再次对请求行中的路径进行解析
            _parsePath();
            break;
        case PARSE_STATE::HEADERS:
            _parseRequestHeader(line);

            if (buff.readableBytes() <= 2)
            {
                // 2 是CRLF换行符的大小
                m_state = PARSE_STATE::FINISH;
            }
            break;
        case PARSE_STATE::BODY:
            _parseDataBody(line);

            break;
        default:
            std::cout << "未知状态\n";
            break;
        }
        if (lineEnd == buff.curWritePtr())
        {
            break;
        }
        // 更新读指针的位置
        buff.updateReadPtr(static_cast<size_t>(lineEnd - buff.curReadPtr()) + 2);
    }
    return true;
}

bool Httprequest::_parseRequestLine(const std::string &line) //解析请求行
{
    // printf("function:Httprequest::_parseRequestLine(const std::string &line)\n");
    // 使用C++11的正则表达式进行查找
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$"); //
    std::smatch subMatch;                                // 存储查找得到的结果？
    if (std::regex_match(line, subMatch, patten))
    {
        // 查找到了，并把查找得到的结果保存在subMatch中
        m_method = subMatch[1];
        m_path = subMatch[2];
        m_version = subMatch[3];
        // 更改状态机的状态
        m_state = PARSE_STATE::HEADERS; // 接下来去解析首部字段
        // printf("m_method:%s, m_path:%s, m_version:%s\n", m_method.data(), m_path.data(), m_version.data());
        return true;
    }
    return false;
}
void Httprequest::_parseRequestHeader(const std::string &line) //解析请求头
{
    //printf("function:Httprequest::_parseRequestHeader(const std::string &line)\n");
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if (std::regex_match(line, subMatch, patten))
    {
        //std::cout << subMatch[1] << ": " << subMatch[2] << std::endl;
        m_header[subMatch[1]] = subMatch[2];
    }
    else
    {
        // 首部字段解析完毕
       // printf("解析首部字段完毕\n");
        m_state = PARSE_STATE::BODY;
    }
}
void Httprequest::_parseDataBody(const std::string &line) //解析数据体
{
    //printf("function:Httprequest::_parseDataBody(const std::string &line)\n");
    m_body = line;
    //_parsePost();
    m_state = PARSE_STATE::FINISH;
}
void Httprequest::_parsePath() //解析请求资源的网址
{
    // printf("Httprequest::_parsePath()\n");
    if (m_path == "/")
    {
        // 访问首页,自动跳转到固定资源位置
        m_path = "/index.html";
    }
    else
    {
        // 查找
        for (auto &item : DEFAULT_HTML)
        {
            if (item == m_path)
            {
                m_path += ".html";
                break;
            }
        }
    }
    // printf("m_path:%s\n", m_path.data());
}

//获取HTTP信息
std::string Httprequest::path() const
{
    return m_path;
}
std::string Httprequest::method() const
{
    return m_method;
}
std::string Httprequest::version() const
{
    return m_version;
}
// std::string Httprequest::getPost(const std::string &key) const
// {
// }
// std::string Httprequest::getPost(const char *key) const
// {
// }
bool Httprequest::isKeepAlive() const
{
    if (m_header.count("Connection") > 0)
    {
        return m_header.find("Connection")->second == "keep-alive" && m_version == "HTTP/1.1";
    }
    else return false;
}