#!/bin/sh

set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
jobs=${JOBS:-$(nproc)}

exec make -C "$project_dir" -j"$jobs" "$@"
