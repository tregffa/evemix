enable_testing()

file(GLOB SRCS *.cpp)

add_executable(
  tests
  ${SRCS})

target_include_directories(tests
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/inc
)

target_link_libraries(tests PUBLIC evemix PRIVATE 
    GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main
    rpclib::rpc
    )