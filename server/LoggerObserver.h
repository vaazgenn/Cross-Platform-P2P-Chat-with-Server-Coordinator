#pragma once
#include "IObserver.h"
#include <string>
#include <fstream>
#include <mutex>

class LoggerObserver : public IObserver {
public:
    LoggerObserver(const std::string& filename);
    ~LoggerObserver();

    void onClientConnected(const std::string& ip, int port) override;
    void onClientDisconnected(const std::string& ip) override;

private:
    std::ofstream logFile;
    std::mutex logMutex;

    void log(const std::string& message);
};
