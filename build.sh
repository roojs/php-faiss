#!/usr/bin/env bash
# Build (and optionally install) the php-fiass Zend extension.
# Usage:
#   ./build.sh              # phpize + configure + make; smoke with -d extension=
#   ./build.sh --install    # same + sudo make install; remind php.ini / php-fpm
set -euo pipefail
cd "$(dirname "$0")"

do_install=0
if [[ "${1:-}" == "--install" ]]; then
  do_install=1
elif [[ -n "${1:-}" ]]; then
  echo "Usage: $0 [--install]" >&2
  exit 1
fi

command -v phpize >/dev/null || {
  echo "error: phpize not found — install php*-dev matching your PHP" >&2
  exit 1
}
command -v php-config >/dev/null || {
  echo "error: php-config not found — install php*-dev matching your PHP" >&2
  exit 1
}
if command -v pkg-config >/dev/null && ! pkg-config --exists faiss 2>/dev/null; then
  echo "warn: pkg-config faiss not found — ensure libfaiss-dev is installed" >&2
fi

if [[ -f Makefile ]]; then
  make distclean || true
fi
phpize
./configure --enable-faiss
make -j"$(nproc)"

so="$(pwd)/modules/faiss.so"
if [[ ! -f "$so" ]]; then
  echo "error: expected $so after make" >&2
  exit 1
fi
echo "Built: $so"

php -d "extension=$so" test_faiss.php

if [[ "$do_install" -eq 1 ]]; then
  sudo make install
  echo "Installed into $(php-config --extension-dir)."
  echo "Enable with: extension=faiss  (e.g. conf.d drop-in) or extension=$so"
  echo "If using php-fpm/Apache, restart that SAPI after enabling."
fi
