#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <set>

class TcpServer {
    public:
        TcpServer(int);
        virtual ~TcpServer();
        void run();
        virtual bool do_handle(int);
    protected:
        void bind_and_listen();
        void _bind();
        void _listen();

        int _port;
        int _listen_fd;
        std::set<int> _fd_set;
};

#endif
