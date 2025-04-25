#pragma once
#include <vector>
#include <string>
#include "IObserver.h"
#include "PeerRegistryObserver.h"

class Server {
public:
    Server(int port);
    ~Server();

    void start();
    void stop();

    void attach(IObserver* observer);
    void detach(IObserver* observer);

    void setPeerRegistry(PeerRegistryObserver* registry);

private:
    int port;
    bool running;
    int serverSocket;
    std::vector<IObserver*> observers;
    PeerRegistryObserver* peerRegistry = nullptr;

    void notifyClientConnected(const std::string& clientIP, int clientPort);
    void notifyClientDisconnected(const std::string& clientIP);
    void handleClient(int clientSocket, const std::string& clientIP);

    bool initSocket();
    void cleanupSocket();
};
