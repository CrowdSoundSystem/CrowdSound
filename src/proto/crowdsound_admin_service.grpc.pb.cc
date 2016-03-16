// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: proto/crowdsound_admin_service.proto

#include "proto/crowdsound_admin_service.pb.h"
#include "proto/crowdsound_admin_service.grpc.pb.h"

#include <grpc++/channel.h>
#include <grpc++/impl/client_unary_call.h>
#include <grpc++/impl/rpc_service_method.h>
#include <grpc++/impl/service_type.h>
#include <grpc++/support/async_unary_call.h>
#include <grpc++/support/async_stream.h>
#include <grpc++/support/sync_stream.h>
namespace CrowdSound {

static const char* Admin_method_names[] = {
  "/CrowdSound.Admin/SkipStatus",
  "/CrowdSound.Admin/Skip",
  "/CrowdSound.Admin/GetSettings",
  "/CrowdSound.Admin/SetSetting",
  "/CrowdSound.Admin/GetVersionInfo",
  "/CrowdSound.Admin/GetDBStats",
};

std::unique_ptr< Admin::Stub> Admin::NewStub(const std::shared_ptr< ::grpc::Channel>& channel, const ::grpc::StubOptions& options) {
  std::unique_ptr< Admin::Stub> stub(new Admin::Stub(channel));
  return stub;
}

Admin::Stub::Stub(const std::shared_ptr< ::grpc::Channel>& channel)
  : channel_(channel), rpcmethod_SkipStatus_(Admin_method_names[0], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Skip_(Admin_method_names[1], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetSettings_(Admin_method_names[2], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SetSetting_(Admin_method_names[3], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetVersionInfo_(Admin_method_names[4], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetDBStats_(Admin_method_names[5], ::grpc::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Admin::Stub::SkipStatus(::grpc::ClientContext* context, const ::CrowdSound::SkipStatusRequest& request, ::CrowdSound::SkipStatusResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_SkipStatus_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::CrowdSound::SkipStatusResponse>* Admin::Stub::AsyncSkipStatusRaw(::grpc::ClientContext* context, const ::CrowdSound::SkipStatusRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::CrowdSound::SkipStatusResponse>(channel_.get(), cq, rpcmethod_SkipStatus_, context, request);
}

::grpc::Status Admin::Stub::Skip(::grpc::ClientContext* context, const ::CrowdSound::SkipRequest& request, ::CrowdSound::SkipResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_Skip_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::CrowdSound::SkipResponse>* Admin::Stub::AsyncSkipRaw(::grpc::ClientContext* context, const ::CrowdSound::SkipRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::CrowdSound::SkipResponse>(channel_.get(), cq, rpcmethod_Skip_, context, request);
}

::grpc::Status Admin::Stub::GetSettings(::grpc::ClientContext* context, const ::CrowdSound::GetSettingsRequest& request, ::CrowdSound::GetSettingsResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_GetSettings_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::CrowdSound::GetSettingsResponse>* Admin::Stub::AsyncGetSettingsRaw(::grpc::ClientContext* context, const ::CrowdSound::GetSettingsRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::CrowdSound::GetSettingsResponse>(channel_.get(), cq, rpcmethod_GetSettings_, context, request);
}

::grpc::Status Admin::Stub::SetSetting(::grpc::ClientContext* context, const ::CrowdSound::SetSettingRequest& request, ::CrowdSound::SetSettingResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_SetSetting_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::CrowdSound::SetSettingResponse>* Admin::Stub::AsyncSetSettingRaw(::grpc::ClientContext* context, const ::CrowdSound::SetSettingRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::CrowdSound::SetSettingResponse>(channel_.get(), cq, rpcmethod_SetSetting_, context, request);
}

::grpc::Status Admin::Stub::GetVersionInfo(::grpc::ClientContext* context, const ::CrowdSound::GetVersionInfoRequest& request, ::CrowdSound::GetVersionInfoResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_GetVersionInfo_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::CrowdSound::GetVersionInfoResponse>* Admin::Stub::AsyncGetVersionInfoRaw(::grpc::ClientContext* context, const ::CrowdSound::GetVersionInfoRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::CrowdSound::GetVersionInfoResponse>(channel_.get(), cq, rpcmethod_GetVersionInfo_, context, request);
}

::grpc::Status Admin::Stub::GetDBStats(::grpc::ClientContext* context, const ::CrowdSound::GetDBStatsRequest& request, ::CrowdSound::GetDBStatsResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_GetDBStats_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::CrowdSound::GetDBStatsResponse>* Admin::Stub::AsyncGetDBStatsRaw(::grpc::ClientContext* context, const ::CrowdSound::GetDBStatsRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::CrowdSound::GetDBStatsResponse>(channel_.get(), cq, rpcmethod_GetDBStats_, context, request);
}

Admin::AsyncService::AsyncService() : ::grpc::AsynchronousService(Admin_method_names, 6) {}

Admin::Service::~Service() {
  delete service_;
}

::grpc::Status Admin::Service::SkipStatus(::grpc::ServerContext* context, const ::CrowdSound::SkipStatusRequest* request, ::CrowdSound::SkipStatusResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void Admin::AsyncService::RequestSkipStatus(::grpc::ServerContext* context, ::CrowdSound::SkipStatusRequest* request, ::grpc::ServerAsyncResponseWriter< ::CrowdSound::SkipStatusResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::Status Admin::Service::Skip(::grpc::ServerContext* context, const ::CrowdSound::SkipRequest* request, ::CrowdSound::SkipResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void Admin::AsyncService::RequestSkip(::grpc::ServerContext* context, ::CrowdSound::SkipRequest* request, ::grpc::ServerAsyncResponseWriter< ::CrowdSound::SkipResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::Status Admin::Service::GetSettings(::grpc::ServerContext* context, const ::CrowdSound::GetSettingsRequest* request, ::CrowdSound::GetSettingsResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void Admin::AsyncService::RequestGetSettings(::grpc::ServerContext* context, ::CrowdSound::GetSettingsRequest* request, ::grpc::ServerAsyncResponseWriter< ::CrowdSound::GetSettingsResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(2, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::Status Admin::Service::SetSetting(::grpc::ServerContext* context, const ::CrowdSound::SetSettingRequest* request, ::CrowdSound::SetSettingResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void Admin::AsyncService::RequestSetSetting(::grpc::ServerContext* context, ::CrowdSound::SetSettingRequest* request, ::grpc::ServerAsyncResponseWriter< ::CrowdSound::SetSettingResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(3, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::Status Admin::Service::GetVersionInfo(::grpc::ServerContext* context, const ::CrowdSound::GetVersionInfoRequest* request, ::CrowdSound::GetVersionInfoResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void Admin::AsyncService::RequestGetVersionInfo(::grpc::ServerContext* context, ::CrowdSound::GetVersionInfoRequest* request, ::grpc::ServerAsyncResponseWriter< ::CrowdSound::GetVersionInfoResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(4, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::Status Admin::Service::GetDBStats(::grpc::ServerContext* context, const ::CrowdSound::GetDBStatsRequest* request, ::CrowdSound::GetDBStatsResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void Admin::AsyncService::RequestGetDBStats(::grpc::ServerContext* context, ::CrowdSound::GetDBStatsRequest* request, ::grpc::ServerAsyncResponseWriter< ::CrowdSound::GetDBStatsResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(5, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::RpcService* Admin::Service::service() {
  if (service_ != nullptr) {
    return service_;
  }
  service_ = new ::grpc::RpcService();
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      Admin_method_names[0],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< Admin::Service, ::CrowdSound::SkipStatusRequest, ::CrowdSound::SkipStatusResponse>(
          std::mem_fn(&Admin::Service::SkipStatus), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      Admin_method_names[1],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< Admin::Service, ::CrowdSound::SkipRequest, ::CrowdSound::SkipResponse>(
          std::mem_fn(&Admin::Service::Skip), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      Admin_method_names[2],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< Admin::Service, ::CrowdSound::GetSettingsRequest, ::CrowdSound::GetSettingsResponse>(
          std::mem_fn(&Admin::Service::GetSettings), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      Admin_method_names[3],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< Admin::Service, ::CrowdSound::SetSettingRequest, ::CrowdSound::SetSettingResponse>(
          std::mem_fn(&Admin::Service::SetSetting), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      Admin_method_names[4],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< Admin::Service, ::CrowdSound::GetVersionInfoRequest, ::CrowdSound::GetVersionInfoResponse>(
          std::mem_fn(&Admin::Service::GetVersionInfo), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      Admin_method_names[5],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< Admin::Service, ::CrowdSound::GetDBStatsRequest, ::CrowdSound::GetDBStatsResponse>(
          std::mem_fn(&Admin::Service::GetDBStats), this)));
  return service_;
}


}  // namespace CrowdSound
