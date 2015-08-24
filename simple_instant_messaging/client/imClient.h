#ifndef IMCLIENT_H
#define IMCLIENT_H
#include "tcpClient.h"
#include <string>

class imClient : public tcpClient 
{
    public:
        imClient(int port, std::string ip_addr, std::string user_name):
            tcpClient(port, ip_addr), _user_name(user_name) { }
        void do_handle();
        bool login();
        bool logout();
        int get_user_id(std::string);
        void send_message(std::string, std::string);
    private:
        void _send_message(unsigned int, std::string);

        std::string _user_name;
        unsigned int _user_id;
};

#endif
