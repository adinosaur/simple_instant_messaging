#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <string>

class tcpClient 
{
    public:
        tcpClient(int, std::string);
        ~tcpClient();
        void run();
        virtual void do_handle();
    protected:
        void _connect();
        int _port;
        int _confd;
        std::string _ip_addr;
};

#endif
