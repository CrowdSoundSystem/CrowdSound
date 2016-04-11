#!/bin/python

import subprocess

template = """// GENERATED BY version.py
#ifndef version_hpp
#define version_hpp

#include <string>

const std::string CROWDSOUND_VERSION = "{}";
const std::string PLAYSOURCE_VERSION = "{}";
const std::string ALGORITHM_VERSION  = "{}";
const std::string SKRILLEX_VERSION   = "{}";
const std::string GRPC_VERSION       = "{}";

#endif
"""

def get_git_revision_hash():
    result = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'])
    result = str.strip(result.decode('utf-8'))
    return result

def get_submodule_status():
    result = subprocess.check_output(['git', 'submodule', 'status'])
    result = str.strip(result.decode('utf-8')).split('\n')

    statuses = {}
    for l in result:
        parts = str.strip(l).split(' ')
        statuses[parts[1]] = parts[0][:7]

    return statuses

if __name__ == "__main__":
    submodules = get_submodule_status()

    # Generate library version from git info
    crowdsound_version = get_git_revision_hash()
    algorithm_version = submodules['decisionalgorithm']
    skrillex_version = submodules['skrillex']

    # Playsource is hard coded, until we can hook up
    # we can expose it dynamically (because it can change dynamically)
    playsource_version = "48bfe14d3730c44b2b14b330d10a118eb626a258"[:7]

    # gRPC version is hard coded, because determining it super
    # hard, plus it rarely changes.
    grpc_version = "0.11-1"

    output = template.format(
        crowdsound_version,
        algorithm_version,
        skrillex_version,
        playsource_version,
        grpc_version
    )

    f = open("src/version.hpp", "w")
    f.write(output)
    f.close()
