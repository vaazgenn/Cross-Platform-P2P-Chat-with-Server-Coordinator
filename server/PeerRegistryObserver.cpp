#include "PeerRegistryObserver.h"

void PeerRegistryObserver::onClientConnected(const std::string& ip, int port) {
    std::lock_guard<std::mutex> lock(peerMutex);
    peerList[ip] = port;
}

void PeerRegistryObserver::onClientDisconnected(const std::string& ip) {
    std::lock_guard<std::mutex> lock(peerMutex);
    peerList.erase(ip);
}

std::map<std::string, int> PeerRegistryObserver::getPeerList() {
    std::lock_guard<std::mutex> lock(peerMutex);
    return peerList;
}
