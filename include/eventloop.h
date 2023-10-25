#include <stdint.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <pthread.h>

#include <vector>
#include <functional>

#include "mutex.h"
#include "epoller.h"

namespace tiny_muduo
{
    class Epoller;
    class Channel;

    class Eventloop
    {
    private:
        pthread_t tid_;
        Epoller *epoller_;
        int wakeup_fd_;
        Channel *wakeup_channel_;
        Channels active_channels_;
        ToDoList to_do_list_;

        MutexLock mutex_;

    public:
        using std::vector<Channel *> Channels;
        using std::function<void()> BasicFunc;
        using std::vector<BasicFunc> ToDoList;

        Eventloop();
        ~Eventloop();

        bool IsInThreadLoop() { return ::pthread_self() == tid_; }
        void Update(Channel *channel) { epoller_->Update(channel); }

        void Loop();
        void HandleRead();
        void RunOneFunc(const BasiicFunc &func);
        void DoToList();

        pthread_t DebugShowTid() { return tid_; }
    }
} // namespace tiny_muduo
