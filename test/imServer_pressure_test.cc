#include <string>
#include <random>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/dimp/dimp.h"
#include "../src/client/imClient.h"

const unsigned int BUFSIZE = 128;
const unsigned int FORK_SIZE = 10;
const unsigned short PORT = 6666;
const char* IP = "127.0.0.1";

int main()
{
    std::default_random_engine dre;
    for (int i = 0; i != FORK_SIZE; ++i)
    {
        unsigned int r = dre();
        pid_t pid = fork();
        if (pid == 0)
        {
            imClient im_client(PORT, IP, std::to_string(r));
            DimpPackage response = im_client._send_message(0, "test message...\n");
            while (1)
            {
                unsigned char buf[BUFSIZE];
                ssize_t read_bytes = im_client.read_from_socket(buf, sizeof(buf));
                write(1, buf, read_bytes);
            }
            exit(0);
        }
        else if (pid > 0)
        {
            std::cout << "fork a process pid: " << pid << std::endl;
            //wait(NULL);
        }
    }
    return 0;
}
