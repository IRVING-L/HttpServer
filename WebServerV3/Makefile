CXX=g++
CXXFLAGS=-std=c++14 -O2 -Wall -g

TARGET:=myserver
SOURCE:=$(wildcard ../*.cpp)
OBJS=./src/buffer.cpp ./src/timer.cpp ./src/epoller.cpp ./src/webserver.cpp ./src/threadpoolV4.cpp \
	./src/httprequest.cpp ./src/httpresponse.cpp ./src/httpconnect.cpp   ./main.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CXXFLAGS)  $(OBJS) -o ./$(TARGET) -pthread


