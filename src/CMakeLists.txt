cmake_minimum_required(VERSION 2.8.0)
project(crowdsound)

include_directories(.)

file(GLOB_RECURSE crowdsound_srcs
    "*.cpp"
)

add_executable(crowdsound server.cpp)
target_link_libraries(crowdsound skrillex decision_algorithm)

install(TARGETS crowdsound DESTINATION bin)
