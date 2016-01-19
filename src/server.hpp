#ifndef crowdsoundserver_hpp
#define crowdsoundserver_hpp

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <grpc++/grpc++.h>
#include "proto/crowdsound_service.grpc.pb.h"

#include "skrillex/skrillex.hpp"
#include "DecisionAlgorithm.h"
#include "PlaySource.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::Status;

using CrowdSound::GetQueueRequest;
using CrowdSound::GetQueueResponse;
using CrowdSound::ListTrendingArtistsRequest;
using CrowdSound::ListTrendingArtistsResponse;
using CrowdSound::PostSongRequest;
using CrowdSound::PostSongResponse;
using CrowdSound::VoteSongRequest;
using CrowdSound::VoteSongResponse;

class CrowdSoundImpl final : public CrowdSound::CrowdSound::Service {
public:
    explicit CrowdSoundImpl(std::shared_ptr<skrillex::DB> db, DecisionSettings decision_settings);

    Status GetQueue(ServerContext* context, const GetQueueRequest* request, ServerWriter<GetQueueResponse>* writer) override;
    Status ListTrendingArtists(ServerContext* context, const ListTrendingArtistsRequest* request, ServerWriter<ListTrendingArtistsResponse>* writer) override;
    Status PostSong(ServerContext* context, ServerReader<PostSongRequest>* request, PostSongResponse* resp) override;
    Status VoteSong(ServerContext* context, const VoteSongRequest* request, VoteSongResponse* resp) override;

protected:
    void runPlaySource();
private:
    std::shared_ptr<skrillex::DB> db_;
    std::shared_ptr<DecisionAlgorithm> algo_;
    std::shared_ptr<PlaySource> playsource_;

    std::thread ps_thread_;
};

#endif
