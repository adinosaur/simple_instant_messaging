#include "gtest/gtest.h"
#include "../include/dimp/dimp.h"

unsigned char dimp_package[] = {
        'D', 'I', 'M', 'P',     //protocal name
        0x01, 0x00, 0x00, 0x00, //protocal version
        0x03, 0x00,             //protocal status
        0x10, 0x00, 0x00, 0x00, //from id
        0x20, 0x00, 0x00, 0x00, //to id
        0x05, 0x00,             //dtat length
        'h', 'e', 'l', 'l', 'o' //body
};

TEST(DIMP, GET_NAME) 
{
    DimpPackage dimp(dimp_package);
    EXPECT_EQ("DIMP", dimp.get_name());
}

TEST(DIMP, SET_NAME) 
{
    DimpPackage dimp;
    dimp.set_name("DIMP");
    EXPECT_EQ("DIMP", dimp.get_name());
}

TEST(DIMP, GET_VERSION) 
{
    DimpPackage dimp(dimp_package);
    EXPECT_EQ(1, dimp.get_version());
}

TEST(DIMP, SET_VERSION) 
{
    DimpPackage dimp;
    dimp.set_version(1);
    EXPECT_EQ(1, dimp.get_version());
    dimp.set_version(2);
    EXPECT_EQ(2, dimp.get_version());
}

TEST(DIMP, GET_STATUS) 
{
    DimpPackage dimp(dimp_package);
    EXPECT_EQ(3, dimp.get_status());
}

TEST(DIMP, SET_STATUS) 
{
    DimpPackage dimp;
    dimp.set_status(256);
    EXPECT_EQ(256, dimp.get_status());
}

TEST(DIMP, GET_FROM) 
{
    DimpPackage dimp(dimp_package);
    EXPECT_EQ(16, dimp.get_from());
}

TEST(DIMP, SET_FROM) 
{
    DimpPackage dimp;
    dimp.set_from(255);
    EXPECT_EQ(255, dimp.get_from());
}

TEST(DIMP, GET_TO) 
{
    DimpPackage dimp(dimp_package);
    EXPECT_EQ(32, dimp.get_to());
}

TEST(DIMP, SET_TO) 
{
    DimpPackage dimp;
    dimp.set_to(128);
    EXPECT_EQ(128, dimp.get_to());
}

TEST(DIMP, GET_DATALEN) 
{
    DimpPackage dimp(dimp_package);
    EXPECT_EQ(5, dimp.get_data_len());
}

TEST(DIMP, SET_DATALEN) 
{
    DimpPackage dimp;
    dimp.set_data_len(18);
    EXPECT_EQ(18, dimp.get_data_len());
}

TEST(DIMP, GET_BODY) 
{
    DimpPackage dimp(dimp_package);
    EXPECT_EQ("hello", dimp.get_body());
}

TEST(DIMP, SET_BODY) 
{
    DimpPackage dimp;
    dimp.set_body("world");
    EXPECT_EQ("world", dimp.get_body());
    EXPECT_EQ(5, dimp.get_data_len());
}

TEST(DIMP, GET_REQUEST) 
{
    unsigned char buf[1024];
    DimpPackage request;
    request.set_name("DIMP");                           //设置协议名
    request.set_version(1);                             //设置协议版本
    request.set_status(DimpPackage::DIMP_STATUS_LOGIN); //设置状态
    request.set_from(16);                               //设置发送者id
    request.set_to(32);                                 //设置接受者id
    request.set_body("hello");                          //设置body为用户名
    
    request.get_all(buf);
    bool flag = true;
    for (int i = 0; i != sizeof(dimp_package); ++i) {
        if (buf[i] != dimp_package[i]) {
            flag = false;
            break;
        }
    }
    EXPECT_TRUE(flag);
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
