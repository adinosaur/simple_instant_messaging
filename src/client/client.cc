#include "../../include/tcpClient/tcpClient.h"
#include "imClient.h"

int port = 6666;
const char* ip_addr = "127.0.0.1";

int main(int ac, char** av)
{
    if (ac != 2)
        return 1;
    else {
        TcpClient* p = new imClient(port, ip_addr, av[1]);
        p->run();
        delete p;
        return 0;
    }
}
