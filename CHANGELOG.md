# Changelog

All notable changes to php-faiss are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
for git tags (`v0.1.0`).

`debian/changelog` is generated from this file at release time
(`scripts/release/changelog.sh`).

## [0.1.0] - Unreleased

### Added

- Initial release of the FaissIndex PHP extension
- Debian module packages `php8.4-faiss` and `php8.5-faiss`, one per PHP version on the suite, enabled for every SAPI of that PHP
- Empty `php-faiss` package depending on the suite's default module, same as `php-imagick`
