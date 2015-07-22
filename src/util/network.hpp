#ifndef network_cpp
#define network_cpp

#include <iostream>
#include <string>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

// Used to satisfy OS X
#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

struct connection_closed { };
struct socket_error {
    int err;
    socket_error(int err) : err(err) { }
};

int connect_to_host(const std::string& ip, short port, double timeout) {
    struct sockaddr_in address;
    struct timeval     tv;

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port        = htons(port);
    tv.tv_sec               = timeout;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sock, (struct sockaddr*)&address, sizeof(address)) == -1) {
        std::cerr << "Error connecting to " << ip << ":" << port << " - " << strerror(errno) << std::endl;
        return -1;
    }

    if (timeout != 0) {
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
    }

    return sock;
}

std::string read_line(int socket) {
    std::string message;
    char        buffer[8192];

    while (true) {
        int read = recv(socket, buffer, sizeof(buffer) - 2, 0);

        if (read > 0) {
            buffer[read]     = '\0';
            buffer[read + 1] = '\0';

            const char* packet = buffer;
            while (*packet != '\0') {
                message += packet;
                packet  += strlen(packet) + 1;

                if (message.length() > 1 && message[message.length() - 1] == '\n') {
                    return message;
                }
            }
        } else if (!read) {
            close(socket);
            throw connection_closed();
        } else {
            throw socket_error(errno);
        }
    }
}

#endif
