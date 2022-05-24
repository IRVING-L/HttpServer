#include "httpresponse.h"
#include "log.h"
const std::unordered_map<std::string, std::string> Httpresponse::SUFFIX_TYPE = {
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/nsword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css "},
    {".js", "text/javascript "},
};

const std::unordered_map<int, std::string> Httpresponse::CODE_STATUS = {
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
};

const std::unordered_map<int, std::string> Httpresponse::CODE_PATH = {
    {400, "/400.html"},
    {403, "/403.html"},
    {404, "/404.html"},
};

Httpresponse::Httpresponse()
{
    m_code = -1;
    m_path = m_srcDir = "";
    m_keepalive = false;
    m_mmfile = nullptr;
    mmFileState = {0};
}
Httpresponse::~Httpresponse()
{
    _unmapFile();
}

void Httpresponse::init(const std::string srcDir, const std::string path, bool isKeepAlive, int code)
{
    //printf("function: Httpresponse::init\n");
    if (srcDir == "")
    {
        // std::cout << "location:httpresponse.cpp::init"
        //           << "error:invalid srcDir" << std::endl;
        LOG_ERROR("error:invalid srcDir");
        return;
    }
    if (m_mmfile != nullptr)
    {
        _unmapFile();
    }
    // std::cout << "srcDir:" << srcDir << std::endl
    //             << "path:" << path << std::endl;
    m_code = code;
    m_keepalive = isKeepAlive;
    m_path = path;
    m_srcDir = srcDir;
    m_mmfile = nullptr;
    mmFileState = {0};
}
void Httpresponse::_unmapFile()
{
    if (m_mmfile != nullptr)
    {
        munmap(m_mmfile, mmFileState.st_size);
        m_mmfile = nullptr;
    }
}
bool Httpresponse::makeResponse(Buffer &buffer)
{
    // 获取请求资源的状态，使用stat函数
    // 404 Not Found
    // printf("function:Httpresponse::makeResponse(Buffer &buffer)\n");
    // printf("m_srcDir:%s, m_path:%s\n", m_srcDir.data(), m_path.data());
    if (stat((m_srcDir + m_path).data(), &mmFileState) < 0 || S_ISDIR(mmFileState.st_mode))
    {
        // 找不到文件 || 文件为目录文件
        m_code = 404;
    }
    // 403 forbidden
    else if (!(mmFileState.st_mode & S_IROTH))
    {
        // S_IROTH 其他组成员访问
        m_code = 403;
    }
    // printf("old m_code: %d\n", m_code);
    if(m_code == -1) { m_code = 200; }
    // printf("current m_code: %d\n", m_code);
    errorHTML();
    addStateLine(buffer);
    addHeader(buffer);
    addContent(buffer);
    // 如果没有正确获取到资源，需要报告一下状态
    return m_code == 200 ? true : false;
}

// 填写首部字段
void Httpresponse::addStateLine(Buffer &buffer)
{
    //printf("function:Httpresponse::addStateLine(Buffer &buffer)\n");
    std::string status;
    // 从哈希表从查找m_code对应的状态字
    if (CODE_STATUS.count(m_code) > 0)
    {
        status = CODE_STATUS.find(m_code)->second;
    }
    else
    {
        m_code = 400; // BAD REQUEST
        status = CODE_STATUS.find(400)->second;
    }
    // 把响应的字段写入到缓冲区中
    buffer.append("HTTP/1.1 " + std::to_string(m_code) + " " + status + "\r\n");
}
void Httpresponse::addHeader(Buffer &buffer)
{
    // Connection
    //printf("function:Httpresponse::addHeader(Buffer &buffer)\n");
    buffer.append("Connection: ");
    if (m_keepalive)
    {
        buffer.append("keep-alive\r\n");
        buffer.append("keep-alive: max=6, timeout=120\r\n");
    }
    else
    {
        buffer.append("close\r\n");
    }
    // Content-type
    buffer.append("Content-type: " + getFileType() + "\r\n");
    // content-legnth
    buffer.append("Content-length: " + std::to_string(mmFileState.st_size) + "\r\n\r\n");
}
void Httpresponse::addContent(Buffer &buffer)
{
    // printf("function:Httpresponse::addContent(Buffer &buffer)\n");
    // printf("path:%s\n", (m_srcDir + m_path).data());
    int srcFd = open((m_srcDir + m_path).data(), O_RDONLY);
    if (srcFd == -1)
    {
        errorContent(buffer, "File NotFound!\r\n");
        // std::cout << "location:httpresponse.cpp:addContent,"
        //           << "error:open file faild" << std::endl;
        LOG_ERROR("error:open file faild");
        return;
    }
    //内存映射
    int *mmret = (int *)mmap(0, mmFileState.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if (*mmret == -1)
    {
        errorContent(buffer, "File NotFound!\r\n");
        // std::cout << "location:httpresponse.cpp:addContent,"
        //           << "error:mmap faild" << std::endl;
        LOG_ERROR("error:mmap faild");
        return;
    }
    m_mmfile = (char *)mmret;
    close(srcFd);
}

void Httpresponse::errorHTML()
{
    //printf("function:Httpresponse::errorHTML()\n");
    if (CODE_PATH.count(m_code) > 0)
    {
        m_path = CODE_PATH.find(m_code)->second;
        stat((m_srcDir + m_path).data(), &mmFileState);
    }
}
std::string Httpresponse::getFileType()
{
    std::string::size_type idx = m_path.find_last_of('.');
    if (idx == std::string::npos)
    {
        // not found
        return "text/plain";
    }
    std::string suffix = m_path.substr(idx); // 从'.'开始的子串
    if (SUFFIX_TYPE.count(suffix) > 0)
    {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

char *Httpresponse::file() const
{
    return m_mmfile;
}
size_t Httpresponse::fileLen() const
{
    return mmFileState.st_size;
}
void Httpresponse::errorContent(Buffer &buffer, std::string message)
{
    std::string body;
    std::string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if (CODE_STATUS.count(m_code) == 1)
    {
        status = CODE_STATUS.find(m_code)->second;
    }
    else
    {
        status = "Bad Request";
    }
    body += std::to_string(m_code) + " : " + status + "\r\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>HttpWebServer</em></body></html>";

    buffer.append("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    buffer.append(body);
}
int Httpresponse::_code() const
{
    return m_code;
}