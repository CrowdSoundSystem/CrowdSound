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

    {
        lock_guard<mutex> lock(server_->settings_guard_);
        resp->set_threshold(server_->skip_threshold_);
    }

    return Status::OK;
}

Status CrowdSoundAdminImpl::Skip(ServerContext* context, const SkipRequest* request, SkipResponse* resp) {
    server_->skipSong();

    return Status::OK;
}

Status CrowdSoundAdminImpl::GetSettings(ServerContext* context, const GetSettingsRequest* request, GetSettingsResponse* response) {
    lock_guard<mutex> lock(server_->settings_guard_);

    // TODO: Global database read options.
    response->set_filter_buffered(true);
    response->set_inactivity_threshold(100);
    response->set_result_limit(100);

    // Queue/Settings Settings
    response->set_session_name(server_->session_name_);
    response->set_queue_size(server_->queue_size_);
    response->set_trending_artists_size(server_->trending_artists_size_);
    response->set_skip_threshold(server_->skip_threshold_);

    // Algorithm Settings
    response->set_count_weight(server_->algo_settings_.m_countWeight);
    response->set_vote_weight(server_->algo_settings_.m_voteWeight);
    response->set_genre_weight(server_->algo_settings_.m_genreWeight);
    response->set_artist_weight(server_->algo_settings_.m_artistWeight);
    response->set_played_again_mult(server_->algo_settings_.m_playedAgainMultipler);
    response->set_min_repeat_window(server_->algo_settings_.m_minsBeforeCanPlayAgain);
}

Status CrowdSoundAdminImpl::SetSetting(ServerContext* context, const SetSettingRequest* request, SetSettingResponse* response) {
    return Status::OK;
}
