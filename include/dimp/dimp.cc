#include "dimp.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

DimpPackage::DimpPackage(unsigned char* head) 
{
    _copy(head, _package_head, PACKAGE_HEAD_LEN);
    _copy(head + PACKAGE_HEAD_LEN, _body, get_data_len());
}

DimpPackage::DimpPackage(const DimpPackage& d):_body(d._body) 
{
    _copy(d._package_head, _package_head, PACKAGE_HEAD_LEN);
}

DimpPackage& DimpPackage::operator=(const DimpPackage& d) 
{
    _copy(d._package_head, _package_head, PACKAGE_HEAD_LEN);
    _body = d._body;
        return *this;
}

string DimpPackage::get_name() 
{
    std::string name;
    for (int i = 0; i != DIMP_LEN_PROTOCAL_NAME; ++i)
        name.push_back(_package_head[DIMP_POS_PROTOCAL_NAME + i]);
    return name;
}

bool DimpPackage::check() 
{
    return get_name() == "DIMP" && get_version() == 1;
}

void DimpPackage::set_name(const char* name) 
{
    _copy((const unsigned char*)name, _package_head+DIMP_POS_PROTOCAL_NAME, DIMP_LEN_PROTOCAL_NAME);
}

unsigned int DimpPackage::get_version() 
{
    return *(unsigned int*)(_package_head+DIMP_POS_PROTOCAL_VERSION);
}

void DimpPackage::set_version(unsigned int version) 
{
    _copy((unsigned char*)&version, _package_head+DIMP_POS_PROTOCAL_VERSION, DIMP_LEN_PROTOCAL_VERSION);
}

unsigned short DimpPackage::get_status() 
{
    return *(unsigned short*)(_package_head+DIMP_POS_PROTOCAL_STATUS);
}

void DimpPackage::set_status(unsigned short status) 
{
    _copy((unsigned char*)&status, _package_head+DIMP_POS_PROTOCAL_STATUS, DIMP_LEN_PROTOCAL_STATUS);
}

unsigned int DimpPackage::get_from() 
{
    return *(unsigned int*)(_package_head+DIMP_POS_PROTOCAL_FROM);
}

void DimpPackage::set_from(unsigned int from) 
{
    _copy((unsigned char*)&from, _package_head+DIMP_POS_PROTOCAL_FROM, DIMP_LEN_PROTOCAL_FROM);
}

unsigned int DimpPackage::get_to() 
{
    return *(unsigned int*)(_package_head+DIMP_POS_PROTOCAL_TO);
}

void DimpPackage::set_to(unsigned int to) 
{
    _copy((unsigned char*)&to, _package_head+DIMP_POS_PROTOCAL_TO, DIMP_LEN_PROTOCAL_TO);
}

unsigned short DimpPackage::get_data_len() 
{
    return *(unsigned int*)(_package_head+DIMP_POS_PROTOCAL_DATALEN);
}

void DimpPackage::set_data_len(unsigned int data_len) 
{
    _copy((unsigned char*)&data_len, _package_head+DIMP_POS_PROTOCAL_DATALEN, DIMP_LEN_PROTOCAL_DATALEN);
}

string DimpPackage::get_body() 
{
    std::string body;
    for (auto byte : _body)
        body.push_back(byte);
    return body;
}

void DimpPackage::set_body(std::string body) 
{
    std::vector<unsigned char> tmp;
    _body.swap(tmp);
    for (auto byte : body)
        _body.push_back(byte);
    set_data_len(_body.size());
}

void DimpPackage::get_all(unsigned char* buf) 
{
    _copy(_package_head, buf, PACKAGE_HEAD_LEN);
    buf += PACKAGE_HEAD_LEN;
    for (auto byte : _body)
        *buf++ = byte;
}

int DimpPackage::get_package_len() 
{
    return PACKAGE_HEAD_LEN + get_data_len();
}

void DimpPackage::print() 
{
    std::cout << "protocal name: " << get_name() << std::endl;
    std::cout << "protocal version: " << get_version() << std::endl;
    std::cout << "protocal status: " << get_status() << std::endl;
    std::cout << "protocal from: " << get_from() << std::endl;
    std::cout << "protocal to: " << get_to() << std::endl;
    std::cout << "protocal data length: " << get_data_len() << std::endl;
    std::cout << "protocal body: " << get_body() << std::endl;
}

void DimpPackage::_copy(const unsigned char* sp, unsigned char* dp, int len) 
{
    for (int i = 0; i != len; ++i)
        dp[i] = sp[i];
}

void DimpPackage::_copy(const unsigned char* sp, std::vector<unsigned char>& dp, int len) 
{
    for (int i = 0; i != len; ++i)
        dp.push_back(sp[i]);
}

