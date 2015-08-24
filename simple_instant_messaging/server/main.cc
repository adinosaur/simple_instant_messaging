#include "imServer.h"

int port = 6666;

int main()
{
    TcpServer* p = new IMServer(port);
    p->run();
    delete p;
    return 0;
}
