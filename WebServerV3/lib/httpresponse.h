#ifndef __HTTPRESPONSE_H__
#define __HTTPRESPONSE_H__

#include <unordered_map>
#include <fcntl.h>    //open
#include <unistd.h>   //close
#include <sys/stat.h> //stat
#include <sys/mman.h> //mmap,munmap
#include <assert.h>
#include <string.h>
#include <iostream>

#include "buffer.h"

class Httpresponse
{
public:
    Httpresponse();
    ~Httpresponse();
    void init(const std::string srcDir, const std::string path, bool isKeepAlive = false, int code = -1);
    bool makeResponse(Buffer &buffer);
    void _unmapFile();
    char *file() const;
    size_t fileLen() const;
    void errorContent(Buffer &buffer, std::string message);
    int _code() const;

private:
    // 填写首部字段
    void addStateLine(Buffer &buffer);
    void addHeader(Buffer &buffer);
    void addContent(Buffer &buffer);

    void errorHTML();
    std::string getFileType();

    int m_code;
    bool m_keepalive;
    std::string m_path;
    std::string m_srcDir;

    char *m_mmfile;
    struct stat mmFileState;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;
};

#endif