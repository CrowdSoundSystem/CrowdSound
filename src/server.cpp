#include "server.hpp"

#include <iostream>
#include <functional>

using namespace std;

using grpc::Channel;

Server::Server(std::shared_ptr<skrillex::DB> db, std::shared_ptr<Channel> channel)
: db_(db)
, algo_(db)
, playsource_(make_unique<PlaysourceClient>(channel, 4, this))
, ps_thread_(bind(&Server::runPlaySource, this))
// Default Settings
, queue_size_(20)
, trending_artists_size_(20)
, skip_threshold_(0.5f)
, session_name_("Crowdsound")
, algo_settings_(DecisionSettings::defaultSettings())
{
}

void Server::runPlaySource() {
    while (true) {
        cout << "[server] running playsource loop" << endl;

        // The queue loop will return if it recovers an error.
        // We wrap it in attemps to restart it.
        //
        // This occurs when the connection to the end playsource
        // is unrecoverable, and any playsource state is invalide,
        // and so it must stop. However, we can re-run the loop
        // in attemps that the connection will come back up, and
        // we will then have a fresh state.
        playsource_->runQueueLoop();
    }
}

void Server::updateActivity(string userId) {
    if (userId.size() > 0) {
        int64_t timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        db_->setActivity(userId, timestamp);
    }
}

void Server::runAlgorithm() {
    lock_guard<mutex> lock(settings_guard_);
    algo_.run(algo_settings_);
}

void Server::skipSong() {
    lock_guard<recursive_mutex> lock(skip_guard_);
    playsource_->skipSong();
    skip_voters_.clear();
}

void Server::voteSkip(std::string userId) {
    lock_guard<recursive_mutex> lock(skip_guard_);
    skip_voters_.insert(userId);

    int users = 0;

    skrillex::Status status = db_->getSessionUserCount(users);
    if (status != skrillex::Status::OK()) {
        cout << "[server] failed to get user count: " << status.message() << endl;
        return;
    }

    float percentage = (float)skip_voters_.size() / (float)users;

    lock_guard<mutex> settings_lock(settings_guard_);
    cout << "[server] skip vote: " << skip_voters_.size() << "/" << users << " threshold: " << skip_threshold_ << endl;
    if (percentage > skip_threshold_) {
        skipSong();
    }

}

void Server::clearSkipVotes() {
    lock_guard<recursive_mutex> lock(skip_guard_);
    skip_voters_.clear();
}
