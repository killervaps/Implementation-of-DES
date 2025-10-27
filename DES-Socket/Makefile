CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread

all: des_server des_client

des_server: des_server.cpp des_crypto.cpp
	$(CXX) $(CXXFLAGS) -o des_server des_server.cpp des_crypto.cpp

des_client: des_client.cpp des_crypto.cpp
	$(CXX) $(CXXFLAGS) -o des_client des_client.cpp des_crypto.cpp

clean:
	rm -f des_server des_client

.PHONY: all clean