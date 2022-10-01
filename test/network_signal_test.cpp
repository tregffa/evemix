#include "gtest/gtest.h"
#include "network_signal.h"


class NetworSignal : public ::testing::Test {
protected:
    void SetUp() override {
        remote_server = std::make_shared<evemix::Server>("127.0.0.1", 8080);
        network_signal = std::make_shared<evemix::Signal>("test_signal", *remote_server);
        remote_server->async_run();
    }

    void TearDown() override {
        remote_server->stop();
    }

    std::shared_ptr<evemix::Server> remote_server;
    std::shared_ptr<evemix::Signal> network_signal;
};

TEST_F(NetworSignal, ShouldUnizializatedAfterCreated) {
    evemix::Server server("127.0.0.1", 8081);
    rpc::client client("127.0.0.1", 8080);
    evemix::Slot slot("test_signal", "test_slot", client, server);
    EXPECT_FALSE(slot.connected());
}

TEST_F(NetworSignal, ShouldConnect) {
    evemix::Server server("127.0.0.1", 8081);
    rpc::client client("127.0.0.1", 8080);
    server.async_run();
    evemix::Slot slot("test_signal", "test_slot", client, server);
    slot.conenct([] {});
    EXPECT_TRUE(slot.connected());
}

TEST_F(NetworSignal, ShouldSignalSlotComunicate) {
    evemix::Server server("127.0.0.1", 8081);
    rpc::client client("127.0.0.1", 8080);
    server.async_run();
    evemix::Slot slot("test_signal", "test_slot", client, server);
    auto flag = false;
    slot.conenct([&] { flag = true; });
    network_signal->emit();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(flag);
}