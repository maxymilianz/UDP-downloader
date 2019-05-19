#ifndef TRANSPORT
#define TRANSPORT

#include "constants.h"
#include "sliding_window.h"
#include "socket_tools.h"

#include <fcntl.h>
#include <iostream>
#include <unistd.h>

using namespace std;


typedef int FileDescriptor;

bool IsIp(string argument);

bool IsNumber(string argument);

void AssureCorrectArguments(int argc, char *argv[]);

FileDescriptor CreateFileDescriptor(string filename);

void SaveDataToFile(FileDescriptor file, uint8_t *data, int data_size);

struct sockaddr_in *CreateRecipient(string ip, int port);

void AcquireFile(string filename, int size, SocketDescriptor socket, struct sockaddr_in *server);

int main(int argc, char *argv[]);

#endif