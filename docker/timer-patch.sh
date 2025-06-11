#!/bin/bash
# Patch the boost timer usage in the codebase to use the new timer API
# This script should be run from the root of the repository

# 1) Switch headers & types and fix .elapsed() → (user+system)/1e9
set -euo pipefail
find . -type f \( -name '*.hpp' -o -name '*.h' -o -name '*.cpp' \) \
  -exec sed -i \
    -e 's|#include <boost/timer.hpp>|#include <boost/timer/timer.hpp>|g' \
    -e 's/\bboost::timer\b/boost::timer::cpu_timer/g' \
    -e 's/\([A-Za-z_][A-Za-z0-9_]*\)\.elapsed()/((\1.elapsed().user+\1.elapsed().system)\/1e9)/g' \
    {} +

# 2) Update every CMakeLists.txt so Boost pulls in timer+chrono
find . -type f -name 'CMakeLists.txt' \
  -exec sed -i -E \
    's|(find_package\(Boost[^(]*COMPONENTS[[:space:]]*)(system filesystem program_options)([[:space:]]*REQUIRED)|\1\2 timer chrono\3|g' \
  {} +
