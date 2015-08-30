#include "string.h"
#include <unistd.h>
#include <sys/select.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "imClient.h"
#include "../../include/dimp/dimp.h"

using namespace std;

#define BUFSIZE 4096

void imClient::do_handle() 
{
    string content;

    if (login()) 
    {
        while(1) 
        {
            fd_set rset;
            FD_ZERO(&rset);
            FD_SET(0, &rset);
            FD_SET(_confd, &rset);

            select(_confd + 1, &rset, NULL, NULL, NULL);
            if (FD_ISSET(0, &rset)) 
            {
                getline(cin, content);
                if (!content.empty())
                    parse(content);
                else
                    cin.clear();
            }
            if (FD_ISSET(_confd, &rset)) 
            {
                unsigned char recv_buf[BUFSIZE];
                ssize_t recv_bytes = read_from_socket(recv_buf, sizeof(recv_buf));
                if(recv_bytes <= 0) 
                    return;
                else
                {
                    DimpPackage response(recv_buf);
                    if (response.get_status() == DimpPackage::DIMP_STATUS_DATA)
                        cout << response.get_body() << endl;
                }
            }
        }
        logout();
    }
    else 
    {
        cerr << "[ERROR] user_name already exist: " << _user_name << endl;
    }
}

ssize_t imClient::read_from_socket(unsigned char* buf, size_t count)
{
    ssize_t recv_bytes = read(_confd, buf, count);
    if(recv_bytes < 0) 
        cerr << "[ERROR] read error...\n";
    else if (recv_bytes == 0) 
        cerr << "[ERROR] IM server close...\n";
    return recv_bytes;
}

DimpPackage imClient::get_response(unsigned short status, int to, std::string body)
{
    unsigned char send_buf[BUFSIZE];
    unsigned char recv_buf[BUFSIZE];
    ssize_t recv_bytes;
    
    DimpPackage request;
    request.set_name("DIMP");
    request.set_version(1);
    request.set_status(status);
    request.set_from(_user_id);
    request.set_to(to);
    if (!body.empty())
        request.set_body(body);

    request.get_all(send_buf);
    
    if(write(_confd, send_buf, request.get_package_len()) < 0)
        cerr << "[ERROR] write error\n";
    
    recv_bytes = read_from_socket(recv_buf, sizeof(recv_buf));
    
    DimpPackage response(recv_buf);
    if (response.get_status() == DimpPackage::DIMP_STATUS_ERROR)
        cerr << "[ERROR] reply DIMP_STATUS_ERROR\n";

    return response;
}

bool imClient::login() 
{
    DimpPackage response = get_response(DimpPackage::DIMP_STATUS_LOGIN, 0, _user_name);
    _user_id = stoul(response.get_body());
    return response.get_status() != DimpPackage::DIMP_STATUS_ERROR;
}

bool imClient::logout() 
{
    DimpPackage response = get_response(DimpPackage::DIMP_STATUS_LOGOUT, 0, "");
    return response.get_status() != DimpPackage::DIMP_STATUS_ERROR;
}

void imClient::parse(std::string content)
{
    static unsigned int id = 0;
    vector<string> users_list;
    string user_name;

    switch (content[0])
    {
        case '@':
            user_name = content.substr(1);
            if (user_name == "all")
                id = 0;
            else
            {
                id = get_user_id(user_name);
                if (id == 0) 
                    cout << "[INFO] user: " << user_name << " does not online." << endl;
                else
                    cout << ">>> send message to " << user_name << endl;
            }
            break;
        
        case '*':
            users_list = get_all_users();
            print_active_users(users_list);
            break;
        
        default:
            _send_message(id, content);
    }
}

void imClient::print_active_users(const vector<string>& users_list)
{
    // first find the longest name in username lists
    // then according to the longest calculate split_line length
    // ( len(split_line) = longest_username_len + 4 )
    // finally print each username in username list.
    //
    //  result like that:
    //      +------------------+
    //      | all active users |
    //      +------------------+
    //      | user_name_1      |
    //      +------------------+
    //      | user_name_2      |
    //      +------------------+
    //      | user_name_3      |
    //      +------------------+
    //      | user_name_4      |
    //      +------------------+


    const char* title = "all active users";
    string::size_type longest_username_len = strlen(title);

    // find the length of the longest username in list.
    for_each(users_list.begin(), users_list.end(), 
        [&longest_username_len](const string& un){longest_username_len = std::max(longest_username_len, un.size());});
    
    // making the split line. (for example: +----...----+)
    string split_line;
    split_line.push_back('+');
    for (int i = 0; i != longest_username_len+3; ++i)
        split_line.push_back('-');
    split_line.push_back('+');

    // print title.
    cout << split_line << endl;
    cout << "| " << title;
        for (int i = strlen(title) + 2; i < split_line.size() - 1; ++i)
            cout << " ";
    cout << "|" << endl;
    cout << split_line << endl;
    
    // print each username in lists.
    for (auto user : users_list)
    {
        cout << "| " << user;
        for (int i = user.size() + 2; i < split_line.size() - 1; ++i)
            cout << " ";
        cout << "|" << endl;
        cout << split_line << endl;
    }
}

unsigned int imClient::get_user_id(string user_name) 
{
    DimpPackage response = get_response(DimpPackage::DIMP_STATUS_CHECK_UID, 0, user_name);
    return stoul(response.get_body());
}

vector<string> imClient::get_all_users()
{
    const int USER_NAME_BUF_LEN = 32;
    char user_name_buf[USER_NAME_BUF_LEN];
    vector<string> users_list;
    
    DimpPackage response = get_response(DimpPackage::DIMP_STATUS_GET_ALL_USERS, 0, "");
    if (response.get_status() != DimpPackage::DIMP_STATUS_ERROR)
    {
        stringstream ss(response.get_body());
        while (ss.getline(user_name_buf, USER_NAME_BUF_LEN))
            users_list.push_back(user_name_buf);
        return std::move(users_list);
    }
    return std::move(users_list);
}

DimpPackage imClient::_send_message(unsigned int uid, std::string msg) 
{
    return get_response(DimpPackage::DIMP_STATUS_DATA, uid, msg);
}
