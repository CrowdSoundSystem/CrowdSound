#ifndef playsourceclient_hpp
#define playsourceclient_hpp

#include <memory>

#include "server.hpp"
#include "skrillex/skrillex.hpp"
#include "DecisionAlgorithm.h"

#include <grpc++/grpc++.h>
#include "proto/playsource_service.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class Server;

class PlaysourceClient {
public:
    PlaysourceClient(
            std::shared_ptr<Channel> channel,
            int maxQueueSize,
            Server* server);

    void runQueueLoop();
    void stop();

    void skipSong();
private:
    bool pullFromQueue(int& count);

private:
    bool running_;
    int max_queue_size_;

    std::unique_ptr<Playsource::Playsource::Stub> stub_;

    Server*                       server_;
    std::shared_ptr<skrillex::DB> db_;
};

#endif
