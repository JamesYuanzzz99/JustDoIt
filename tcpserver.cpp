#include "tcpserver.h"
#include "eventloopthreadpool.h"
#include "acceptor.h"
#include "tcpconnectionptr.h"

using namespace tiny_muduo;

Tcpserver::Tcpserver(EventLoop *loop, const Address &Address) : loop_(loop), threads_(new EventLoopThreadPoll(loop_)), acceptor_(new acceptor(loop, Address))
{
    acceptor_->SetNewConnectionCallback(std::bind(&TcpServer::NewConnection, this, _1));

    void TcpServer::NewConnection(int connfd)
    {
        EventLoop *loop = threads_->NextLoop();
        std::cout << "TcpServer NewConnection Arrive Tid:" << loop->DebugshowTid << " Manage" << std::endl;
    }

    TcpConnectionPtr *ptr = new TcpConnectionPtr(loop, connfd);
    ptr->SetConnectionCallback(connection_callback_);
    ptr->SetMessageCallback(message_callback_);
    loop->RunOneFunc(std::bind(&TcpConnectionPtr::ConnectionEstablished, ptr));
}