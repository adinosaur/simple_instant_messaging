#ifndef TcpClient_H
#define TcpClient_H
#include <string>

class TcpClient 
{
    public:
        TcpClient(int, std::string);
        virtual ~TcpClient();
        void run();
        virtual void do_handle();
    protected:
        void _connect();
        int _port;
        int _confd;
        std::string _ip_addr;
};

#endif
