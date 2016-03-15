#include <chrono>
#include <iostream>
#include <thread>
#include <queue>

#include "playsource_client.hpp"
#include "skrillex/skrillex.hpp"

using namespace std;

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::WriteOptions;

using Playsource::Song;
using Playsource::QueueSongRequest;
using Playsource::QueueSongResponse;
using Playsource::SkipSongRequest;
using Playsource::SkipSongResponse;

PlaysourceClient::PlaysourceClient(shared_ptr<Channel> channel, int maxQueueSize, shared_ptr<skrillex::DB> db, shared_ptr<DecisionAlgorithm> algorithm)
    : stub_(Playsource::Playsource::NewStub(channel))
    , running_(false)
    , max_queue_size_(maxQueueSize)
    , db_(db)
    , algorithm_(algorithm)
{
}

skrillex::Song getSongFromBuffer(const skrillex::ResultSet<skrillex::Song>& buffer, int song_id) {
    for (auto it = buffer.begin(); it != buffer.end(); it++) {
        if (it->id == song_id) {
            return *it;
        }
    }

    return skrillex::Song();
}

void PlaysourceClient::runQueueLoop() {
    cout << "[playsource] running queue loop" << endl;
    ClientContext context;

    // We do _not_ want buffering here.
    WriteOptions writeOptions;
    writeOptions.clear_buffer_hint();

    // Open up the stream
    shared_ptr<ClientReaderWriter<QueueSongRequest, QueueSongResponse>> stream(
        stub_->QueueSong(&context)
    );

    running_ = true;
    int sendPosition = 0;
    skrillex::ResultSet<skrillex::Song> buffer;

    while (running_) {
        skrillex::Status s = db_->getBuffer(buffer);
        if (s != skrillex::Status::OK()) {
            cout << "[playsource] could not retrieve buffer: " << s.message() << endl;
            return;
        }

        // If the buffer is not full, try to pull in songs.
        if (buffer.size() < max_queue_size_) {
			algorithm_->run(DecisionSettings::defaultSettings());

            int count = buffer.size();
            if (!pullFromQueue(count)) {
                return;
            }

            if (count == 0) {
                cout << "[playsource] queue empty, sleeping..." << endl;
                this_thread::sleep_for(chrono::seconds(1));
            }

            continue;
        }

        for (auto it = buffer.begin() + sendPosition; it != buffer.end(); it++) {
            QueueSongRequest req;
            req.mutable_song()->set_song_id(it->id);
            req.mutable_song()->set_name(it->name);
            req.mutable_song()->add_artists(it->artist.name);

            sendPosition++;
            if (!stream->Write(req, writeOptions)) {
                cout << "[playsource] stream closed" << endl;
                return;
            }

            cout << "[playsource] buffering: " << *it << endl;
        }

        QueueSongResponse resp;
        if (!stream->Read(&resp)) {
            Status status = stream->Finish();
            cout << "[playsource] no response from server. Shutting down playsource" << endl;
            cout << "[playsource] error: " << status.error_message() << endl;
            return;
        }

        // Are there any issues with this song?
        skrillex::Song responseSong = getSongFromBuffer(buffer, resp.song_id());
        if (!resp.finished()) {
            // There was an issue with this song, and it wasn't put into the
            // mopidy queue. What this means, is that the position of this song
            // in our local buffer, is not present in mopidy's buffer, and so everything
            // is offset by 1. Example:
            //
            // Local Buffer:
            // +---+---+---+
            // | A | B | C |
            // +---+---+---+
            //
            // Remote Buffer:
            // +---+---+---+
            // | A | C | - |
            // +---+---+---+
            //
            //
            // Therefore, we have to remove 'B' in our local queue, shifting
            // everything up (shift C, and decrease songPosition). Our buffer will then be
            // back in sync, but we will also need to fill out the missing slot.

            // Step 1: Update buffer state.
            db_->removeFromBuffer(resp.song_id());
            sendPosition--;

            // Step 2: Update db song state.
            if (!resp.found()) {
                cout << "[playsource] song not found: " << responseSong << endl;
                db_->markUnplayable(resp.song_id());
            } else {
                cout << "[playsource] song was not queued (server queue full?): " << responseSong << endl;
            }

            // Step 3: Continue, to let the next iteration refill queue.
            continue;
        } else {
            cout << "[playsource] song finished: " << responseSong << endl;
        }

		algorithm_->run(DecisionSettings::defaultSettings());
        db_->songFinished();
        db_->bufferNext();
        sendPosition--;
    }

    stream->Finish();
}

void PlaysourceClient::stop() {
    running_ = false;
}

bool PlaysourceClient::pullFromQueue(int& count) {
    skrillex::ResultSet<skrillex::Song> queue;
    skrillex::Status s = db_->getQueue(queue);

    if (s != skrillex::Status::OK()) {
        cout << "[playsource] could not retrieve from queue: " << s.message() << endl;
        return false;
    }

    for (auto it = queue.begin(); it != queue.end(); it++, count++) {
        if (count >= max_queue_size_) {
            break;
        }

        db_->bufferNext();
    }

    return true;
}

void PlaysourceClient::skipSong() {
    SkipSongRequest  req;
    SkipSongResponse resp;

    ClientContext context;
    Status status = stub_->SkipSong(&context, req, &resp);
    if (!status.ok()) {
        cout << "[playsource] skip failed" << endl;
    }
}
