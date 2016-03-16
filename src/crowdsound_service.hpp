#ifndef crowdsoundserver_hpp
#define crowdsoundserver_hpp

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <grpc++/grpc++.h>
#include "proto/crowdsound_service.grpc.pb.h"
#include "proto/crowdsound_admin_service.grpc.pb.h"

#include "playsource_client.hpp"
#include "skrillex/skrillex.hpp"
#include "DecisionAlgorithm.h"

using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::Status;

using CrowdSound::PingRequest;
using CrowdSound::PingResponse;
using CrowdSound::GetSessionDataRequest;
using CrowdSound::GetSessionDataResponse;
using CrowdSound::GetPlayingRequest;
using CrowdSound::GetPlayingResponse;
using CrowdSound::GetQueueRequest;
using CrowdSound::GetQueueResponse;
using CrowdSound::ListTrendingArtistsRequest;
using CrowdSound::ListTrendingArtistsResponse;
using CrowdSound::PostSongRequest;
using CrowdSound::PostSongResponse;
using CrowdSound::VoteSongRequest;
using CrowdSound::VoteSongResponse;
using CrowdSound::VoteArtistRequest;
using CrowdSound::VoteArtistResponse;
using CrowdSound::VoteSkipRequest;
using CrowdSound::VoteSkipResponse;

using CrowdSound::SkipStatusRequest;
using CrowdSound::SkipStatusResponse;
using CrowdSound::SkipRequest;
using CrowdSound::SkipResponse;

class CrowdSoundAdminImpl;

class CrowdSoundImpl final : public CrowdSound::CrowdSound::Service {
public:
    explicit CrowdSoundImpl(std::shared_ptr<Server> server);

    Status Ping(ServerContext* context, const PingRequest* request, PingResponse* resp) override;
    Status GetSessionData(ServerContext* context, const GetSessionDataRequest* request, GetSessionDataResponse* resp) override;
    Status GetPlaying(ServerContext* context, const GetPlayingRequest* request, GetPlayingResponse* writer) override;
    Status GetQueue(ServerContext* context, const GetQueueRequest* request, ServerWriter<GetQueueResponse>* writer) override;
    Status ListTrendingArtists(ServerContext* context, const ListTrendingArtistsRequest* request, ServerWriter<ListTrendingArtistsResponse>* writer) override;
    Status PostSong(ServerContext* context, ServerReader<PostSongRequest>* request, PostSongResponse* resp) override;
    Status VoteSong(ServerContext* context, const VoteSongRequest* request, VoteSongResponse* resp) override;
    Status VoteArtist(ServerContext* context, const VoteArtistRequest* request, VoteArtistResponse* resp) override;
    Status VoteSkip(ServerContext* context, const VoteSkipRequest* request, VoteSkipResponse* resp) override;

private:
    std::shared_ptr<Server>       server_;
    std::shared_ptr<skrillex::DB> db_;
    skrillex::Mapper              mapper_;
};

#endif
