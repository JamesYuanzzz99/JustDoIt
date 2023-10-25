#include "callback.h"
#include "eventloop.h"
#include "acceptor.h"
#include "eventloopthreadpool.h"

namespace tiny_muduo
{
    class Address;
    class Tcpserver
    {
    private:
        EventLoop *loop_;
        EventLoopThreadPool *threads_;
        Acceptor *acceptor_;

        ConnectionCallback connection_callback_;
        MessageCallback message_callback_;

        Tcpserver(EchoLoop *loop, const Address address);
        ~Tcpserver();

        void Start()
        {
            threads_->StartLoop();
            loop_->RunOneFunc(std::bind(&Acceptor::Listrn, acceptor_));
        }
        void SetconnectionCallback(const ConnectionCallback &callback)
        {
            connection_callback_ = callback;
        }

        void SetThreadNums(int thread_nums)
        {
            threads_->SetThreadNums(thread_nums);
        }

        void NewConnection(int connfd);
    }
};