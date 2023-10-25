#include "acceptor.h"
#include <assert.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <bits/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include "address.h"
#include "channel.h"

using namespace tiny_muduo;
namespace
{
    const int kMaxListenNum = 5;
}

Acceptor::Acceptor(EventLoop *loop, const Address &address) : loop_(loop), listenfd_(socket(PF_INET, SOCK_STREAM, 0)), channel_(new Channel(loop, listenfd_))
{
    BindListenFd(address);
    channel_->SetReadCallback(std::bind(&Acceptor::NewConnection, this));
}

void Acceptor::BindListenFd(const Address &addr)
{
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int ret = bind(listenfd,
                   (struct sockaddr *)(&address),
                   sizeof(address));
    assert(ret != -1);
}

void Acceptor::NewConnection()
{
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(listenfd, (struct sockaddr *)(&client), &client_addrlength);
    new_connection_callback_(connfd);
}