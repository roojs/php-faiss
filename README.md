# PHP extension for FAISS

A PHP extension that links against the FAISS library (`libfaiss`). It exposes `FaissIndex` for local HNSW indexes: create, load, add, count, dimension, save, and search.

The C++ wrapper (`faiss_c_wrapper.cpp` / `faiss_c_wrapper.h`) is part of this tree. Distro `libfaiss-dev` ships the C++ headers and a static library, not this wrapper.

## Prerequisites

- PHP 8.4 dev headers (`php8.4-dev` or matching API version)
- `libfaiss-dev` (BLAS and LAPACK come with it)
- `libglib2.0-dev` (wrapper logging)
- `libgomp-dev`
- `build-essential`, `g++`

**amd64 only.** Deploy hosts are Debian 13 (`trixie`). Ubuntu 25.04+ (`plucky`, `questing`, `resolute`) is for local testing on the same architecture.

`libfaiss-dev` is the distro package. Build on **each host** against that host’s PHP — do not copy `faiss.so` between machines.

## Build

```bash
cd php-fiass
phpize
./configure
make -j$(nproc)
```

Output: `modules/faiss.so`

Optional install:

```bash
sudo make install
```

Or load without installing:

```bash
php -d extension=$PWD/modules/faiss.so test_faiss.php
```

Restart php-fpm / Apache after enabling for those SAPIs.

## Debian package

`php-faiss` is an empty package that depends on the suite's default module, the same way `php-imagick` depends on `php8.4-imagick`. The module itself is `php8.4-faiss`, or `php8.5-faiss` where that suite's default PHP is 8.5. `dh-php` registers it and enables it for every SAPI of that version — `cli`, `fpm`, `apache2`, and the rest. A SAPI installed later enables registered modules on its own.

```bash
sudo apt-get install php-faiss
php -m | grep faiss
```

`scripts/release.sh` tags `vX.Y.Z` from `CHANGELOG.md` and pushes it. GitHub Actions builds the suite `.deb` files and attaches them all to that one release. The [roojs APT repository](https://roojs.github.io/repos/) can pull that release in later. Do not run the release script from an agent.

## API

| Method | Description |
|--------|-------------|
| `FaissIndex::create(int $dim)` | New HNSWFlat index (M=16) |
| `FaissIndex::load(string $path)` | Load from disk |
| `add(array $vector)` | Add one vector |
| `count(): int` | `ntotal` — next sequential label |
| `dimension(): int` | Index dimension |
| `save(string $path)` | Persist index |
| `searchWithIds(array $query, int $k, array $ids): array` | `[['id'=>int,'distance'=>float], …]`; restrict to `ids` via `IDSelectorBatch`. Empty `$ids` → `[]`. |

## Notes

- Links against system `libfaiss` (static `libfaiss.a` plus BLAS, LAPACK, and OpenMP). Does not vendor FAISS sources.
- The index is freed by the object destructor. `unset($idx)` releases it immediately.

## Artificial Intelligence Usage

This project was developed with the assistance of artificial intelligence.

- API design was done by the author
- Other than the API design, very little of this extension was hand crafted
- AI was used to create and build the implementation

## License

Copyright (c) 2026 Roo J Solutions Ltd. Author: Leon <leon@roojs.com>.

php-faiss is distributed under the [PHP License, version 3.01](LICENSE), the same license used by PHP extensions.

Debian packages are maintained by Alan Knowles <alan@roojs.com>.

This product includes PHP software, freely available from <http://www.php.net/software/>.
