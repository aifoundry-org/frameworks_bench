#!/usr/bin/env bash

REPO_ROOT="$(git rev-parse --show-toplevel)"
IMAGE_NAME="zephyr-base:22.04"

docker_args=(
  --rm -it
  -v "${REPO_ROOT}/zephyr:/workspace"
  -w /workspace
)

docker run "${docker_args[@]}" "${IMAGE_NAME}" bash