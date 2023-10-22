#ifndef __PROCESSPOLL_H_
#define __PROCESSPOLL_H_
#include <iostream>
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
#define MAX_EVENTS_NUMBER = 10;
#define MAX_USER_PER_PROCESS = 10000;
class process
{
public:
    int pid;
    int pipe[2];

    process() : pid(-1), pipe{0, 0} {}
    ~process() { delete pipe; }
};

template <class T>
class processpool
{
private:
    int idx;
    int listenfd;
    int epollfd;
    int max_processes_num;
    process *sub_processes;
    static processpool<T> *instance;

    processpool(int listenfd, int max_processes_num = 6)
    {
    }
    ~processpool()
    {
        delete[] sub_processes;
    }

public:
    static processpool<T> *create(int listenfd, int _max_processes_num = 8)
    {
        if (instance == nullptr)
        {
            instance = new processpool<T>(listenfd, _max_processes_num);
            return instance;
        }

        return instance;
    }

    void run();
    void run_parent();
    void run_child();
    void setup_up_sig();
};
template <class T>
processpool<T> *processpool<T>::instance = nullptr;

template <class T>
processpool<T>::processpool(int listenfd, int _max_processes_num) : idx(-1), listenfd(listenfd), epollfd(0), (max_processes_num(_max_processes_num)), sub_processes(nullptr)
{
    sub_processes = new process[max_processes_num];

    for (int i = 0; i < max_processes_num; i++)
    {
        socketpair(PF_UNIX, SOCK_STREAM, 0, sub_processes[i].pipe);
        sub_processes[i].pid = fork();

        if (sub_processes[i].pid > 0)
        {
            close(sub_processes[i].pipe[1]);
            continue;
        }
        else
        {
            close(sub_processes[i].pipe[0]);
            idx = i;
            break;
        }
    }
}

static int set_non_blocking(int fd)
{
    int old_state = fcntl(fd, F_GETFL);
    int new_state = old_state | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_state);

    return old_state;
}

static void addfd(int epollfd, int fd)
{
    epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    set_non_blocking(fd);
}

static void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr);
    close(fd);
}

template <typename T>
void processpool<T>::run()
{
    idx == -1 ? run_parent() : run_child();
}

template <typename T>
void processpool<T>::setup_up_sig()
{
    epollfd = epoll_create(5);
    assert(epollfd != -1);
}

template <class T>
void processpool<T>::run_parent()
{
    epoll_event events[MAX_EVENTS_NUMBER];
    setup_up_sig();

    int pre_idx = 0;
    int has_new_cli = 1;
    int number = 0;

    while (true)
    {
        number = epoll_wait(epollfd, events, MAX_EVENTS_NUMBER, -1);

        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd)
            {
                int pos = pre_idx;
                do
                {
                    pos = (pos + 1) % max_processes_num; // 看不懂
                } while (sub_processes[pos].pid == -1);  // 看不懂
                pre_idx = pos;                           // 看不懂

                send(sub_processes[pos].pipe[0], (void *)&has_new_cli,
                     sizeof(has_new_cli), 0);
                std::cout << "parent processes has sent msg to " << pos << " child" << std::endl;
            }
        }
    }
}

template <typename T>
void processpool<T>::run_child()
{
    epoll_event events[MAX_EVENTS_NUMBER];
    setup_up_sig();

    int pipefd = sub_processes[idx].pipe[1];
    addfd(epollfd, pipefd);
    T *users = new T[MAX_USER_PER_PROCESS];

    int number = 0;
    while (1)
    {
        number = epoll_wait(epollfd, events, MAX_EVENTS_NUMBER, -1);
        for (int i = 0; i < number; ++i)
        {
            int sockfd = events[i].data.fd;
            if (sockfd == pipefd && (events[i].events & EPOLLIN))
            {
                struct sockaddr_in client;
                socklen_t client_addrlength = sizeof(client);
                int connfd = accept(listenfd, (struct sockaddr *)(&client),
                                    &client_addrlength);
                addfd(epollfd, connfd);
                users[connfd].init(epollfd, connfd, client);
                std::cout << "child " << idx << " is addfding" << std::endl;
                continue;
            }
            else if (events[i].events & EPOLLIN)
            {
                std::cout << "child " << idx << " has recv msg" << std::endl;
                users[sockfd].process();
            }
        }
    }

    delete[] users;
    users = nullptr;

    close(epollfd);
    close(pipefd);
}
#endif
