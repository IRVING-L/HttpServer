
#include "../lib/http_client_info.h"

// 构造函数和析构函数
ClientInfo::ClientInfo()
{
}
ClientInfo::ClientInfo(char *ip, int port, int cfd)
{
    strcpy(m_IP, ip);
    m_port = port;
    m_cfd = cfd;
}
ClientInfo::ClientInfo(char *ip, int port, int cfd, struct sockaddr_in *addr)
{
    strcpy(m_IP, ip);
    m_port = port;
    m_cfd = cfd;
    m_sockaddr.sin_addr.s_addr = addr->sin_addr.s_addr;
    m_sockaddr.sin_family = addr->sin_family;
    m_sockaddr.sin_port = addr->sin_port;
}
ClientInfo::~ClientInfo()
{
}

// static静态成员的定义
int ClientInfo::m_epollfd = -1;
int ClientInfo::m_userCount = 0;

// 成员函数

void ClientInfo::set(char *ip, int port)
{
    strcpy(m_IP, ip);
    m_port = port;
}

int ClientInfo::getFd()
{
    return m_cfd;
}