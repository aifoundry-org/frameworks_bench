#!/usr/bin/env bash

REPO_ROOT="$(git rev-parse --show-toplevel)"
IMAGE_NAME="nn-train:latest"

docker_args=(
  --rm -it
  -v "${REPO_ROOT}/models_gen:/workspace/src"
  -w /workspace
)

docker run "${docker_args[@]}" "${IMAGE_NAME}" bash