#ifndef SOCKET_TOOLS
#define SOCKET_TOOLS

#include <arpa/inet.h>
#include <iostream>
#include <netinet/ip.h>
#include <strings.h>

using namespace std;


typedef int SocketDescriptor;

SocketDescriptor CreateSocket();

void BindSocket(SocketDescriptor socket, string ip, int port);

#endif