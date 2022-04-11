# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.2] - 2022-04-11

### Fixed

- `lwgeom_to_wkt` and `lwgeom_to_wkb` had incorrectly wrapped the last param as a `bool`
  instead of a `char`.
- `FFI::LWGeom.find_lib` now uses better logic for finding `liblwgeom`.
