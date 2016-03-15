#include <grpc++/grpc++.h>

#include "admin_service.hpp"
#include "proto/crowdsound_admin_service.pb.h"
#include "proto/crowdsound_admin_service.grpc.pb.h"

using namespace std;

using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;

using CrowdSound::SkipStatusRequest;
using CrowdSound::SkipStatusResponse;
using CrowdSound::SkipRequest;
using CrowdSound::SkipResponse;

CrowdSoundAdminImpl::CrowdSoundAdminImpl(shared_ptr<Server> server)
: server_(server)
{
}

Status CrowdSoundAdminImpl::SkipStatus(ServerContext* context, const SkipStatusRequest* request, SkipStatusResponse* resp) {
    int votesToSkip = 0;
    int totalUsers = 0;

    {
        lock_guard<recursive_mutex> lock(server_->skip_guard_);

        votesToSkip = server_->skip_voters_.size();
        skrillex::Status status = server_->db_->getSessionUserCount(totalUsers);
        if (status != skrillex::Status::OK()) {
            return Status(StatusCode::INTERNAL, status.message());
        }
    }

    resp->set_votes_to_skip(votesToSkip);
    resp->set_total_users(totalUsers);

    return Status::OK;
}

Status CrowdSoundAdminImpl::Skip(ServerContext* context, const SkipRequest* request, SkipResponse* resp) {
    server_->skipSong();

    return Status::OK;
}
