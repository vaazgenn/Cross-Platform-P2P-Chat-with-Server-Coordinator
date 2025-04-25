#pragma once
#include <string>

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onClientConnected(const std::string& ip, int port) = 0;
    virtual void onClientDisconnected(const std::string& ip) = 0;
};
