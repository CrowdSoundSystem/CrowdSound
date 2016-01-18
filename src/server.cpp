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

using CrowdSound::ListSongsRequest;
using CrowdSound::ListSongsResponse;
using CrowdSound::ListTrendingArtistsRequest;
using CrowdSound::ListTrendingArtistsResponse;
using CrowdSound::PostSongRequest;
using CrowdSound::PostSongResponse;
using CrowdSound::VoteSongRequest;
using CrowdSound::VoteSongResponse;

using skrillex::DB;
using skrillex::ResultSet;
using skrillex::ReadOptions;

CrowdSoundImpl::CrowdSoundImpl(shared_ptr<DB> db)
: db_(db) {
}

Status CrowdSoundImpl::ListSongs(ServerContext* context, const ListSongsRequest* request, ServerWriter<ListSongsResponse>* writer) {
    for (Song s : this->songs_) {
        ListSongsResponse resp;
        resp.set_name(s.name);
        resp.set_isplaying(false);

        writer->Write(resp);
    }

    return Status::OK;
}

Status CrowdSoundImpl::ListTrendingArtists(ServerContext* context, const ListTrendingArtistsRequest* request, ServerWriter<ListTrendingArtistsResponse>* writer) {
    for (Song s : this->songs_) {
        ListTrendingArtistsResponse resp;
        resp.set_name(s.name);
        resp.set_score(32);

        writer->Write(resp);
    }

    return Status::OK;
}

Status CrowdSoundImpl::PostSong(ServerContext* context, ServerReader<PostSongRequest>* reader, PostSongResponse* resp) {
    PostSongRequest request;

    while (reader->Read(&request)) {
        Song s;

        s.name = request.name();
        s.artist = request.artist();
        s.genre = request.genre();

        this->songs_.push_back(s);
    }

    return Status::OK;
}

Status CrowdSoundImpl::VoteSong(ServerContext* context, const VoteSongRequest* request, VoteSongResponse* resp) {
    cout << "Someone tried to vote...lol" << endl;
    cout << "Song: " << request->name() << endl;
    cout << "Artist: " << request->artist() << endl;
    cout << "Like: " << request->like() << endl;

    return Status::OK;
}

