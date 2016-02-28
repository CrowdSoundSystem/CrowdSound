// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: proto/playsource_service.proto

#include "proto/playsource_service.pb.h"
#include "proto/playsource_service.grpc.pb.h"

#include <grpc++/channel.h>
#include <grpc++/impl/client_unary_call.h>
#include <grpc++/impl/rpc_service_method.h>
#include <grpc++/impl/service_type.h>
#include <grpc++/support/async_unary_call.h>
#include <grpc++/support/async_stream.h>
#include <grpc++/support/sync_stream.h>
namespace Playsource {

static const char* Playsource_method_names[] = {
  "/Playsource.Playsource/QueueSong",
  "/Playsource.Playsource/GetPlaying",
  "/Playsource.Playsource/GetPlayHistory",
};

std::unique_ptr< Playsource::Stub> Playsource::NewStub(const std::shared_ptr< ::grpc::Channel>& channel, const ::grpc::StubOptions& options) {
  std::unique_ptr< Playsource::Stub> stub(new Playsource::Stub(channel));
  return stub;
}

Playsource::Stub::Stub(const std::shared_ptr< ::grpc::Channel>& channel)
  : channel_(channel), rpcmethod_QueueSong_(Playsource_method_names[0], ::grpc::RpcMethod::BIDI_STREAMING, channel)
  , rpcmethod_GetPlaying_(Playsource_method_names[1], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetPlayHistory_(Playsource_method_names[2], ::grpc::RpcMethod::SERVER_STREAMING, channel)
  {}

::grpc::ClientReaderWriter< ::Playsource::QueueSongRequest, ::Playsource::QueueSongResponse>* Playsource::Stub::QueueSongRaw(::grpc::ClientContext* context) {
  return new ::grpc::ClientReaderWriter< ::Playsource::QueueSongRequest, ::Playsource::QueueSongResponse>(channel_.get(), rpcmethod_QueueSong_, context);
}

::grpc::ClientAsyncReaderWriter< ::Playsource::QueueSongRequest, ::Playsource::QueueSongResponse>* Playsource::Stub::AsyncQueueSongRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
  return new ::grpc::ClientAsyncReaderWriter< ::Playsource::QueueSongRequest, ::Playsource::QueueSongResponse>(channel_.get(), cq, rpcmethod_QueueSong_, context, tag);
}

::grpc::Status Playsource::Stub::GetPlaying(::grpc::ClientContext* context, const ::Playsource::GetPlayingRequest& request, ::Playsource::GetPlayingResponse* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_GetPlaying_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::Playsource::GetPlayingResponse>* Playsource::Stub::AsyncGetPlayingRaw(::grpc::ClientContext* context, const ::Playsource::GetPlayingRequest& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::Playsource::GetPlayingResponse>(channel_.get(), cq, rpcmethod_GetPlaying_, context, request);
}

::grpc::ClientReader< ::Playsource::GetPlayHistoryResponse>* Playsource::Stub::GetPlayHistoryRaw(::grpc::ClientContext* context, const ::Playsource::GetPlayHistoryRequest& request) {
  return new ::grpc::ClientReader< ::Playsource::GetPlayHistoryResponse>(channel_.get(), rpcmethod_GetPlayHistory_, context, request);
}

::grpc::ClientAsyncReader< ::Playsource::GetPlayHistoryResponse>* Playsource::Stub::AsyncGetPlayHistoryRaw(::grpc::ClientContext* context, const ::Playsource::GetPlayHistoryRequest& request, ::grpc::CompletionQueue* cq, void* tag) {
  return new ::grpc::ClientAsyncReader< ::Playsource::GetPlayHistoryResponse>(channel_.get(), cq, rpcmethod_GetPlayHistory_, context, request, tag);
}

Playsource::AsyncService::AsyncService() : ::grpc::AsynchronousService(Playsource_method_names, 3) {}

Playsource::Service::~Service() {
  delete service_;
}

::grpc::Status Playsource::Service::QueueSong(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::Playsource::QueueSongResponse, ::Playsource::QueueSongRequest>* stream) {
  (void) context;
  (void) stream;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void Playsource::AsyncService::RequestQueueSong(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::Playsource::QueueSongResponse, ::Playsource::QueueSongRequest>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestBidiStreaming(0, context, stream, new_call_cq, notification_cq, tag);
}

::grpc::Status Playsource::Service::GetPlaying(::grpc::ServerContext* context, const ::Playsource::GetPlayingRequest* request, ::Playsource::GetPlayingResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void Playsource::AsyncService::RequestGetPlaying(::grpc::ServerContext* context, ::Playsource::GetPlayingRequest* request, ::grpc::ServerAsyncResponseWriter< ::Playsource::GetPlayingResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::Status Playsource::Service::GetPlayHistory(::grpc::ServerContext* context, const ::Playsource::GetPlayHistoryRequest* request, ::grpc::ServerWriter< ::Playsource::GetPlayHistoryResponse>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void Playsource::AsyncService::RequestGetPlayHistory(::grpc::ServerContext* context, ::Playsource::GetPlayHistoryRequest* request, ::grpc::ServerAsyncWriter< ::Playsource::GetPlayHistoryResponse>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestServerStreaming(2, context, request, writer, new_call_cq, notification_cq, tag);
}

::grpc::RpcService* Playsource::Service::service() {
  if (service_ != nullptr) {
    return service_;
  }
  service_ = new ::grpc::RpcService();
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      Playsource_method_names[0],
      ::grpc::RpcMethod::BIDI_STREAMING,
      new ::grpc::BidiStreamingHandler< Playsource::Service, ::Playsource::QueueSongRequest, ::Playsource::QueueSongResponse>(
          std::mem_fn(&Playsource::Service::QueueSong), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      Playsource_method_names[1],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< Playsource::Service, ::Playsource::GetPlayingRequest, ::Playsource::GetPlayingResponse>(
          std::mem_fn(&Playsource::Service::GetPlaying), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      Playsource_method_names[2],
      ::grpc::RpcMethod::SERVER_STREAMING,
      new ::grpc::ServerStreamingHandler< Playsource::Service, ::Playsource::GetPlayHistoryRequest, ::Playsource::GetPlayHistoryResponse>(
          std::mem_fn(&Playsource::Service::GetPlayHistory), this)));
  return service_;
}


}  // namespace Playsource

