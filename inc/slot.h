#ifndef EVEMIX_SIGNAL_CLIENT_H_
#define EVEMIX_SIGNAL_CLIENT_H_

#include "rpc/server.h"
#include "rpc/client.h"

#include "common.h"

namespace evemix {

class Server : public rpc::server {
public:
    Server(const std::string& address, uint16_t port) : rpc::server(address, port),
        address_(address), port_(port) {};
    ~Server() {};

    const std::string& address() { return address_; }
    uint16_t port() { return port_; };

private:
    const std::string address_;
    const uint16_t port_;
};

class Slot {
public:
    enum class State {
        Uninitialized,
        Connecting,
        Connected,
        Interrupted
    };

    Slot(std::string_view signal_name, std::string_view slot_name, rpc::client& client, 
        Server& server) : 
            signal_name_(signal_name), 
            slot_name_(slot_name), 
            client_(client), 
            server_(server) {};

    ~Slot() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (state_ == State::Connected) {
            client_.call(disconnect_function_name(signal_name_), 
                server_.address(), server_.port(), slot_name_);
        }
    }

    template <typename F>
    void conenct(F function) {
        static constexpr auto reconnect_timeout = std::chrono::milliseconds(300);
        set_state(State::Connecting);
        while (!try_conenct(function)) {
            if (stop_command_) {
                set_state(State::Interrupted);
                stop_command_ = false;
                return;
            }
            std::this_thread::sleep_for(reconnect_timeout);
        }
        set_state(State::Connected);
    }

    template <typename F>
    void conenct_async(F function) {
        std::async([&]() {connect(function)});
    }

    void stop_connecting() {
        stop_command_ = true;
    }

    bool connected() {
        std::lock_guard<std::mutex> lock(mutex_);
        return state_ == State::Connected;
    }

private:
    const std::string signal_name_;
    const std::string slot_name_;
    rpc::client& client_;
    Server& server_;
    std::atomic_bool stop_command_{ false };
    State state_{ State::Uninitialized };
    std::mutex mutex_;

    template <typename F>
    bool try_conenct(F function) {
        try {
            client_.call(connect_function_name(signal_name_), server_.address(), server_.port(), slot_name_);
            server_.bind(slot_name_, function);
            return true;
        } catch (std::exception& e) {
            return false;
        }
    }

    void set_state(State state) {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = state;
    }
};

}


#endif // EVEMIX_SIGNAL_SERVER_H_