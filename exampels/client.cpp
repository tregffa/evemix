#include <iostream>
#include <atomic>
#include "network_signal.h"

int main() {
    // Creating a client that connects to the localhost on port 8080
    rpc::client client("127.0.0.1", 8080);
    evemix::Server srv("127.0.0.1", 8081);
    evemix::Slot slot("OnStart", "HandleStart", client, srv);
    srv.async_run();
    std::atomic_int i = 0;
    slot.conenct([&]() {
        i++;
        });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << i << "\n";
    }
    return 0;
}

//переделать в example
//написать пару тестов
//добавить описание