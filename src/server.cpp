#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h>
#include "util/network.hpp"

// Boost it up
#include <boost/algorithm/string.hpp>

// DB
#include "skrillex/skrillex.hpp"

// Decision Algorithm (We probably want to clean this up)
#include "DecisionAlgorithm.h"

#define CONNECTION_BACKLOG 5
#define PORT 2048

using namespace std;
using namespace skrillex;

// Global State
bool stop = false;
shared_ptr<DB> db;
shared_ptr<DecisionAlgorithm> algorithm;

// In-memory storage for songs, artists and genres
map<string, int> songID;
map<string, int> songCount;
map<string, int> artistID;
map<string, int> genreID;
map<string, int> artistCount;
map<string, int> genreCount;

int songIDCount = 1;
int artistIDCount = 1;
int genreIDCount = 1;

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

    cout << "Connection recieved!" << endl;

    try {
        string data;

        cout << "Reading lines..." << endl;
        while (!stop && (data = read_line(socket)) != "") {
            cout << "Read Line: " << data << endl;
            // Some parsing required once I know exactly what IAM msg and song info message look like
            // Although if it is an iam message we just kinda treat it as a Chinese daughter and pretend it never happened

            cout << "Checking Media Item Sended" << endl;
            if (data.compare("mediaitemsended\n") == 0){
                algorithm->run();
                ResultSet<Song> playlist;

                // Don't know how this status ish works
                Status status;
                status = db->getSongs(playlist);
                for (auto& s : playlist) {
                    cout << "DB: Song[" << s.name << ", " << s.artist.name << ", " << s.genre.name << "]" << endl;
                }

                close(socket);
                break;
            }

            cout << "Checking Song..." << endl;
            if ((data.substr(0,3).compare("iam") != 0)&&(data.compare("mediaitemsended") != 0)) {
                cout << "Erasing things" << endl;
                boost::erase_all(data, "\"");
                //data.erase(std::remove(data.begin(), data.end(), '['), data.end()); // If we don't use boost
                //data.erase(std::remove(data.begin(), data.end(), ']'), data.end());

                cout << "Splitting..." << endl;
                vector<string> song_info;
                boost::split(song_info, data, boost::is_any_of(","));

                Song s;
                Artist a;
                Genre g;

                string songName   = "Unknown";
                string artistName = "Unknown";
                string genreName  = "Unknown";

                cout << "Constructing Objects" << endl;
                if (song_info.size() >= 1) {
                    songName = song_info.at(0);
                    songName = songName.substr(0, songName.length());
                }
                if (song_info.size() >= 2) {
                    artistName = song_info.at(1);
                    artistName = artistName.substr(0, artistName.length());
                }
                if (song_info.size() >= 3) {
                    genreName = song_info.at(2);
                    genreName = genreName.substr(0, genreName.length() - 1);
                }


                // Check if the song/artist/genre exists in the inmemory storage
                cout << "Crazy saad logic" << endl;
                s.name = songName;
                if (songID.find(songName) == songID.end()){
                    songID[songName] = songIDCount;
                    s.id = songIDCount;
                    songCount[songName] = 1;
                    s.count = 1;
                    songIDCount++;
                } else{
                    s.id = songID.find(songName)->second;
                    s.count = songCount.find(songName)->second+1;
                }

                a.name = artistName;
                if (artistID.find(artistName) == artistID.end()){
                    artistID[artistName] = artistIDCount;
                    a.id = artistIDCount;
                    artistCount[artistName] = artistIDCount;
                    a.count = 1;
                    artistIDCount++;
                } else{
                    a.id = artistID.find(artistName)->second;
                    a.count = artistCount.find(artistName)->second+1;
                }

                g.name = genreName;
                if (genreID.find(genreName) == genreID.end()){
                    genreID[genreName] = genreIDCount;
                    g.id = genreIDCount;
                    genreCount[genreName] = genreIDCount;
                    g.count = 1;
                    genreIDCount++;
                } else{
                    g.id = genreID.find(genreName)->second;
                    g.count = genreCount.find(genreName)->second+1;
                }

                cout << "Crazy logic complete" << endl;
                a.votes = 0;
                a.session_id = 0;

                g.votes = 0;
                g.session_id = 0;

                s.artist = a;
                s.genre = g;
                s.votes = 0;
                s.session_id = 0;

                cout << "Adding song" << endl;
                db->addSong(s);
            }

            // Parse the data.
            //     1) It's an iam message, so set the id (above)
            //     2) It's a song message, so create a song / artist / genre and add to db (and also call db->count())
            //     3) It's a vote message, so vote in db
            //
            // Additionally, we can call algo.run(), still have to decide

            // For now, echo it back!
            cout << "Echoing: " << data << endl;
            send(socket, data.c_str(), data.length(), MSG_NOSIGNAL);
        }

        close(socket);
    } catch (connection_closed) {
        cerr << "Handler Failed: Conncetion Closed" << endl;
    } catch (socket_error) {
        cerr << "Handler Failed: Socket Error" << endl;
    }

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

    // Okay, so what we want here is to push db
    // into the algorithm, such that getMusicData()
    // can read from the database.
    algorithm.reset(new DecisionAlgorithm(DecisionSettings::defaultSettings(), db));

    // The next thing we need to decide to do is
    // how to run the algorithm. Since serve()
    // runs until the program dies, we need a way
    // to run the algorithm. A few ways:
    //     1) Run the algorithm after receiving data (maybe some heuristics, like every 10 pieces of data)
    //     2) Periodically Run
    return serve(PORT);
}
