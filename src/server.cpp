#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <thread>

#include <sys/socket.h>
#include <sys/types.h>
#include "util/network.hpp"

// Boost it up
#include <boost/algorithm/string.hpp>

// DB
#include "skrillex/skrillex.hpp"

// Decision Algorithm (We probably want to clean this up)
#include "DecisionAlgorithm.h"
#include "PlaySource.h"

#define CONNECTION_BACKLOG 5
#define PORT 2048

using namespace std;
using namespace skrillex;

// Global State
bool stop = false;
shared_ptr<DB> db;
shared_ptr<DecisionAlgorithm> algorithm;
shared_ptr<PlaySource> playsource;

// In-memory storage for songs, artists and genres
map<string, int> songID;
map<string, int> songCount;
map<string, int> artistID;
map<string, int> genreID;
map<string, int> artistCount;
map<string, int> genreCount;
//map<string, int> songVote;
//map<string, int> artistVote;
//map<string, int> genreVote;

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

            cout << "Checking Media Items Ended" << endl;
            if (data.compare("mediaitemsended\n") == 0){
                //algorithm->run();
                ResultSet<Song> playlist;

                Status status;
                status = db->getSongs(playlist);
                for (auto& s : playlist) {
                    cout << "DB: Song[" << s.name << ", " << s.artist.name << ", " << s.genre.name << "]" << endl;
                }
                cout << "Printing Queue Items"<< endl;
                status = db->getQueue(playlist);
                for (auto& s : playlist) {
                    cout << "DB: Queue[" << s.name << ", " << s.artist.name << ", " << s.genre.name << ", " << s.count << "]" << endl;
                }
                
                close(socket);
                break;
            }

            cout << "Checking Song..." << endl;
            if ((data.substr(0,3).compare("iam") != 0)&&(data.compare("mediaitemsended") != 0)) {
                cout << "Erasing things" << endl;
                boost::erase_all(data, "\"");

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
                s.name = songName;
                if (songID.find(songName) == songID.end()){
                    songID[songName] = songIDCount;
                    s.id = songIDCount;
                    songCount[songName] = 1;
                    s.count = 1;
                    songIDCount++;
                } else{
                    s.id = songID.find(songName)->second;
                    songCount[songName]++;
                    s.count = songCount.find(songName)->second;
                    cout << "Song ID = " << s.id << ", Count = " << s.count << endl;
                }

                a.name = artistName;
                if (artistID.find(artistName) == artistID.end()){
                    artistID[artistName] = artistIDCount;
                    a.id = artistIDCount;
                    artistCount[artistName] = 1;
                    a.count = 1;
                    artistIDCount++;
                } else{
                    a.id = artistID.find(artistName)->second;
                    artistCount[artistName]++;
                    a.count = artistCount.find(artistName)->second;
                    cout << "Artist ID = " << a.id << ", Count = " << a.count << endl;
                }

                g.name = genreName;
                if (genreID.find(genreName) == genreID.end()){
                    genreID[genreName] = genreIDCount;
                    g.id = genreIDCount;
                    genreCount[genreName] = 1;
                    g.count = 1;
                    genreIDCount++;
                } else{
                    g.id = genreID.find(genreName)->second;
                    genreCount[genreName]++;
                    g.count = genreCount.find(genreName)->second;
                    cout << "Genre ID = " << g.id << ", Count = " << g.count << endl;
                }

                a.votes = 0;
                a.session_id = 0;

                g.votes = 0;
                g.session_id = 0;

                s.artist = a;
                s.genre = g;
                s.votes = 0;
                s.session_id = 0;

                cout << "Adding song" << endl;
                cout << "Count = " << s.count << endl;
                db->addSong(s);
            }

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

void runPlaySource(shared_ptr<DecisionAlgorithm> algo, shared_ptr<PlaySource> source) {
    
    while(true) {
        // Run through iteration of PS, and then generate songs after
        
        cout << "Running source" << endl;
        source->run();
        
        cout << "Running algo" << endl;
        algo->run();
    }
}

int main() {
    // Load the DB...
    DB* raw = 0;
    Status status = open(raw, "", Options::InMemoryOptions());
    db.reset(raw);

    // Setup Algorthm
    algorithm.reset(new DecisionAlgorithm(DecisionSettings::defaultSettings(), db));
    // Setup playsource
    playsource.reset(new PlaySource(db));
    
    //Start PlaySource thread that uses algorithm to generate music
    // Dont need to join as it terminates when goes out of scope of main.
    std::thread psThread(runPlaySource, algorithm, playsource);
    
    return serve(PORT);
}
