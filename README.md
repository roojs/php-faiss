# php-fiass — PHP Zend extension for FAISS

Vendored `faiss_c_wrapper.cpp/.h` from `OLLMchat/libocvector/`. Exposes the `FaissIndex` class to PHP.

Sibling repo at `gitlive/php-fiass` (used by `Pman.Translate` RAG indexer).

## Prerequisites

- PHP 8.4 dev headers (`php8.4-dev` or matching API version)
- `libfaiss-dev` and runtime BLAS/LAPACK (`libblas-dev`, `liblapack-dev` — pulled in by `libfaiss-dev` on Ubuntu)
- `build-essential`, `g++`

Build on **each host** against that host’s PHP — do not copy `faiss.so` between machines.

## Build / install

```bash
cd php-fiass
./build.sh              # phpize + configure + make; smoke-tests modules/faiss.so
./build.sh --install    # same + sudo make install
```

Output: `modules/faiss.so`

After `--install`, enable in `php.ini` / conf.d if needed:

```ini
extension=faiss
```

Or load the built path without installing:

```ini
extension=/path/to/php-fiass/modules/faiss.so
```

Restart php-fpm / Apache after enabling for those SAPIs.

### Manual fallback

```bash
cd php-fiass
phpize
./configure --enable-faiss
make -j$(nproc)
# optional: sudo make install
```

## API

| Method | Description |
|--------|-------------|
| `FaissIndex::create(int $dim)` | New HNSWFlat index (M=16) |
| `FaissIndex::load(string $path)` | Load from disk |
| `add(array $vector)` | Add one vector |
| `addBatch(array $flat, int $dim)` | Add N vectors from flat array |
| `count(): int` | `ntotal` — next sequential label |
| `save(string $path)` | Persist index |
| `search(array $query, int $k): array` | `[['id'=>int,'distance'=>float], …]` |

PHP wrapper: `Pman.Translate/RAG/Faiss.php` (`Pman_Translate_RAG_Faiss`).
