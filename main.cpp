#include "include/echo.h"
#include "include/processpool.h"
int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        std::cout << "Something is Wrong";
        return 0;
        }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 1);

    

   

    ret = listen(listenfd, 5);
    assert(ret != -1);

    processpool<echo> *pool = processpool<echo>::create(listenfd, 8);
    pool->run();

    close(listenfd);

    return 0;
}
