# dinosaur simple im system makefile.

CC = clang++-3.6

all: ./build/client.o ./build/server.o ./build/tcpClient.o ./build/tcpServer.o ./build/imClient.o
	$(CC) --std=c++11 -Wall -g ./build/tcpClient.o ./build/imClient.o ./build/dimp.o ./build/client.o -o ./build/client.out
	$(CC) --std=c++11 -Wall -g ./build/tcpServer.o ./build/imServer.o ./build/dimp.o ./build/server.o -o ./build/server.out


./build/client.o: ./build/imClient.o ./src/client/client.cc
	$(CC) --std=c++11 -Wall -g -c ./build/imClient.o ./src/client/client.cc -o ./build/client.o


./build/server.o: ./build/imServer.o ./src/server/server.cc
	$(CC) --std=c++11 -Wall -g -c ./build/imServer.o ./src/server/server.cc -o ./build/server.o


./build/imClient.o: ./src/client/imClient.cc ./build/tcpClient.o ./build/dimp.o
	$(CC) --std=c++11 -Wall -g -c ./src/client/imClient.cc ./build/tcpClient.o ./build/dimp.o -o ./build/imClient.o


./build/imServer.o: ./src/server/imServer.cc ./build/tcpServer.o ./build/dimp.o 
	$(CC) --std=c++11 -Wall -g -c ./src/server/imServer.cc ./build/tcpServer.o ./build/dimp.o -o ./build/imServer.o


./build/tcpClient.o: ./include/tcpClient/tcpClient.cc
	$(CC) --std=c++11 -Wall -g -c ./include/tcpClient/tcpClient.cc -o ./build/tcpClient.o


./build/tcpServer.o: ./include/tcpServer/tcpServer.cc
	$(CC) --std=c++11 -Wall -g -c ./include/tcpServer/tcpServer.cc -o ./build/tcpServer.o


./build/dimp.o: ./include/dimp/dimp.cc
	$(CC) --std=c++11 -Wall -g -c ./include/dimp/dimp.cc -o ./build/dimp.o

test_dimp: ./test/dimp_unittest.cc ./build/dimp.o
	g++ --std=c++11 -Wall -g -c ./test/dimp_unittest.cc -o ./build/dimp_unittest.o
	$(CC) --std=c++11 -Wall -g ./build/dimp.o ./build/dimp_unittest.o -o ./test/dimp_unittest.out -lgtest -lpthread

unit_test_imClient: ./build/tcpClient.o ./build/imClient.o ./build/dimp.o
	g++ --std=c++11 -Wall -g -c ./test/imClient_unittest.cc -o ./build/imClient_unittest.o
	$(CC) --std=c++11 -Wall -g ./build/imClient_unittest.o ./build/tcpClient.o ./build/imClient.o ./build/dimp.o -o ./test/unit_test_imClient.out -lgtest -lpthread

pressure_test_imServer: ./build/tcpClient.o ./build/imClient.o ./build/dimp.o
	$(CC) --std=c++11 -Wall -g ./build/imClient.o ./build/tcpClient.o ./build/dimp.o ./test/imServer_pressure_test.cc -o ./test/pressure_test

clean:
	rm ./build/*

