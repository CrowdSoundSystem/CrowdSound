#include <ctime>
#include <iostream>
#include <string>
#include <stdexcept>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include "util/network.hpp"

// DB
#include "skrillex/skrillex.hpp"

// Decision Algorithm (We probably want to clean this up)
#include "DecisionAlgorithm.h"
#include "Common/DecisionSettings.h"

#define CONNECTION_BACKLOG 5
#define PORT 2048

using namespace std;
using namespace skrillex;

// Global State
bool stop = false;
shared_ptr<DB> db;

// Each client (from mobile device) has a connection
// with the Twisted Server. The Twisted Server then opens
// a connection to us. As a result, each connection handler
// represents an individual client (and session).
//
// This allows us to keep any session state localized to
// the connection handler.
//
// Additionally, the primary method of the connection handler
// is to read line by line (read_line()). While the client and
// Twisted Server may send the entire message at one time, the
// connection_handler will only proceccess line by line, to make
// things super simple. (Remember, you can send a huge blob, but
// the read_line() message will only read until a new line, and return,
// but the data is still in the socket buffer).
void* connection_handler(void* sock) {
    string id;
    int socket = *( (int*)sock );

    try {
        string data;
        while (!stop && (data = read_line(socket)) != "") {
            // Parse the data.
            //     1) It's an iam message, so set the id (above)
            //     2) It's a song message, so create a song / artist / genre and add to db (and also call db->count())
            //     3) It's a vote message, so vote in db
            //
            // Additionally, we can call algo.run(), still have to decide
        }

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
    // Load the DB...
    DB* raw = 0;
    Status status = open(raw, "", Options::InMemoryOptions());
    db.reset(raw);

    // Setup algorithm settings
    DecisionSettings settings;
	settings.m_countWeight = 1.0;

	settings.m_voteWeight = DecisionSettings::VoteWeight_Equal;

	settings.m_songGenreWeight = DecisionSettings::VoteWeight_Equal;
	settings.m_artistGenreWeight = DecisionSettings::VoteWeight_Equal;

	settings.m_tierMultipliers[Tier_SAG] = 1.0f;
	settings.m_tierMultipliers[Tier_SA] = 0.9f;
	settings.m_tierMultipliers[Tier_SG] = 0.8f;
	settings.m_tierMultipliers[Tier_AG] = 0.7f;
	settings.m_tierMultipliers[Tier_S] = 0.6f;
	settings.m_tierMultipliers[Tier_A] = 0.5f;
	settings.m_tierMultipliers[Tier_G] = 0.4f;

	settings.m_numSongsGenerated = 3;

    // Okay, so what we want here is to push db
    // into the algorithm, such that getMusicData()
    // can read from the database.
	DecisionAlgorithm algorithm(settings);

    // The next thing we need to decide to do is
    // how to run the algorithm. Since serve()
    // runs until the program dies, we need a way
    // to run the algorithm. A few ways:
    //     1) Run the algorithm after receiving data (maybe some heuristics, like every 10 pieces of data)
    //     2) Periodically Run
    return serve(PORT);
}
