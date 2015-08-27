#ifndef IMCLIENT_H
#define IMCLIENT_H
#include "tcpClient.h"
#include <string>
#include <vector>

class imClient : public tcpClient 
{
    public:
        imClient(int port, std::string ip_addr, std::string user_name):
            tcpClient(port, ip_addr), _user_name(user_name) { }

        bool login();
        bool logout();

        void parse(std::string);
        void do_handle();

        int get_user_id(std::string);
        std::vector<std::string> get_all_users();
        void print_active_users(const std::vector<std::string>&);

        void send_message(std::string, std::string);
    private:
        void _send_message(unsigned int, std::string);

        std::string _user_name;
        unsigned int _user_id;
};

#endif
