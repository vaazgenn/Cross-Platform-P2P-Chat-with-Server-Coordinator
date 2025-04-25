#include "LoggerObserver.h"
#include <chrono>
#include <ctime>
#include <iomanip>

LoggerObserver::LoggerObserver(const std::string& filename) {
    logFile.open(filename, std::ios::app);
}

LoggerObserver::~LoggerObserver() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void LoggerObserver::onClientConnected(const std::string& ip, int port) {
    log("[CONNECT] " + ip + ":" + std::to_string(port));
}

void LoggerObserver::onClientDisconnected(const std::string& ip) {
    log("[DISCONNECT] " + ip);
}

void LoggerObserver::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&timeNow);

    if (logFile.is_open()) {
        logFile << "[" << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << "] ";
        logFile << message << std::endl;
    }
}
