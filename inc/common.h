#ifndef EVEMIX_COMMON_H_
#define EVEMIX_COMMON_H_

#include <string_view>
#include <string>

namespace evemix {

namespace {
    constexpr std::string_view kConnect = "Connect";
    constexpr std::string_view kDisConnect = "Disconnect";
}

std::string connect_function_name(std::string_view signal_name) {
    return std::string(kConnect) + std::string(signal_name);
}
std::string disconnect_function_name(std::string_view signal_name) {
    return std::string(kDisConnect) + std::string(signal_name);
}
}

#endif // !EVEMIX_COMMON_H_
