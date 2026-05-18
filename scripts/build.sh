#!/bin/bash
set -euo pipefail

usage() {
    cat <<USAGE
Usage: ${0##*/} [--build-dir <dir>] [--toolchain <dir>]

Options:
  --build-dir <dir>         Build directory. Defaults to build
  --toolchain <file>        Use toolchain file for cross-compiling
  --release                 Build in Release mode (default: RelWithDebInfo).
  --avx                     Build with AVX2 acceleration enabled
  --run                     Run after
  -h, --help                Show this help message.
USAGE
}

BUILD_DIR="build"
TOOLCHAIN=""
BUILD_TYPE="RelWithDebInfo"
RUN=0
AVX=0

while [[ $# -gt 0 ]]; do
    case "$1" in
        --build-dir)    shift; BUILD_DIR="$1" ;;
        --build-dir=*)  BUILD_DIR="${1#*=}" ;;
        --toolchain)    shift; TOOLCHAIN="$1" ;;
        --toolchain=*)  TOOLCHAIN="${1#*=}" ;;
        --release)      BUILD_TYPE="Release" ;;
        --run)          RUN=1 ;;
        --avx)          AVX=1 ;;
        -h|--help)      usage; exit 0 ;;
        *)              echo "Unknown option: $1" >&2; usage >&2; exit 1 ;;
    esac
    shift || true
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

if [[ "${BUILD_DIR}" != /* ]]; then
    BUILD_DIR="${REPO_ROOT}/${BUILD_DIR}"
fi

mkdir -p "${BUILD_DIR}"

if command -v nproc >/dev/null 2>&1; then
    PARALLEL_JOBS="$(nproc)"
else
    PARALLEL_JOBS="$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
fi

cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN} \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DUSE_AVX=${AVX}

cmake --build "${BUILD_DIR}" --parallel "${PARALLEL_JOBS}"

if [[ ${RUN} != 0 ]]; then
    ${BUILD_DIR}/cli
fi
