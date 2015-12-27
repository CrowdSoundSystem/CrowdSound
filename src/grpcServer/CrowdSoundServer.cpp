#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "crowdSoundProto.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using CrowdSound::ListSongsRequest;
using CrowdSound::ListSongsResponse;
using CrowdSound::ListTrendingArtistsRequest;
using CrowdSound::ListTrendingArtistsResponse;
using CrowdSound::PostSongRequest;
using CrowdSound::PostSongResponse;
using CrowdSound::VoteSongRequest;
using CrowdSound::VoteSongResponse;
using std::chrono::system_clock;


class CrowdSoundServer final : public CrowdSound::CrowdSound::Service {
	Status ListSongs(ServerContext* context, const ListSongsRequest* request, ServerWriter<ListSongsResponse>* writer) override {
		/*

		IMPLEMENTATION HERE


		*/

		return Status::OK;
	}

	Status ListTrendingArtists(ServerContext* context, const ListTrendingArtistsRequest* request, ListTrendingArtistsResponse* response) override {
		/*

		IMPLEMENTATION HERE


		*/

		return Status::OK;
	}

	Status PostSong(ServerContext* context, ServerReader<PostSongRequest>* reader, PostSongResponse* response) override {
		/*

		IMPLEMENTATION HERE


		*/

		return Status::OK;
	}

	Status VoteSong(ServerContext* context, const VoteSongRequest* request, VoteSongResponse* response) override {
		/*

		IMPLEMENTATION HERE


		*/

		return Status::OK;
	}
};

void RunServer() {
	  std::string server_address("0.0.0.0:50051");
	  CrowdSoundServer service;

	  ServerBuilder builder;
	  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	  builder.RegisterService(&service);
	  std::unique_ptr<Server> server(builder.BuildAndStart());
	  std::cout << "Server listening on " << server_address << std::endl;
	  server->Wait();
	}

int main(int argc, char** argv) {
  RunServer();
  return 0;
}