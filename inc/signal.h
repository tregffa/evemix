#ifndef EVEMIX_SIGNAL_SERVER_H_
#define EVEMIX_SIGNAL_SERVER_H_

#include <set>

#include "rpc/client.h"
#include "rpc/server.h"

#include "common.h"

namespace evemix {

class Signal {
public:
    Signal(std::string_view name, rpc::server& server) : name_(name), server_(server) {
        registration();
    };
    
    template
    <typename... Args>
    void emit(Args... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& [address, port, client, function_name] : clients_) {
            client->async_call(function_name, std::forward<Args>(args)...);
        }
    }
protected:
    void registration() {
        std::string name = connect_function_name(name_);
        server_.bind(name, [this] (const std::string& address, uint16_t port, const std::string& function_name) {
            addClient(address, port, function_name);
            });

        name = disconnect_function_name(name_);
        server_.bind(name, [this](const std::string& address, uint16_t port, const std::string& function_name) {
            removeClient(address, port, function_name);
            });
    }

    void addClient(std::string_view address, uint16_t port, std::string_view function_name) {
        std::string addr = std::string(address);
        auto client = std::make_shared<rpc::client>(addr, port);
        RemoteClient r_client(std::move(addr), port, std::move(client), std::string(function_name));
        std::lock_guard<std::mutex> lock(mutex_);
        if (clients_.find(r_client) != clients_.end()) {
            clients_.erase(r_client);
        }
        clients_.insert(std::move(r_client));
    }
    void removeClient(std::string_view address, uint16_t port, std::string_view function_name) {
        RemoteClient r_client(std::string(address), port, std::shared_ptr<rpc::client>(), std::string(function_name));
        std::lock_guard<std::mutex> lock(mutex_);
        if (clients_.find(r_client) != clients_.end()) {
            clients_.erase(r_client);
        }
    }
private:
    struct RemoteClient {
        const std::string address;
        const uint16_t port;
        std::shared_ptr<rpc::client> client;
        const std::string function_name;

        RemoteClient(std::string address, uint16_t port,
            std::shared_ptr<rpc::client> client, std::string function_name) :
            address(std::move(address)), port(port), client(std::move(client)), function_name(std::move(function_name)) {}

        friend bool operator<(const RemoteClient& l, const RemoteClient& r) {
            return std::tie(l.address, l.port, l.function_name)
                < std::tie(r.address, r.port, r.function_name);
        }

        friend bool operator==(const RemoteClient& l, const RemoteClient& r) {
            return std::tie(l.address, l.port, l.function_name)
                == std::tie(r.address, r.port, r.function_name);
        }
    };

    const std::string name_;
    rpc::server& server_;
    std::set<RemoteClient> clients_;
    std::mutex mutex_;
};

}

#endif // EVEMIX_SIGNAL_SERVER_H_
