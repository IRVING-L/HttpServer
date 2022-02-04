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

Httprequest::Httprequest() { _init(); }

// 成员函数
void Httprequest::_init()
{
    m_method = m_path = m_version = m_body = "";
    m_state = PARSE_STATE::REQUEST_LINE;
    m_header.clear();
    m_post.clear();
}
// 解析请求报文
bool Httprequest::parse(Buffer &buff)
{
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
            if (_parseRequestLine(line))
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
            break;
        }
        if (lineEnd == buff.curWritePtr())
        {
            break;
        }
        // 更新读指针的位置
        buff.updateReadPtr(static_cast<size_t>(lineEnd - buff.curReadPtr()) + 2);
    }
}