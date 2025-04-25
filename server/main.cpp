#include "Server.h"
#include "LoggerObserver.h"
#include "PeerRegistryObserver.h"

int main() {
    Server server(12345);

    LoggerObserver logger("server_log.txt");
    PeerRegistryObserver registry;

    server.setPeerRegistry(&registry); // подключение реестра пиров
    server.attach(&logger);
    server.attach(&registry);

    server.start();
    return 0;
}
