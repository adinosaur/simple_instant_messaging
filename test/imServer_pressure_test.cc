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
            im_client._send_message(0, "test message...\n");
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
