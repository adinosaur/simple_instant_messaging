#ifndef DIMP_PACKAGE_H
#define DIMP_PACKAGE_H

//  Dinosaur IM Protocal
//
//  数据包结构如下
//
//   4bytes  4bytes   2bytes
//  +--------+---------+--------+
//  | DIMP   | version | status |
//  +--------+---------+--------+
//  | from_id| to_id   | datalen|
//  +--------+---------+--------+
//  |          DTAT             |
//  |                           |
//  +---------------------------+
//
//  注册消息包
//   4bytes  4bytes   2bytes
//  +--------+---------+--------+
//  | DIMP   | version | login  |
//  +--------+---------+--------+
//  |   /    |   /     | datalen|
//  +--------+---------+--------+
//  |        UserName           |
//  |                           |
//  +---------------------------+
//  回应注册消息包
//   4bytes  4bytes   2bytes
//  +--------+---------+--------+
//  | DIMP   | version | login  |
//  +--------+---------+--------+
//  |   /    |   /     | datalen|
//  +--------+---------+--------+
//  |        User ID            |
//  |                           |
//  +---------------------------+
//
//  新用户加入IM网络之前先进行注册，注册时提供一个用户名给IM服务器
//  IM服务器在检查没有用户名没有重复后返回一个唯一的User ID。
//  否则IM服务器返回一个数据包，其状态字段表示出错
//
//


#include <vector>
#include <string>
#include <iostream>

class DimpPackage 
{
    public:
        static const int PACKAGE_HEAD_LEN = 20;
        
        static const unsigned short DIMP_STATUS_DATA = 1;
        static const unsigned short DIMP_STATUS_ERROR = 2;
        static const unsigned short DIMP_STATUS_LOGIN = 3;
        static const unsigned short DIMP_STATUS_LOGOUT = 4;
        static const unsigned short DIMP_STATUS_REPLY = 5;
        static const unsigned short DIMP_STATUS_CHECK_UID = 6;
        
        DimpPackage() = default;
        explicit DimpPackage(unsigned char* head);
        
        DimpPackage(const DimpPackage& d);
        DimpPackage& operator=(const DimpPackage& d);
        
        bool check();
        std::string get_name();
        void set_name(const char*);
        
        unsigned int get_version();
        void set_version(unsigned int);
        
        unsigned short get_status();
        void set_status(unsigned short);
        
        unsigned int get_from();
        void set_from(unsigned int);
        unsigned int get_to();
        void set_to(unsigned int);
        
        unsigned short get_data_len();
        void set_data_len(unsigned int);
        
        std::string get_body();
        void set_body(std::string);
        
        void get_all(unsigned char*);
        int get_package_len();
        void print();

    private:
        void _copy(const unsigned char* sp, unsigned char* dp, int len);
        void _copy(const unsigned char* sp, std::vector<unsigned char>& dp, int len);
        
        unsigned char _package_head[PACKAGE_HEAD_LEN] = {0x00};
        std::vector<unsigned char> _body;
        
        static const int DIMP_POS_PROTOCAL_NAME = 0;
        static const int DIMP_POS_PROTOCAL_VERSION = 4;
        static const int DIMP_POS_PROTOCAL_STATUS = 8;
        static const int DIMP_POS_PROTOCAL_FROM = 10;
        static const int DIMP_POS_PROTOCAL_TO = 14;
        static const int DIMP_POS_PROTOCAL_DATALEN = 18;

        static const int DIMP_LEN_PROTOCAL_NAME = 4;
        static const int DIMP_LEN_PROTOCAL_VERSION = 4;
        static const int DIMP_LEN_PROTOCAL_STATUS = 2;
        static const int DIMP_LEN_PROTOCAL_FROM = 4;
        static const int DIMP_LEN_PROTOCAL_TO = 4;
        static const int DIMP_LEN_PROTOCAL_DATALEN = 2;

};

#endif
