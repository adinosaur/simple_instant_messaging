#include "imServer.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <sys/select.h>
#include <iostream>

using namespace std;

TcpServer::TcpServer(int port):_port(port) 
{
    _listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    _fd_set.insert(_listen_fd);
}

TcpServer::~TcpServer() 
{
    close(_listen_fd);
}

void TcpServer::bind_and_listen() 
{
    _bind();
    int so_reuseaddr = 1;
    setsockopt(_listen_fd ,SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));
    _listen();
}

void TcpServer::_bind() 
{
    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_port = htons(_port);
    saddr.sin_family = AF_INET;
    if (bind(_listen_fd, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
        cerr << "[ERROR] bind error\n";
}

void TcpServer::_listen() 
{
    if (listen(_listen_fd, 1) != 0)
        cerr << "[ERROR] listen error\n";
}

void TcpServer::run() 
{
    char client_addr[32];
    struct sockaddr_in saddr;
    socklen_t len = sizeof(saddr);
    bind_and_listen();
    
    while (1) 
    {
        fd_set rset;
        FD_ZERO(&rset);
        for (auto fd : _fd_set)
            FD_SET(fd, &rset);
        int max_fd_puls_1 = *(--_fd_set.end()) + 1;
        int nready = select(max_fd_puls_1, &rset, NULL, NULL, NULL);
        
        if (FD_ISSET(_listen_fd, &rset)) 
        {
            int confd = accept(_listen_fd, (struct sockaddr*)&saddr, &len);
            _fd_set.insert(confd);
            inet_ntop(AF_INET, &saddr.sin_addr, client_addr, sizeof(client_addr));
            cout << "[INFO] >>> connection start";
            cout << ", from " << client_addr;
            cout << ", port " << ntohs(saddr.sin_port);
            cout << ", fd:" << confd << endl;
        }
        for (auto confd : _fd_set) 
        {
            if (confd == _listen_fd)
                continue;
            if (FD_ISSET(confd, &rset)) 
            {
                if (!do_handle(confd)) 
                {
                    cout << "[INFO] <<< connection close, fd:" << confd << endl;
                    _fd_set.erase(confd);
                    close(confd);
                }
            }
        }
    }
}

bool TcpServer::do_handle(int fd) 
{
    cout << "[INFO] TcpServer::do_handle" << endl;
    return true;
}
