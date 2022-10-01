cmake_minimum_required (VERSION 3.8)

project ("evemix")

find_package(GTest CONFIG REQUIRED)
find_package(rpclib CONFIG REQUIRED)

add_library(evemix INTERFACE)
target_include_directories(evemix
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
	    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Include sub-projects.
add_subdirectory ("test")
set_property(TARGET tests PROPERTY CXX_STANDARD 17)