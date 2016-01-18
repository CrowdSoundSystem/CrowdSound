#!/bin/bash

# Generate the CPP files
protoc --grpc_out=src/ --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) proto/crowdsound_service.proto
protoc --cpp_out=src/ proto/crowdsound_service.proto
