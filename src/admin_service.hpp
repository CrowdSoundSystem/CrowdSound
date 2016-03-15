#ifndef admin_service_hpp
#define admin_service_hpp

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <grpc++/grpc++.h>
#include "proto/crowdsound_service.grpc.pb.h"
#include "proto/crowdsound_admin_service.grpc.pb.h"

#include "crowdsound_service.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::Status;

using CrowdSound::SkipStatusRequest;
using CrowdSound::SkipStatusResponse;
using CrowdSound::SkipRequest;
using CrowdSound::SkipResponse;

class CrowdSoundAdminImpl final : public CrowdSound::Admin::Service {
public:
    explicit CrowdSoundAdminImpl(std::shared_ptr<CrowdSoundImpl> service);

    Status SkipStatus(ServerContext* context, const SkipStatusRequest* request, SkipStatusResponse* response) override;
    Status Skip(ServerContext* context, const SkipRequest* request, SkipResponse* response) override;
private:
    std::shared_ptr<CrowdSoundImpl> service_;
};

#endif
