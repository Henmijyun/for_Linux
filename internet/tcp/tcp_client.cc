
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>



void usage(std::string proc)
{
    std::cout << "Usage: " << proc << " serverIp serverPort\n" << std::endl; 
}

// ./tcp_client targetIp targetPort
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "sock error" << std::endl;
        exit(2);
    }

    // client 不需要显示bing，系统自动port选择 
    // 连接服务器
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        std::cerr << "connect error" << std::endl;
        exit (3);
    }
    std::cout << "connect success" << std::endl;

    while (true)
    {
        std::string line;
        std::cout << "请输入# ";
        std::getline(std::cin, line);
        send(sock, line.c_str(), line.size(), 0);   // 写到服务器

        char buffer[1024];
        size_t s = recv(sock, buffer, sizeof(buffer)-1, 0);   // 从服务器读
        if (s > 0)
        {
            buffer[s] = 0;
            std::cout << "server 回显# " << buffer << std::endl;
        }
        else if (s == 0)
        {
            // 对方服务器关闭连接
            break;
        }
        else
        {
            // 读取失败
            break;
        }
    }
    close(sock);
    
    return 0;
}