# Change Log

Format for this file derived from [http://keepachangelog.com](http://keepachangelog.com).

## Unreleased

### Changed

- *BREAKING*: Renamed `OGR::GeometryExtensions::EWKBIO` to `OGR::Geometry::EWKBIOExtensions`.
- *BREAKING*: Renamed `OGR::GeometryExtensions::EWKBRecord` to `OGR::Geometry::EWKBRecord`.
- *BREAKING*: Renamed `OGR::GeometryExtensions::LWGeomWrapper` to `OGR::Geometry::LWGeomExtensions`.
- *BREAKING*: Renamed `OGR::GeometryExtensions::WKBRecord` to `OGR::Geometry::WKBRecord`.
- *BREAKING*: All methods with default boolean args are now keyword args.
- *BREAKING*: `required_ruby_version` set to `>= 2.6`

### Added

* [DEV-361] Move extension methods from ffi-gdal to ffi-gdal-extensions.

## 0.1.1 / 2016-12-29

* Fix broken loading when `LWGEOM_LIBRARY_PATH` is specified

## 0.1.0 / 2016-05-27

* Add `OGR::GeometryExtensions::LWGeomWrappers` with `#make_valid`.
* EWKB support for reading and writing geometries.
