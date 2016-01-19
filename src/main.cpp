#include <iostream>
#include <memory>
#include <string>

#include "skrillex/skrillex.hpp"
#include "DecisionAlgorithm.h"

#include "server.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using namespace std;


int main() {
    skrillex::DB* raw = 0;
    skrillex::Status status = skrillex::open(raw, "", skrillex::Options::InMemoryOptions());
    shared_ptr<skrillex::DB> db(raw);

    if (status != skrillex::Status::OK()) {
        cout << "Failed to initialize database: " << status.message() << endl;
        return -1;
    }

    string listen_address("0.0.0.0:50051");

    CrowdSoundImpl service(db, DecisionSettings::defaultSettings());

    ServerBuilder builder;
    builder.AddListeningPort(listen_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Listening on " << listen_address << endl;

    server->Wait();
}
