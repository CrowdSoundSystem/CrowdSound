#include <iostream>
#include <memory>
#include <string>

#include "skrillex/skrillex.hpp"
#include "DecisionAlgorithm.h"

#include "server.hpp"
#include "crowdsound_service.hpp"
#include "admin_service.hpp"
#include "playsource_client.hpp"

using grpc::Channel;
using grpc::ServerBuilder;
using namespace std;


int main(int argc, char *argv[]) {
    string db_path = "crowdsound.db";

    if (argc > 1) {
        db_path = string(argv[1]);
    }

    // Create/Load the main database.
    skrillex::Options options;
    options.create_if_missing = true;

    skrillex::DB* raw = 0;
    skrillex::Status status = skrillex::open(raw, db_path, options);
    shared_ptr<skrillex::DB> db(raw);

    if (status != skrillex::Status::OK()) {
        cout << "Failed to initialize database: " << status.message() << endl;
        return -1;
    }

    // Create channel to playsource
    shared_ptr<Channel> channel = grpc::CreateChannel("localhost:50052", grpc::InsecureCredentials());

    // Create main server.
    shared_ptr<Server> server = make_shared<Server>(db, channel);

    // Create gRPC services
    shared_ptr<CrowdSoundImpl>      crowdsound_service(new CrowdSoundImpl(server));
    shared_ptr<CrowdSoundAdminImpl> admin_service(new CrowdSoundAdminImpl(server));

    ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(crowdsound_service.get());
    builder.RegisterService(admin_service.get());

    unique_ptr<grpc::Server> grpcServer(builder.BuildAndStart());
    cout << "Listening on 0.0.0.0:50051" << endl;

    grpcServer->Wait();
}
