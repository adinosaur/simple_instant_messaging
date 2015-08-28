
clang++-3.6 --std=c++11 -c dimp.cc -o dimp.o
g++ --std=c++11 -c dimp_unittest.cc -o dimp_unittest.o -lgtest -lpthread
clang++-3.6 dimp.o dimp_unittest.o -o test.out
