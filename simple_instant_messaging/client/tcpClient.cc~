#include "tcpClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string>
#include <iostream>
using namespace std;

tcpClient::tcpClient(int port, string ip_addr):_port(port), _ip_addr(ip_addr) 
{
    _confd = socket(AF_INET, SOCK_STREAM, 0);
}

tcpClient::~tcpClient() 
{
    close(_confd);
}

void tcpClient::_connect() 
{
    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_port = htons(_port);
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, _ip_addr.c_str(), &saddr.sin_addr);
    connect(_confd, (struct sockaddr*)&saddr, sizeof(saddr));
}

void tcpClient::run() 
{
    _connect();
    do_handle();
}

void tcpClient::do_handle() 
{
    cout << "[INFO] tcpClient::do_handle" << endl;
}
