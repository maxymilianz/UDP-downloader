// Maksymilian Zawartko
// 289528

#include "socket_tools.h"


SocketDescriptor CreateSocket() {
    SocketDescriptor result = socket(AF_INET, SOCK_DGRAM, 0);

    if (result == -1) {
        cout << "Error creating socket\n";
        exit(1);
    }
    
    return result;
}

void BindSocket(SocketDescriptor socket, string ip, int port) {
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &address.sin_addr);

    bind(socket, (struct sockaddr *) &address, sizeof(address));
}