#include <ctime>
#include <iostream>
#include <string>
#include <stdexcept>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "skrillex/skrillex.hpp"

#include "util/network.hpp"

#define CONNECTION_BACKLOG 5
#define PORT 2048

using namespace std;
using namespace skrillex;

void* connection_handler(void* sock) {
    int socket = *( (int*)sock );

    try {
        close(socket);
    } catch (connection_closed) {
    } catch (socket_error) { }

    return 0;
}

int serve(int port) {
    int                server_socket;
    int                client_socket;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t          client_len;

    client_len                  = sizeof(client_addr);
    server_socket               = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(port);

    if (::bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Could not bind to port " << port << " - " << strerror(errno) << endl;
        return 1;
    }

    if (listen(server_socket, CONNECTION_BACKLOG) == -1) {
        cerr << "Could not listen on port " << port << " - " << strerror(errno) << endl;
        return 1;
    }

    cout << "Listening on port " << port << endl;

    while (true) { client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            cerr << "Error accepting connection: " << strerror(errno) << endl;
            return 1;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, 0, connection_handler, (void*)&client_socket) == -1) {
            cerr << "Error creationg connection thread: " << strerror(errno) << endl;
            return 1;
        }
    }
}

int main() {
    DB* raw = 0;
    Status status = open(raw, "", Options::InMemoryOptions());


    return serve(PORT);
}
