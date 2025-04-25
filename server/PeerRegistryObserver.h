#pragma once
#include "IObserver.h"
#include <map>
#include <string>
#include <mutex>

class PeerRegistryObserver : public IObserver {
public:
    void onClientConnected(const std::string& ip, int port) override;
    void onClientDisconnected(const std::string& ip) override;

    std::map<std::string, int> getPeerList();

private:
    std::map<std::string, int> peerList;
    std::mutex peerMutex;
};
