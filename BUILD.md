# Build

## From source

Install the PHP development headers for the version you are building against, plus `build-essential`, `g++`, `pkgconf`, `libfaiss-dev`, and `libglib2.0-dev`. `g++` provides OpenMP. BLAS and LAPACK come in with `libfaiss-dev`.

```bash
cd php-fiass
phpize
./configure
make -j$(nproc)
```

Output: `modules/faiss.so`

Load it without installing:

```bash
php -d extension=$PWD/modules/faiss.so test_faiss.php
```

## Debian package

`scripts/release/debian-control.sh` writes one module package per version from `phpquery -V`: `php8.4-faiss`, `php8.5-faiss`, and so on. It also writes an empty `php-faiss` package that depends on the distro default (`php` package's dependency). `dpkg-buildpackage` builds those packages. `dh-php` installs the module and the maintainer scripts call `php_invoke enmod <version> ALL faiss`, which enables it for every SAPI of that version (`cli`, `fpm`, `apache2`, and the rest).

```bash
sudo apt-get install -y build-essential dpkg-dev debhelper dh-php php-all-dev pkgconf g++ libfaiss-dev libglib2.0-dev
./scripts/release/debian-control.sh
dpkg-buildpackage -us -uc -b
```

GitHub Actions does this for each suite and appends `+<suite>` to the Debian version so the packages on the one GitHub Release have distinct filenames. `scripts/release.sh` tags; it does not build the packages.
