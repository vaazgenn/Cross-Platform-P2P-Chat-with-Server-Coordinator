#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include "Server.h"
#include <iostream>
#include <thread>
#include <cstring>
#include <algorithm>

Server::Server(int port) : port(port), running(false), serverSocket(-1) {
    initSocket();
}

Server::~Server() {
    stop();
    cleanupSocket();
}

void Server::attach(IObserver* observer) {
    observers.push_back(observer);
}

void Server::detach(IObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Server::notifyClientConnected(const std::string& clientIP, int clientPort) {
    for (auto observer : observers) {
        observer->onClientConnected(clientIP, clientPort);
    }
}

void Server::notifyClientDisconnected(const std::string& clientIP) {
    for (auto observer : observers) {
        observer->onClientDisconnected(clientIP);
    }
}

bool Server::initSocket() {
#ifdef _WIN32
    WSADATA wsaData;
    return (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
#else
    return true;
#endif
}

void Server::cleanupSocket() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    sockaddr_in serverAddr{}; // структура для хранения данных о сервере 
    serverAddr.sin_family = AF_INET; // ipv4
    serverAddr.sin_port = htons(port); // преобразуем порядок байтов под tcp/ip
    serverAddr.sin_addr.s_addr = INADDR_ANY; // слушаем все 

    // привязываем сокет к порту и ip (порт резервируется)
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed\n";
        return;
    }

    // перводим сокет в режим прослушивания 
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed\n";
        return;
    }

    running = true;
    std::cout << "Server listening on port " << port << "\n";

    while (running) {
        sockaddr_in clientAddr{};
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);// блок. вызов;
        if (clientSocket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::string clientIP = inet_ntoa(clientAddr.sin_addr);
        int clientPort = ntohs(clientAddr.sin_port);
        notifyClientConnected(clientIP, clientPort);

        std::thread(&Server::handleClient, this, clientSocket, clientIP).detach();
    }
}

void Server::stop() {
    running = false;
#ifdef _WIN32
    closesocket(serverSocket);
#else
    close(serverSocket);
#endif
}

void Server::handleClient(int clientSocket, const std::string& clientIP) {
    char buffer[1024];
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        std::string msg(buffer);
        if (msg == "GET_PEERS" && peerRegistry) {
            std::string peerList;
            auto peers = peerRegistry->getPeerList();
            for (const auto& [ip, port] : peers) {
                if (ip != clientIP) {
                    peerList += ip + ":" + std::to_string(port) + "\n";
                }
            }
            send(clientSocket, peerList.c_str(), peerList.size(), 0);
        }
    }

    notifyClientDisconnected(clientIP);
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}

void Server::setPeerRegistry(PeerRegistryObserver* registry) {
    peerRegistry = registry;
}
