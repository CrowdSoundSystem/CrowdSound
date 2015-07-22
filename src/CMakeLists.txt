cmake_minimum_required(VERSION 2.8.0)
project(crowdsound)

find_package (Threads REQUIRED)

include_directories(.)

file(GLOB_RECURSE crowdsound_srcs
    "*.cpp"
)

add_executable(crowdsound server.cpp)
target_link_libraries(crowdsound skrillex decision_algorithm "${CMAKE_THREAD_LIBS_INIT}")

install(TARGETS crowdsound DESTINATION bin)
