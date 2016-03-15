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

CrowdSoundAdminImpl::CrowdSoundAdminImpl(shared_ptr<CrowdSoundImpl> service)
: service_(service)
{
}

Status CrowdSoundAdminImpl::SkipStatus(ServerContext* context, const SkipStatusRequest* request, SkipStatusResponse* resp) {
    int votesToSkip = 0;
    int totalUsers = 0;

    {
        lock_guard<mutex> lock(service_->skip_guard_);

        votesToSkip = service_->skip_voters_.size();
        skrillex::Status status = service_->db_->getSessionUserCount(totalUsers);
        if (status != skrillex::Status::OK()) {
            return Status(StatusCode::INTERNAL, status.message());
        }
    }

    resp->set_votes_to_skip(votesToSkip);
    resp->set_total_users(totalUsers);

    return Status::OK;
}

Status CrowdSoundAdminImpl::Skip(ServerContext* context, const SkipRequest* request, SkipResponse* resp) {
    lock_guard<mutex> lock(service_->skip_guard_);

    service_->playsource_->skipSong();
    service_->skip_voters_.clear();

    return Status::OK;
}
