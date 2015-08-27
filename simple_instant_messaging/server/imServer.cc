#include "imServer.h"
#include "../dimp/dimp.h"
#include <unistd.h>
#include <string.h>

using namespace std;

#define BUFSIZE 4096
const char* WRONG_DIMP_REQUEST = "[ERROR] wrong dimp head";

unsigned int IMServer::get_confd_from_username(string user_name) 
{
    return _id_to_confd[_user_to_id[user_name]];
}

unsigned int IMServer::get_id_from_username(string user_name)
{
    return _user_to_id[user_name];
}

unsigned int IMServer::get_confd_from_id(unsigned int id)
{
    return _id_to_confd[id];
}

string IMServer::get_username_from_id(unsigned int id)
{
    return _confd_to_user[_id_to_confd[id]];
}

unsigned int IMServer::get_id_from_confd(unsigned int confd)
{
    return _user_to_id[_confd_to_user[confd]];
}

string IMServer::get_username_from_confd(unsigned int confd)
{
    return _confd_to_user[confd];
}

bool IMServer::has_user(std::string user_name)
{
    return _user_to_id.find(user_name) != _user_to_id.end();
}

bool IMServer::has_id(unsigned int id)
{
    return _id_to_confd.find(id) != _id_to_confd.end();
}

bool IMServer::has_confd(unsigned int confd)
{
    return _confd_to_user.find(confd) != _confd_to_user.end();
}

bool IMServer::do_handle(int confd)
{
    unsigned char send_buf[BUFSIZE];
    unsigned char recv_buf[BUFSIZE];
    DimpPackage request;
    DimpPackage response;
    int read_bytes;
    int user_id = get_id_from_confd(confd);

    read_bytes = read(confd, recv_buf, sizeof(recv_buf));
    if (read_bytes > 0) 
    {
        if (read_bytes >= DimpPackage::PACKAGE_HEAD_LEN) 
        {
            request = DimpPackage(recv_buf);
            response = handle_request(request, confd);
            response.get_all(send_buf);
            write(confd, send_buf, response.get_package_len());
        } 
        else 
        {
            write(confd, WRONG_DIMP_REQUEST, strlen(WRONG_DIMP_REQUEST));
        }
    }
    else if (read_bytes == 0)
    {
        cerr << "[INFO] client close\n";
        del_user(user_id);
    }
    else
    {
        cerr << "[ERROR] read error\n";
        del_user(user_id);
    }
    return read_bytes > 0 && response.get_status() != DimpPackage::DIMP_STATUS_ERROR;
}

bool IMServer::add_user(std::string user_name, int confd)
{
    if (has_user(user_name))
        return false;
    else 
    {
        _user_to_id[user_name] = dre();
        _id_to_confd[_user_to_id[user_name]] = confd;
        _confd_to_user[confd] = user_name;

        send_message(get_id_from_username(user_name), 0, "[INFO] " + user_name + " join...");
        return true;
    }
}

bool IMServer::del_user(unsigned int user_id)
{
    if (!has_id(user_id))
        return false;
    else 
    {
        send_message(user_id, 0, "[INFO] " + get_username_from_id(user_id) + " leave...");

        _user_to_id.erase(_confd_to_user[_id_to_confd[user_id]]);
        _confd_to_user.erase(_id_to_confd[user_id]);
        _id_to_confd.erase(user_id);
        return true;
    }
}

DimpPackage IMServer::handle_request(DimpPackage request, int confd)
{
    DimpPackage response;
    string user_name;
    string message;
    unsigned int user_id;
    unsigned int from_user;
    unsigned int to_user;

    response.set_name("DIMP");
    response.set_version(1);

    if (request.check()) 
    {
        switch (request.get_status()) 
        {
            case DimpPackage::DIMP_STATUS_LOGIN:
                // login
                user_name = request.get_body();
                if (user_name.empty() || !add_user(user_name, confd)) 
                {
                    response.set_status(DimpPackage::DIMP_STATUS_ERROR);
                    cerr << "[ERROR] user_name already exist: " << user_name << endl;
                } 
                else 
                {
                    user_id = get_id_from_username(user_name);
                    response.set_status(DimpPackage::DIMP_STATUS_REPLY);
                    response.set_body(to_string(user_id));
                }
                break;

            case DimpPackage::DIMP_STATUS_LOGOUT:
                // logout
                cout << "[INFO] logout" << endl;
                user_id = request.get_from();
                if (!del_user(user_id))
                    response.set_status(DimpPackage::DIMP_STATUS_ERROR);
                else
                    response.set_status(DimpPackage::DIMP_STATUS_REPLY);
                break;

            case DimpPackage::DIMP_STATUS_CHECK_UID:
                // get user id from user name
                user_name = request.get_body();
                if (has_user(user_name)) 
                {
                    response.set_status(DimpPackage::DIMP_STATUS_REPLY);
                    response.set_body(to_string(get_id_from_username(user_name)));
                } 
                else
                {
                    response.set_status(DimpPackage::DIMP_STATUS_REPLY);
                    //response.set_status(DimpPackage::DIMP_STATUS_ERROR);
                    response.set_body("0");
                }
                break;

            case DimpPackage::DIMP_STATUS_DATA:
                // message
                from_user = request.get_from();
                to_user = request.get_to();
                message = request.get_body();
                message = "[$" + get_username_from_id(from_user) + "]: " + message;
                send_message(from_user, to_user, message);
                response.set_status(DimpPackage::DIMP_STATUS_REPLY);
                break;

            case DimpPackage::DIMP_STATUS_GET_ALL_USERS:
                response.set_status(DimpPackage::DIMP_STATUS_REPLY);
                message.clear();
                for (auto x : _user_to_id)
                    message += (x.first + "\n");
                response.set_body(message);
                break;

            case DimpPackage::DIMP_STATUS_ERROR:
                // 回显
                response = request;
                break;
        }
    }
    else
    {
        response.set_status(DimpPackage::DIMP_STATUS_ERROR);
        response.set_data_len(0);
    }
    return response;
}

void IMServer::print_all_user(ostream& os)
{
    for (auto x : _user_to_id)
        os << "user: " << x.first 
        << ", id: " << x.second 
        << ", confd: " << get_confd_from_id(x.second) << endl;
}

void IMServer::send_message(unsigned int from, unsigned int to, std::string msg)
{
    if (to == 0) 
    {
        for (auto x : _id_to_confd)
            if (x.first != from)
                _send_message(from, x.first, msg);
    }
    else
        _send_message(from, to, msg);
}

void IMServer::_send_message(unsigned int from, unsigned int to, std::string msg)
{
    unsigned char send_buf[BUFSIZE];

    DimpPackage response;
    response.set_name("DIMP");
    response.set_version(1);
    response.set_status(DimpPackage::DIMP_STATUS_DATA);
    response.set_from(from);
    response.set_to(to);
    response.set_body(msg);
    response.get_all(send_buf);

    write(get_confd_from_id(to), send_buf, response.get_package_len());
}
