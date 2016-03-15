#ifndef server_hpp
#define server_hpp

#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>

#include <grpc++/grpc++.h>

#include "skrillex/skrillex.hpp"
#include "DecisionAlgorithm.h"
#include "playsource_client.hpp"

using grpc::Channel;

class CrowdSoundImpl;
class CrowdSoundAdminImpl;

class PlaysourceClient;

class Server final {
public:
    explicit Server(std::shared_ptr<skrillex::DB> db, std::shared_ptr<Channel> playsource);

    void updateActivity(std::string userId);
    void runAlgorithm();
    void skipSong();
    void voteSkip(std::string userId);

private:
    void runPlaySource();
private:
    std::shared_ptr<skrillex::DB>     db_;
    DecisionAlgorithm                 algo_;
    std::unique_ptr<PlaysourceClient> playsource_;

    std::recursive_mutex  skip_guard_;
    std::set<std::string> skip_voters_;

    std::mutex       settings_guard_;
    int              queue_size_;
    int              trending_artists_size_;
    float            skip_threshold_;
    std::string      session_name_;
    DecisionSettings algo_settings_;

    std::thread ps_thread_;

private:
    friend class CrowdSoundImpl;
    friend class CrowdSoundAdminImpl;
    friend class PlaysourceClient;
};

#endif

