#include "include/channel.h"
#include <sys/epoll.h>
using namespace tiny_muduo;

Channel::Channel(EventLoop *loop, const int &fd) : loop_(loop), fd_(fd), events_(0), recv_events_(0) {}

void Channel::HandleEvent()
{
    recv_events_ &EPOLLIN ? read_callback : write_callback_();
}