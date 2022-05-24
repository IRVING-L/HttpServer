#include "httprequest.h"

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
    "/index", "/welcome", "/video", "/picture", "/login", "/register"};
Httprequest::Httprequest() { _init(); }

// 成员函数
void Httprequest::_init()
{
    // printf("function:Httprequest::_init()\n");
    m_method = m_path = m_version = m_body = "";
    m_state = PARSE_STATE::REQUEST_LINE;
    m_header.clear();
    m_usrInfoPost.clear();
}

// 解析请求报文
bool Httprequest::parse(Buffer &buff)
{
    // printf("function:Httprequest::parse(Buffer &buff)\n");
    const char CRLF[] = "\r\n";
    if (buff.readableBytes() <= 0)
    {
        // std::cout << "location:httprequest.cpp:pasre,"
        //           << "error:empty buff" << std::endl;
        LOG_ERROR("error:empty buff");
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
        {
            _parseDataBody(line);
            // 解析完post，拿到用户名和密码后，就要去装填m_path找到对应的网页了
            // 调用mysql操作的对象，查询mysql判断账户和密码是否正确
            _getPost();
            break;
        }
        default:
            // std::cout << "未知状态\n";
            LOG_WARN("未知状态");
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
    // printf("function:Httprequest::_parseRequestHeader(const std::string &line)\n");
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if (std::regex_match(line, subMatch, patten))
    {
        // std::cout << subMatch[1] << ": " << subMatch[2] << std::endl;
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
    // printf("function:Httprequest::_parseDataBody(const std::string &line)\n");
    m_body = line;
    // std::cout << m_body << std::endl;
    LOG_DEBUG("POST revc: %s", m_body.c_str());
    if (m_method == "POST")
    {
        // std::cout << m_method << std::endl;
        _parsePost();
    }
    m_state = PARSE_STATE::FINISH;
}
void Httprequest::_parsePath() //解析请求资源的网址
{
    // printf("Httprequest::_parsePath()\n");
    if (m_method == "GET")
    {
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
    }
    // printf("m_path:%s\n", m_path.data());
}
// 解析post请求
void Httprequest::_parsePost()
{
    /*
    post请求的数据一般都会被编码，浏览器默认的编码格式为：application/x-www-form-urlencoded
    具体的编码原理网上有不少帖子
    */
    if ("" == m_body || m_header.count("Content-Type") == 0)
    { 
        LOG_ERROR("POST解析失败");
        return;
    }
    else if (m_header["Content-Type"] == "application/x-www-form-urlencoded")
    {
        // 调用URL解码函数进行解码
        URLdecode(m_body);
    }
    else if (m_header["Content-Type"] == "application/json")
    {
        // 调用JSON解析函数进行解析
        JsonParse(m_body);
    }
}
// URL解析函数
int Httprequest::ConverHex(char ch)
{
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return ch;
}
void Httprequest::URLdecode(std::string &str)
{
    std::string key, value;
    int num = 0;
    int n = m_body.size();
    int i = 0, j = 0;

    for (; i < n; i++)
    {
        char ch = m_body[i];
        switch (ch)
        {
        case '=':
            key = m_body.substr(j, i - j);
            j = i + 1;
            break;
        case '+':
            m_body[i] = ' ';
            break;
        case '%':
            num = ConverHex(m_body[i + 1]) * 16 + ConverHex(m_body[i + 2]);
            m_body[i + 2] = num % 10 + '0';
            m_body[i + 1] = num / 10 + '0';
            i += 2;
            break;
        case '&':
            value = m_body.substr(j, i - j);
            j = i + 1;
            m_usrInfoPost[key] = value;
            printf("%s = %s", key.c_str(), value.c_str());
            break;
        default:
            break;
        }
    }
    assert(j <= i);
    if (m_usrInfoPost.count(key) == 0 && j < i)
    {
        value = m_body.substr(j, i - j);
        // 保存解析得到的【用户名】和【密码】
        m_usrInfoPost[key] = value;
        printf("%s = %s", key.c_str(), value.c_str());
    }
}

// Json解析函数
void Httprequest::JsonParse(std::string &str)
{
}
// 处理POST的信息
void Httprequest::_getPost()
{
    std::string name = m_usrInfoPost["username"];
    std::string pwd = m_usrInfoPost["password"];

    LOG_INFO("username:%s, password:%s", name.c_str(), pwd.c_str());
    User usr = _userModel.query(name);

    if (m_path == "/register")
    {
        // 注册业务
        usr.setName(name);
        usr.setPwd(pwd);
        _userModel.insert(usr);
        m_path = "/login.html";
    }
    else if (m_path == "/login" && usr.getPwd() == pwd)
    {
        m_path = "/welcome.html";
    }
    else
    {
        m_path = "/error.html";
    }
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
        // LOG_DEBUG("Connection:%s, version:%s", m_header.find("Connection")->second.c_str(), m_version.c_str())
        return m_header.find("Connection")->second == "keep-alive" && m_version == "1.1";
    }
    else
        return false;
}