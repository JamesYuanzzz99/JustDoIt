#include <functional>

namespace tiny_muduo
{
    class EventLoop;
    class Address;
    class Channel;

    class Acceptor
    {
    private:
        EventLoop *loop_;
        int listenfd_;
        Channel *channel_;

        NewConnectionCallback new_connection_callback_;

    public:
        typedef std::function<void(int)> NewConnectionCallback;

        Acceptor(EventLoop *loop, const Address &Address);

        void BindListenFd(const Address &address);
        void Listen();
        void NewConnection();
        void SetNewConnectionCallback(const NewConnectionCallback &callback)
        {
            new_connection_callback_ = callback;
        }
    };
}