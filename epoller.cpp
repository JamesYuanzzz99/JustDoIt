#include "epoller.h"

#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include "channel.h"

using namespace tiny_muduo;

Epoller::Epoller()
    : epollfd_(epoll_create(kMaxEvents)),
      events_(kMaxEvents) {}

void Epoller::Poll(Channels &channels)
{
    int eventnums = EpollWait();
    FillActiveChannels(eventnums, channels);
}
