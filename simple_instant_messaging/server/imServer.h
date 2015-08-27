#ifndef IMSERVER_H
#define IMSERVER_H
#include "tcpServer.h"
#include "../dimp/dimp.h"
#include <unordered_map>
#include <iostream>
#include <random>
#include <string>

class IMServer : public TcpServer 
{
    public:
        IMServer(int port):TcpServer(port) {}
        bool do_handle(int);
        
        DimpPackage handle_request(DimpPackage, int);
        void send_message(unsigned int, unsigned int, std::string);

        bool add_user(std::string, int);
        bool del_user(unsigned int);

        bool has_user(std::string);
        bool has_id(unsigned int);
        bool has_confd(unsigned int);

        unsigned int get_confd_from_username(std::string);
        unsigned int get_id_from_username(std::string);
        
        unsigned int get_confd_from_id(unsigned int);
        std::string get_username_from_id(unsigned int);
        
        unsigned int get_id_from_confd(unsigned int);
        std::string get_username_from_confd(unsigned int);
        
        void print_all_user(std::ostream&);
    private:
        void _send_message(unsigned int, unsigned int, std::string);
        
        std::default_random_engine dre;
        std::unordered_map<std::string, unsigned int> _user_to_id;
        std::unordered_map<unsigned int, unsigned int> _id_to_confd;
        std::unordered_map<unsigned int, std::string> _confd_to_user;
};


#endif
