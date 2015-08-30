#include "gtest/gtest.h"
#include "../src/client/imClient.h"

const unsigned short port = 6666;
const char* ip = "127.0.0.1";

TEST(IMCLIENT, LOGIN)
{
    // new user 'dinosaur' login 
    // should return true.
    imClient im_client1(port, ip, "dinosaur");
    EXPECT_TRUE(im_client1.login());
    
    // new user 'wumanshuo' login
    // should return true.
    imClient im_client2(port, ip, "wumanshuo");
    EXPECT_TRUE(im_client2.login());

    // 'dinosaur' have already online
    // login should return false.
    imClient im_client3(port, ip, "dinosaur");
    EXPECT_FALSE(im_client3.login());
}

TEST(IMCLIENT, LOGOUT)
{
    imClient im_client1(port, ip, "dinosaur");
    
    // new user 'dinosaur' login
    // should return true.
    EXPECT_TRUE(im_client1.login());
    
    imClient im_client2(port, ip, "dinosaur");
    
    // 'dinosaur' have already online
    // login and logout should both return false.
    EXPECT_FALSE(im_client2.login());
    EXPECT_FALSE(im_client2.logout());

    // user 'dinosaur' is online, logout should return true.
    EXPECT_TRUE(im_client1.logout());

    // after last 'dinosaur' logout, 
    // this time login should be successed. And return true.
    EXPECT_TRUE(im_client2.login());
}

TEST(IMCLIENT, GET_USER_ID)
{
    const int BUFSIZE = 128;
    unsigned char buf[BUFSIZE];
    imClient im_client1(port, ip, "dinosaur");
    im_client1.login();

    imClient im_client2(port, ip, "wumanshuo");
    im_client2.login();

    // im_client1 will recv a string "[INFO] wumanshuo join..."
    im_client1.read_from_socket(buf, sizeof(buf));

    // "wumanshuo" online, get_user_id return non 0.
    EXPECT_NE(0, im_client1.get_user_id("wumanshuo"));

    // "www" does not online, get_user_id return 0.
    EXPECT_EQ(0, im_client1.get_user_id("www"));
}

int main(int ac, char** av)
{
    ::testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
