#!/bin/bash

# Generate crowdsound cpp files
protoc --grpc_out=src/ --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) proto/crowdsound_service.proto
protoc --cpp_out=src/ proto/crowdsound_service.proto
protoc --grpc_out=src/ --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) proto/crowdsound_admin_service.proto
protoc --cpp_out=src/ proto/crowdsound_admin_service.proto

# Generate playsource cpp files
protoc --grpc_out=src/ --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) proto/playsource_service.proto
protoc --cpp_out=src/ proto/playsource_service.proto
