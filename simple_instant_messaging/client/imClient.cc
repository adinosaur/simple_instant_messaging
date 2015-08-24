#include "imClient.h"
#include "../dimp/dimp.h"
#include "string.h"
#include <unistd.h>
#include <sys/select.h>
#include <stdio.h>
#include <string>
#include <iostream>

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

            int nready = select(_confd + 1, &rset, NULL, NULL, NULL);
            if (FD_ISSET(0, &rset)) 
            {
                cin >> content;
                _send_message(0, content);
            }
            if (FD_ISSET(_confd, &rset)) 
            {
                unsigned char recv_buf[BUFSIZE];
                int recv_bytes;
                if((recv_bytes = read(_confd, recv_buf, sizeof(recv_buf))) < 0)
                    cerr << "[ERROR] read error...\n";
                DimpPackage response(recv_buf);
                if (response.get_status() == DimpPackage::DIMP_STATUS_DATA)
                    cout << response.get_body() << endl;
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
    size_t recv_bytes;

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
    size_t recv_bytes;

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

int imClient::get_user_id(string user_name) 
{
    unsigned char send_buf[BUFSIZE];
    unsigned char recv_buf[BUFSIZE];
    size_t recv_bytes;

    DimpPackage request;
    request.set_name("DIMP");                               //设置协议名
    request.set_version(1);                                 //设置协议版本
    request.set_status(DimpPackage::DIMP_STATUS_CHECK_UID); //设置状态
    request.get_all(send_buf);

    write(_confd, send_buf, request.get_package_len());
    recv_bytes = read(_confd, recv_buf, sizeof(recv_buf));

    DimpPackage response(recv_buf);
    if (response.get_status() != DimpPackage::DIMP_STATUS_ERROR)
        return -1;
    else
        return stoul(response.get_body());
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
    size_t recv_bytes;

    DimpPackage request;
    request.set_name("DIMP");                               //设置协议名
    request.set_version(1);                                 //设置协议版本
    request.set_status(DimpPackage::DIMP_STATUS_DATA);      //设置状态
    request.set_from(_user_id);                             //设置发送者id
    request.set_to(uid);                                    //设置接收者id
    request.set_body(msg);
    request.get_all(send_buf);

    write(_confd, send_buf, request.get_package_len());
    recv_bytes = read(_confd, recv_buf, sizeof(recv_buf));

    DimpPackage response(recv_buf);
    if (response.get_status() == DimpPackage::DIMP_STATUS_ERROR)
        cerr << "[ERROR] send error...\n";
}
