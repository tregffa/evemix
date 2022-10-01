#include <iostream>
#include "rpc/server.h"
#include "network_signal.h"

int main(int argc, char* argv[]) {
    // Creating a server that listens on port 8080
    rpc::server srv(8080);
    evemix::Signal network_signal("OnStart", srv);
    srv.async_run();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        network_signal.emit();
    }
    

    // Run the server loop.
    srv.stop();

    return 0;
}