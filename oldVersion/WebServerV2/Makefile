CXX=g++
CXXFLAGS=-std=c++11 -O2 -Wall -g

TARGET:=myserver
SOURCE:=$(wildcard ../*.cpp)
OBJS=./src/bufferV2.cpp ./src/timer.cpp ./src/epoller.cpp ./src/webserver.cpp \
	 ./src/threadpool.cpp ./src/httprequest.cpp ./src/httpresponse.cpp ./src/httpconnect.cpp ./src/safequeue.cpp ./main.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CXXFLAGS)  $(OBJS) -o ./bin/$(TARGET) -pthread


