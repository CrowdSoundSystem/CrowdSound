#include <iostream>
#include <grpc++/grpc++.h>

#include "server.hpp"
#include "proto/crowdsound_service.pb.h"
#include "proto/crowdsound_service.grpc.pb.h"

using namespace std;

using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;

using CrowdSound::GetQueueRequest;
using CrowdSound::GetQueueResponse;
using CrowdSound::ListTrendingArtistsRequest;
using CrowdSound::ListTrendingArtistsResponse;
using CrowdSound::PostSongRequest;
using CrowdSound::PostSongResponse;
using CrowdSound::VoteSongRequest;
using CrowdSound::VoteSongResponse;

using skrillex::DB;
using skrillex::ResultSet;
using skrillex::ReadOptions;
using skrillex::WriteOptions;
using skrillex::SortType;
using skrillex::Song;
using skrillex::Artist;
using skrillex::Genre;

CrowdSoundImpl::CrowdSoundImpl(shared_ptr<DB> db, DecisionSettings decision_settings)
: db_(db)
, algo_(new DecisionAlgorithm(decision_settings, db_))
, playsource_(new PlaySource(db_))
, ps_thread_(std::bind(&CrowdSoundImpl::runPlaySource, this)) {
}

void CrowdSoundImpl::runPlaySource() {
    while(true) {
        // Run through iteration of PlaySource, and then generate songs after.
        cout << "Running source" << endl;
        this->playsource_->run();

        cout << "Running algo" << endl;
        this->algo_->run();
    }
}

Status CrowdSoundImpl::GetQueue(ServerContext* context, const GetQueueRequest* request, ServerWriter<GetQueueResponse>* writer) {
    ResultSet<Song> resultSet;
    skrillex::Status status = this->db_->getSongs(resultSet);
    if (status != skrillex::Status::OK()) {
        return Status(StatusCode::INTERNAL, status.message());
    }

    for (Song s : resultSet) {
        GetQueueResponse resp;
        resp.set_name(s.name);
        resp.set_artist(s.artist.name);
        resp.set_genre(s.genre.name);
        resp.set_isplaying(false);

        if (!writer->Write(resp)) {
            cerr << "Failed to write song in GetQueue" << endl;
            return Status(StatusCode::INTERNAL, "");
        }
    }

    return Status::OK;
}

Status CrowdSoundImpl::ListTrendingArtists(ServerContext* context, const ListTrendingArtistsRequest* request, ServerWriter<ListTrendingArtistsResponse>* writer) {
    ReadOptions voteSort;
    voteSort.sort = SortType::Votes;

    ResultSet<Song> resultSet;
    skrillex::Status status = this->db_->getSongs(resultSet, voteSort);
    if (status != skrillex::Status::OK()) {
        return Status(StatusCode::INTERNAL, status.message());
    }

    // TODO: Currently, this just uses the number of votes
    // as the score, but really should be looking at the algorithm
    // for ranking. Maybe there's a 'getScore()' kind of API?
    for (Song s : resultSet) {
        ListTrendingArtistsResponse resp;
        resp.set_name(s.name);
        resp.set_score(s.votes);

        writer->Write(resp);
    }

    return Status::OK;
}

Status CrowdSoundImpl::PostSong(ServerContext* context, ServerReader<PostSongRequest>* reader, PostSongResponse* resp) {
    // We want to decompose the PostSongRequest into an internal song item,
    // which consists of an Artist, Genre, and Song. However, PostSongRequest
    // may not be an actual Song, maybe just an artist (or at least, all we
    // can extract is the artist).
    //
    // TODO: Parsing logic.
    //
    // Once parsing is done, we can insert or link the objects.
    // For now, we will assume the client only sends complete, unique, information.
    PostSongRequest request;
    skrillex::Status status = skrillex::Status::OK();

    while (reader->Read(&request)) {
        Genre g;
        Artist a;
        Song s;

        cout << "Attempting to post song: [" << request.genre() << "] " << request.artist() << " - " << request.name() << endl;

        if (request.genre() != "") {
            g.name = request.genre();

            status = this->db_->addGenre(g);
            if (status != skrillex::Status::OK()) {
                return Status(StatusCode::INTERNAL, status.message());
            }

            s.genre = g;
        }
        if (request.artist() != "") {
            a.name = request.artist();

            this->db_->addArtist(a);
            if (status != skrillex::Status::OK()) {
                return Status(StatusCode::INTERNAL, status.message());
            }

            s.artist = a;
        }
        if (request.name() != "") {
            s.name = request.name();
            this->db_->addSong(s);
            if (status != skrillex::Status::OK()) {
                return Status(StatusCode::INTERNAL, status.message());
            }
        }
    }

    return Status::OK;
}

Status CrowdSoundImpl::VoteSong(ServerContext* context, const VoteSongRequest* request, VoteSongResponse* resp) {
    // The way the current API is designed, there is no feedback to the client
    // about what a PostSong results in, and therefore we have to recompute the
    // target ID each time VoteSong gets called. The algorithm should be
    // deterministic for a given input, so it should be semantically okay.
    //
    // We can alleviate the pressure by either:
    //     a) Internal caching (which, should actually be quick in the parser)
    //     b) Provide feedback to client, so they can vote using ID's instead of string values.
    //
    // The second approach is pretty invasive and touches a bunch of stuff (and probably
    // introduces a lot of work client side too). The first approach comes relatively
    // free with the parsing model, and fuck it, it's a prototype with strick deadlines.

    // TODO: This is primarily blocked by the parser functionality:
    // Give me the complete song object, for a given input. We'll put in a small hack :P

    ResultSet<Song> resultSet;
    skrillex::Status status = this->db_->getSongs(resultSet);
    if (status != skrillex::Status::OK()) {
        return Status(StatusCode::INTERNAL, status.message());
    }

    for (Song s : resultSet) {
        if (s.name == request->name()) {
            cout << "Found voted song: " << s.name << endl;

            int amount = 0;
            if (request->like()) {
                amount = 1;
            } else {
                amount = -1;
            }

            status = this->db_->voteSong(s, amount);
            if (status != skrillex::Status::OK()) {
                return Status(StatusCode::INTERNAL, status.message());
            }

            break;
        }
    }

    cout << "Could not find a song to vote on: " << request->name() << endl;

    return Status::OK;
}

