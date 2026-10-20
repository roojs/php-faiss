#!/usr/bin/env bash
# Copyright (c) 2026 Roo J Solutions Ltd.
# Author: Leon <leon@roojs.com>
# Licensed under the PHP License, version 3.01. See LICENSE.
# Write debian/control binary packages from phpquery -V.
# The source package stays php-faiss. Each module package is named
# php<version>-faiss (php8.4-faiss, php8.5-faiss, ...). An empty
# php-faiss package depends on the distro default, same as php-imagick.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CONTROL="$ROOT/debian/control"

if ! command -v phpquery >/dev/null 2>&1; then
  echo "phpquery not found; install php-all-dev" >&2
  exit 1
fi

mapfile -t versions < <(phpquery -V)
if ((${#versions[@]} == 0)) || [[ -z "${versions[0]}" ]]; then
  echo "phpquery -V returned no PHP versions" >&2
  exit 1
fi

default_pkg="$(apt-cache show php 2>/dev/null | awk '
  /^Depends:/ {
    for (i = 2; i <= NF; i++) {
      gsub(/,/, "", $i)
      if ($i ~ /^php[0-9]/) {
        print $i
        exit
      }
    }
  }
')"
default_version="${default_pkg#php}"
if [[ -z "${default_version}" ]]; then
  echo "could not read the default PHP from the php package" >&2
  exit 1
fi
found=0
for version in "${versions[@]}"; do
  if [[ "${version}" == "${default_version}" ]]; then
    found=1
    break
  fi
done
if [[ "${found}" -ne 1 ]]; then
  echo "default PHP ${default_version} is not in phpquery -V (${versions[*]})" >&2
  exit 1
fi

{
  cat <<'EOF'
Source: php-faiss
Section: php
Priority: optional
Maintainer: Alan Knowles <alan@roojs.com>
Build-Depends: debhelper-compat (= 13),
               dh-php,
               php-all-dev,
               pkgconf,
               g++,
               libfaiss-dev,
               libglib2.0-dev
Standards-Version: 4.7.2
Rules-Requires-Root: no
Homepage: https://github.com/roojs/php-fiass

EOF
  for version in "${versions[@]}"; do
    cat <<EOF
Package: php${version}-faiss
Architecture: amd64
Depends: \${shlibs:Depends},
         \${misc:Depends},
         \${php:Depends}
Provides: php-faiss
Description: FAISS similarity search for PHP ${version}
 FaissIndex builds and searches an HNSW index through libfaiss.
 .
 This package is built for PHP ${version}. phpenmod registers the
 faiss module for every SAPI of that version (cli, fpm, apache2,
 and the rest). A SAPI installed later picks the module up from
 that registry.

EOF
  done
  cat <<EOF
Package: php-faiss
Architecture: all
Depends: php${default_version}-faiss,
         \${misc:Depends}
Description: FAISS similarity search for PHP
 FaissIndex builds and searches an HNSW index through libfaiss.
 .
 This is an empty package that depends on the default PHP version
 (currently ${default_version}).

EOF
} > "$CONTROL"

echo "Wrote ${CONTROL} for: ${versions[*]} (default ${default_version})"
