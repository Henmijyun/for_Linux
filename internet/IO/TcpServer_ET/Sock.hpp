// 套接字对象

#ifndef __TCP_SERVER_HPP__
#define __TCP_SERVER_HPP__

#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <cassert>
#include <memory>
#include <unordered_map>

#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h> 

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>


class Sock
{
public:
    const static int gbacklog = 10; 

public:
    Sock()
    {}
    
    // 创建套接字sock  -- 进程和文件
    static int Socket()
    {
        int listensock = socket(AF_INET, SOCK_STREAM, 0);   // 创建套接字
        if (listensock < 0)
        {
            exit(2);
        }

        // 地址复用，让对应的port、ip 在TIME_WAIT状态时，也可以立马重新启动
        int opt = 1;
        setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        return listensock;
    }

    // 绑定bind  -- 文件+网络
    static void Bind(int sock, uint16_t port, std::string ip = "0.0.0.0")
    {    
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;      // 填充第一个字段为AF_INET
        local.sin_port = htons(port);   // 主机序列 转 网络序列
        inet_pton(AF_INET, ip.c_str(), &local.sin_addr);  // 点分十进制字符串IP地址 -> 4字节主机序列 -> 网络序列

        if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            // bind失败
            exit(3);
        }
    }

    // 连接，TCP是面向链接的，所以通信前需要先建立链接
    static void Listen(int sock)
    {

        if (listen(sock, gbacklog) < 0) 
        {
            // 切换监听状态失败
            exit(4);
        }

        // 初始化完成
    }

    // 获取链接
    // const std::string&  输入型参数
    // std::string*   输出型参数
    // std::string&   输入输出型参数
    static int Accept(int listensock, std::string* ip, uint16_t* port, int *accept_errno)
    {
        struct sockaddr_in src;
        socklen_t len = sizeof(src);
        *accept_errno = 0;
        int servicesock = accept(listensock, (struct sockaddr*)&src, &len);  // 获取链接（获取服务套接字）
        if (servicesock < 0)
        {
            *accept_errno = errno;
            return -1;
        }

        // 获取连接成功
        if (port)
        {
            *port = ntohs(src.sin_port);       // 网络序列->主机序列
        }
        if (ip)
        {
            *ip = inet_ntoa(src.sin_addr);  // 网络序列->点分十进制字符串  
        }

        return servicesock;
    }

    // 连接
    static bool Connect(int sock, const std::string& server_ip, const uint16_t& server_port)
    {
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(server_port);
        server.sin_addr.s_addr = inet_addr(server_ip.c_str());

        // 连接
        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // 设置为非阻塞模式
    static bool SetNonBlock(int sock)
    {
        int fl = fcntl(sock, F_GETFL);  // 获取标志位
        if (fl < 0)
        {
            return false;
        }
        
        fcntl(sock, F_SETFL, fl | O_NONBLOCK);  // 设置非阻塞属性
        return true;
    }

    ~Sock()
    {}

};



#endif