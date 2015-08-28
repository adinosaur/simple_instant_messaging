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
                int recv_bytes = read(_confd, recv_buf, sizeof(recv_buf));
                if(recv_bytes < 0) 
                {
                    cerr << "[ERROR] read error...\n";
                    return;
                }
                else if (recv_bytes == 0) 
                {
                    cerr << "[ERROR] IM server close...\n";
                    return;
                }
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

bool imClient::login() 
{
    unsigned char send_buf[BUFSIZE];
    unsigned char recv_buf[BUFSIZE];
    ssize_t recv_bytes;

    DimpPackage request;
    request.set_name("DIMP");                           //设置协议名
    request.set_version(1);                             //设置协议版本
    request.set_status(DimpPackage::DIMP_STATUS_LOGIN); //设置状态
    request.set_body(_user_name);                       //设置body为用户名
    request.get_all(send_buf);

    write(_confd, send_buf, request.get_package_len());
    recv_bytes = read(_confd, recv_buf, sizeof(recv_buf));

    DimpPackage response(recv_buf);
    if (response.get_status() == DimpPackage::DIMP_STATUS_ERROR)
        return false;
    else 
    {
        _user_id = stoul(response.get_body());
        return true;
    }
}

bool imClient::logout() 
{
    unsigned char send_buf[BUFSIZE];
    unsigned char recv_buf[BUFSIZE];
    ssize_t recv_bytes;

    DimpPackage request;
    request.set_name("DIMP");                           //设置协议名
    request.set_version(1);                             //设置协议版本
    request.set_status(DimpPackage::DIMP_STATUS_LOGOUT);//设置状态
    request.set_from(_user_id);                         //设置发送者id
    request.get_all(send_buf);

    write(_confd, send_buf, request.get_package_len());
    recv_bytes = read(_confd, recv_buf, sizeof(recv_buf));

    DimpPackage response(recv_buf);
    if (response.get_status() != DimpPackage::DIMP_STATUS_ERROR)
        return false;
    else
        return true;
}

void imClient::parse(std::string content)
{
    static int id = 0;
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

int imClient::get_user_id(string user_name) 
{
    unsigned char send_buf[BUFSIZE];
    unsigned char recv_buf[BUFSIZE];
    ssize_t recv_bytes;

    DimpPackage request;
    request.set_name("DIMP");                               //设置协议名
    request.set_version(1);                                 //设置协议版本
    request.set_status(DimpPackage::DIMP_STATUS_CHECK_UID); //设置状态
    request.set_body(user_name);                            //设置查询用户名
    request.get_all(send_buf);

    write(_confd, send_buf, request.get_package_len());
    recv_bytes = read(_confd, recv_buf, sizeof(recv_buf));

    DimpPackage response(recv_buf);
    //response.print();
    //if (response.get_status() == DimpPackage::DIMP_STATUS_ERROR)
    //    return stoul(response.get_body());
    //else
    return stoul(response.get_body());
}

vector<string> imClient::get_all_users()
{
    unsigned char send_buf[BUFSIZE];
    unsigned char recv_buf[BUFSIZE];
    const int USER_NAME_BUF_LEN = 32;
    char user_name_buf[USER_NAME_BUF_LEN];
    vector<string> users_list;
    size_t recv_bytes;

    DimpPackage request;
    request.set_name("DIMP");                                   //设置协议名
    request.set_version(1);                                     //设置协议版本
    request.set_status(DimpPackage::DIMP_STATUS_GET_ALL_USERS); //设置状态
    request.get_all(send_buf);

    write(_confd, send_buf, request.get_package_len());
    recv_bytes = read(_confd, recv_buf, sizeof(recv_buf));

    DimpPackage response(recv_buf);
    if (response.get_status() == DimpPackage::DIMP_STATUS_ERROR)
        return std::move(users_list);
    else
    {
        stringstream ss(response.get_body());
        while (ss.getline(user_name_buf, USER_NAME_BUF_LEN))
            users_list.push_back(user_name_buf);
        return std::move(users_list);
    }
}

void imClient::send_message(std::string user_name, std::string msg) 
{
    int uid;

    if ((uid = get_user_id(user_name) == -1)) 
    {
        cerr << "[ERROR] wrong user name\n";
        return;
    } 
    else 
    {
        _send_message(uid, msg);
    }
}

void imClient::_send_message(unsigned int uid, std::string msg) 
{
    unsigned char send_buf[BUFSIZE];
    unsigned char recv_buf[BUFSIZE];
    int recv_bytes;

    DimpPackage request;
    request.set_name("DIMP");                               //设置协议名
    request.set_version(1);                                 //设置协议版本
    request.set_status(DimpPackage::DIMP_STATUS_DATA);      //设置状态
    request.set_from(_user_id);                             //设置发送者id
    request.set_to(uid);                                    //设置接收者id
    request.set_body(msg);
    request.get_all(send_buf);

    if(write(_confd, send_buf, request.get_package_len()) < 0)
    {
        cerr << "[ERROR] write error\n";
        return;
    }
    
    if ((recv_bytes = read(_confd, recv_buf, sizeof(recv_buf))) < 0)
    {
        cerr << "[ERROR] read error\n";
        return;
    }

    DimpPackage response(recv_buf);
    if (response.get_status() == DimpPackage::DIMP_STATUS_ERROR)
        cerr << "[ERROR] send error...\n";
}
