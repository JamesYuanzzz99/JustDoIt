#ifndef __ECHO_H_
#define __ECHO_H_

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "processpool.h"
#include "tcpserver.h"
#include "tcpconnectionptr.h"

static const int thread_nums = 8;

namespace tiny_muduo
{
    class Address;
    class EventLoop;
} // namespace tiny_muduo

class EchoServer
{
private:
    tiny_muduo::EventLoop *loop;
    tiny_muduo::TcpServer server_;

public:
    EchoServer(tiny_muduo::EventLoop *loop, const tiny_muduo::Address &listen_addr);

    ~EchoServer() {}

    void Start()
    {
        server_Start();
    }
    void ConnectionCallback(tiny_muduo::TcpConnectionPtr *connection_ptr)
    {
        std::cout << "echo_server has a new connection " << std::endl;
    }

    void MessageCallback(tiny_muduo::TcpConnectionPtr *connection_ptr)
    {
        std::string message(connection_ptr->Get());
        std::cout << "echo_server get massage" << std::endl;
        connection_ptr->Send(message);
    }
};

class echo
{
private:
    static const int BUFFER_SIZE = 1024;
    static int epollfd;
    int sockfd;
    sockaddr_in client_addr;
    char buf[1024] = {0};

public:
    echo() {}
    ~echo() {}

    void init(int _epollfd, int _sockfd, const sockaddr_in &address)
    {
        epollfd = _epollfd;
        sockfd = _sockfd;
        client_addr = address;
    }

    void process()
    {
        while (1)
        {
            memset(buf, 0, sizeof(buf));
            int ret = recv(sockfd, buf, sizeof(buf), 0);
            if (ret < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
            }
            else if (ret == 0)
            {
                removefd(epollfd, sockfd);
                break;
            }
            else
            {
                send(sockfd, buf, sizeof(buf), 0);
            }
        }

        return;
    }
};

int echo::epollfd = -1;

#endif
