#ifndef playsourceclient_hpp
#define playsourceclient_hpp

#include <memory>

#include "skrillex/skrillex.hpp"
#include "DecisionAlgorithm.h"

#include <grpc++/grpc++.h>
#include "proto/playsource_service.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class PlaysourceClient {
public:
    PlaysourceClient(
            std::shared_ptr<Channel> channel,
            int maxQueueSize,
            std::shared_ptr<skrillex::DB> db,
            std::shared_ptr<DecisionAlgorithm> algorithm);

    void runQueueLoop();
    void stop();

    void skipSong();
private:
    bool pullFromQueue(int& count);

private:
    bool running_;
    int max_queue_size_;
    std::unique_ptr<Playsource::Playsource::Stub> stub_;
    std::shared_ptr<skrillex::DB> db_;
    std::shared_ptr<DecisionAlgorithm> algorithm_;
	std::shared_ptr<DecisionSettings> algorithmSettings_;
};

#endif
