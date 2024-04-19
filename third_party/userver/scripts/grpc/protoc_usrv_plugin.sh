#!/bin/sh

GRPC_SCRIPTS_PATH="$(dirname $(realpath $0))"
GEN_USERVER_WRAPPERS="${GRPC_SCRIPTS_PATH}/generator.py"

USERVER_PYTHON="${USERVER_GRPC_PYTHON_BINARY:-python3}"

${USERVER_PYTHON} "${GEN_USERVER_WRAPPERS}" "$@"