cmake_minimum_required(VERSION 2.8.0)
project(crowdsound)

include_directories(.)

# Unfortunately, GRPC doesn't have a find_package module to do this
# automatically, so we'll just do it manually.
find_library(GPR_LIBRARY gpr REQUIRED)
find_library(GRPC_LIBRARY grpc REQUIRED)
find_library(GRPC_CPP_LIBRARY grpc++_unsecure REQUIRED)
set(GRPC_LIBRARIES ${GRPC_CPP_LIBRARY} ${GRPC_LIBRARY} ${GPR_LIBRARY})

#protobuf_generate_grpc_cpp(ProtoGRPCSources ProtoGRPCHeaders ${CMAKE_SOURCE_DIR}/proto/crowdsound_service.proto)

file(GLOB_RECURSE crowdsound_srcs
    "*.cpp"
)

file(GLOB_RECURSE ProtoGRPCSources
    "proto/*.cc"
)

include_directories(. "proto/" ${CMAKE_CURRENT_BINARY_DIR})

add_library(generated ${ProtoGRPCSources} ${ProtoGRPCHeaders})
add_executable(crowdsound ${crowdsound_srcs})

set_property(TARGET generated PROPERTY CXX_STANDARD 11)
set_property(TARGET crowdsound PROPERTY CXX_STANDARD 11)
target_link_libraries(crowdsound generated skrillex decision_algorithm ${CMAKE_THREAD_LIBS_INIT} ${GRPC_LIBRARIES} ${PROTOBUF_LIBRARIES})

install(TARGETS crowdsound DESTINATION bin)
