#include <chrono>
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

using CrowdSound::PingRequest;
using CrowdSound::PingResponse;
using CrowdSound::GetSessionDataRequest;
using CrowdSound::GetSessionDataResponse;
using CrowdSound::GetQueueRequest;
using CrowdSound::GetQueueResponse;
using CrowdSound::ListTrendingArtistsRequest;
using CrowdSound::ListTrendingArtistsResponse;
using CrowdSound::PostSongRequest;
using CrowdSound::PostSongResponse;
using CrowdSound::VoteSongRequest;
using CrowdSound::VoteSongResponse;

using skrillex::DB;
using skrillex::Mapper;
using skrillex::ResultSet;
using skrillex::ReadOptions;
using skrillex::WriteOptions;
using skrillex::SortType;
using skrillex::Song;
using skrillex::Artist;
using skrillex::Genre;

CrowdSoundImpl::CrowdSoundImpl(shared_ptr<DB> db, unique_ptr<PlaysourceClient> playsource, shared_ptr<DecisionAlgorithm> algorithm)
: db_(db)
, mapper_(new Mapper(db_))
, algo_(algorithm)
, playsource_(move(playsource))
, ps_thread_(std::bind(&CrowdSoundImpl::runPlaySource, this)) {
}

void CrowdSoundImpl::runPlaySource() {
    // TODO: Use member binding instead?
    cout << "[server] running playsource loop" << endl;
    playsource_->runQueueLoop();
}

Status CrowdSoundImpl::Ping(ServerContext* context, const PingRequest* request, PingResponse* resp) {
    int64_t now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    this->db_->setActivity(request->user_id(), now);

    cout << "Received ping from: " << request->user_id() << endl;

    return Status::OK;
}

Status CrowdSoundImpl::GetSessionData(ServerContext* context, const GetSessionDataRequest* request, GetSessionDataResponse* resp) {
    // TODO: Use real data
    resp->set_session_name("Symposium");

    int users = 0;
    skrillex::Status status = db_->getSessionUserCount(users);
    if (status != skrillex::Status::OK()) {
        return Status(StatusCode::INTERNAL, status.message());
    }

    resp->set_num_users(users);

    return Status::OK;
}

Status CrowdSoundImpl::GetQueue(ServerContext* context, const GetQueueRequest* request, ServerWriter<GetQueueResponse>* writer) {
    ResultSet<Song> resultSet;

    // First, send back the buffer
    skrillex::Status status = this->db_->getBuffer(resultSet);
    if (status != skrillex::Status::OK()) {
        return Status(StatusCode::INTERNAL, status.message());
    }

    int count = 0;
    bool first = true;
    for (Song s : resultSet) {
        GetQueueResponse resp;
        resp.set_name(s.name);
        resp.set_artist(s.artist.name);
        resp.set_genre(s.genre.name);
        resp.set_isplaying(first);
        resp.set_isbuffered(true);

        if (!writer->Write(resp)) {
            break;
        }

        first = false;
        count++;
    }

    status = this->db_->getQueue(resultSet);
    if (status != skrillex::Status::OK()) {
        return Status(StatusCode::INTERNAL, status.message());
    }

    for (Song s : resultSet) {
        GetQueueResponse resp;
        resp.set_name(s.name);
        resp.set_artist(s.artist.name);
        resp.set_genre(s.genre.name);
        resp.set_isplaying(false);
        resp.set_isbuffered(false);

        if (!writer->Write(resp)) {
            break;
        }

        if (++count >= 10) {
            break;
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

    int count = 0;
    for (Song s : resultSet) {
        ListTrendingArtistsResponse resp;
        resp.set_name(s.name);
        resp.set_score(s.votes);

        if (!writer->Write(resp)) {
            break;
        }

        if (++count >= 20) {
            break;
        }
    }

    return Status::OK;
}

Status CrowdSoundImpl::PostSong(ServerContext* context, ServerReader<PostSongRequest>* reader, PostSongResponse* resp) {
    // We want to decompose the PostSongRequest into an internal song item,
    // which consists of an Artist, Genre, and Song. However, PostSongRequest
    // may not be an actual Song, maybe just an artist (or at least, all we
    // can extract is the artist).
    //
    // Note: If a client sends PostSong() after a VoteSong(), their vote will be reset.
    PostSongRequest request;
    skrillex::Status status = skrillex::Status::OK();

    while (reader->Read(&request)) {
        Song song;
        cout << "[Server] Received PostSong: [" << request.genre() << "] " << request.artist() << " - " << request.name() << endl;
        status = mapper_->map(song, request.name(), request.artist(), request.genre());
        if (status != skrillex::Status::OK()) {
            return Status(StatusCode::INTERNAL, status.message());
        }

        if (song.genre.id > 0) {
            status = db_->voteGenre(request.user_id(), song.genre, 0);
            if (status != skrillex::Status::OK()) {
                return Status(StatusCode::INTERNAL, status.message());
            }
        }
        if (song.artist.id > 0) {
            status = db_->voteArtist(request.user_id(), song.artist, 0);
            if (status != skrillex::Status::OK()) {
                return Status(StatusCode::INTERNAL, status.message());
            }
        }
        if (song.id > 0) {
            status = db_->voteSong(request.user_id(), song, 0);
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

    cout << "[Server] Received VoteSong: [" << request->artist() << " - " << request->name() << "] - " << request->like() << endl;

    Song song;
    int amount = request->like() ? 1 : -1;
    skrillex::Status status = mapper_->lookup(song, request->name(), request->artist());

    if (status.notFound()) {
        return Status(StatusCode::NOT_FOUND, status.message());
    } else if (status != skrillex::Status::OK()) {
        return Status(StatusCode::INTERNAL, status.message());
    }

    status = db_->voteSong(request->user_id(), song, amount);
    if (status != skrillex::Status::OK()) {
        return Status(StatusCode::INTERNAL, status.message());
    }

    if (song.artist.id > 0) {
        status = db_->voteArtist(request->user_id(), song.artist, amount);
        if (status != skrillex::Status::OK()) {
            return Status(StatusCode::INTERNAL, status.message());
        }
    }

    if (song.genre.id > 0) {
        status = db_->voteGenre(request->user_id(), song.genre, amount);
        if (status != skrillex::Status::OK()) {
            return Status(StatusCode::INTERNAL, status.message());
        }
    }

    return Status::OK;

}

