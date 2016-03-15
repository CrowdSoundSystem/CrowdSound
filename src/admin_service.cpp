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

    response->set_filter_buffered(server_->skrillex_read_options_.filter_buffered);
    response->set_inactivity_threshold(server_->skrillex_read_options_.inactivity_threshold);
    response->set_result_limit(server_->skrillex_read_options_.result_limit);

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

    return Status::OK;
}


Status CrowdSoundAdminImpl::SetSetting(ServerContext* context, const SetSettingRequest* request, SetSettingResponse* response) {
    lock_guard<mutex> lock(server_->settings_guard_);

    // Why no switch supper :( or any nice way to do this
    if (request->key() == "filter_buffered")            { return apply(request, server_->skrillex_read_options_.filter_buffered); }
    else if (request->key() == "inactivity_threshold")  { return apply(request, server_->skrillex_read_options_.inactivity_threshold); }
    else if (request->key() == "result_limit")          { return apply(request, server_->skrillex_read_options_.result_limit); }

    // Query Options
    else if (request->key() == "session_name")          { return apply(request, server_->session_name_); }
    else if (request->key() == "queue_size")            { return apply(request, server_->queue_size_); }
    else if (request->key() == "trending_artists_size") { return apply(request, server_->trending_artists_size_); }
    else if (request->key() == "skip_threshold")        { return apply(request, server_->skip_threshold_); }

    // Algorithm settings
    else if (request->key() == "count_weight")          { return apply(request, server_->algo_settings_.m_countWeight); }
    else if (request->key() == "vote_weight")           { return apply(request, (int&)server_->algo_settings_.m_voteWeight); }
    else if (request->key() == "genre_weight")          { return apply(request, server_->algo_settings_.m_genreWeight); }
    else if (request->key() == "artist_weight")         { return apply(request, server_->algo_settings_.m_artistWeight); }
    else if (request->key() == "played_again_mult")     { return apply(request, server_->algo_settings_.m_playedAgainMultipler); }
    else if (request->key() == "min_repeat_window")     { return apply(request, server_->algo_settings_.m_minsBeforeCanPlayAgain); }

    return Status(StatusCode::NOT_FOUND, "key not found");
}

Status CrowdSoundAdminImpl::apply(const SetSettingRequest* request, int& val) {
    if (request->value_case() != SetSettingRequest::ValueCase::kIntVal) {
        return Status(StatusCode::INVALID_ARGUMENT, "invalid type for key");
    }

    val = request->int_val();
}
Status CrowdSoundAdminImpl::apply(const SetSettingRequest* request, float& val) {
    if (request->value_case() != SetSettingRequest::ValueCase::kFloatVal) {
        return Status(StatusCode::INVALID_ARGUMENT, "invalid type for key");
    }

    val = request->float_val();
}
Status CrowdSoundAdminImpl::apply(const SetSettingRequest* request, bool& val) {
    if (request->value_case() != SetSettingRequest::ValueCase::kBoolVal) {
        return Status(StatusCode::INVALID_ARGUMENT, "invalid type for key");
    }

    val = request->bool_val();
}
Status CrowdSoundAdminImpl::apply(const SetSettingRequest* request, string& val) {
    if (request->value_case() != SetSettingRequest::ValueCase::kStrVal) {
        return Status(StatusCode::INVALID_ARGUMENT, "invalid type for key");
    }

    val = request->str_val();
}
