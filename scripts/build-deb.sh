#!/usr/bin/env bash
# Kept so that `cd scripts && ./build-deb.sh` still works. The package is built
# by build_deb.sh in the project root, which compiles for this machine's
# architecture and writes the .deb to ~/Documents/Software/deb/<arch>/, beside
# the other projects' packages. RELEASES_COPY=1 also copies it into releases/.
exec "$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/build_deb.sh" "$@"
