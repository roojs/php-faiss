# PHP extension for FAISS

A PHP extension that links against the FAISS library (`libfaiss`). It exposes `FaissIndex` for local HNSW indexes: create, load, add, count, dimension, save, and search.

## Install

Debian packages are published from the [roojs package repositories](https://roojs.github.io/repos/). That page has the APT setup. Install the meta package; it depends on the module for the default PHP on that suite (`php8.4-faiss` or `php8.5-faiss`).

```bash
sudo apt-get install php-faiss
```

## Build

See [BUILD.md](BUILD.md).

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
