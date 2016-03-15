#include <iostream>
#include <memory>
#include <string>

#include "skrillex/skrillex.hpp"
#include "DecisionAlgorithm.h"

#include "crowdsound_service.hpp"
#include "admin_service.hpp"
#include "playsource_client.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using namespace std;


int main() {
    // Create/Load the main database
    skrillex::Options options;
    options.create_if_missing = true;

    skrillex::DB* raw = 0;
    skrillex::Status status = skrillex::open(raw, "crowdsound.db", options);
    shared_ptr<skrillex::DB> db(raw);

    if (status != skrillex::Status::OK()) {
        cout << "Failed to initialize database: " << status.message() << endl;
        return -1;
    }

    // Create algorithm
    shared_ptr<DecisionAlgorithm> algorithm(new DecisionAlgorithm(DecisionSettings::defaultSettings(), db));

    unique_ptr<PlaysourceClient> playsource(new PlaysourceClient(
        grpc::CreateChannel("localhost:50052", grpc::InsecureCredentials()),
        4,
        db,
        algorithm
    ));

    string listen_address("0.0.0.0:50051");

    shared_ptr<CrowdSoundImpl> crowdsound_service(new CrowdSoundImpl(db, move(playsource), algorithm));
    shared_ptr<CrowdSoundAdminImpl> admin_service(new CrowdSoundAdminImpl(crowdsound_service));

    ServerBuilder builder;
    builder.AddListeningPort(listen_address, grpc::InsecureServerCredentials());
    builder.RegisterService(crowdsound_service.get());
    builder.RegisterService(admin_service.get());

    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Listening on " << listen_address << endl;

    server->Wait();
}
