# Change Log

Format for this file derived from [http://keepachangelog.com](http://keepachangelog.com).

## 1.0.0 - 2022-10-10

### Changed

- See 0.3.1 and 0.3.0 changes.

## 0.3.1 - 2022-10-10 [YANKED]

### Fixed

- Load path error

## 0.3.0 - 2022-10-07 [YANKED]

### Changed

- _BREAKING_: Removed `lwgeom` support in favor `rttopo`. `librttopo` is now a dependency.
- _BREAKING_: Renamed `OGR::GeometryExtensions::EWKBIO` to `OGR::Geometry::EWKBIOExtensions`.
- _BREAKING_: Renamed `OGR::GeometryExtensions::EWKBRecord` to `OGR::Geometry::EWKBRecord`.
- _BREAKING_: Renamed `OGR::GeometryExtensions::LWGeomWrapper` to `OGR::Geometry::LWGeomExtensions`.
- _BREAKING_: Renamed `OGR::GeometryExtensions::WKBRecord` to `OGR::Geometry::WKBRecord`.
- _BREAKING_: All methods with default boolean args are now keyword args.
- _BREAKING_: `required_ruby_version` set to `>= 2.6`

### Added

- [DEV-20059] Ensure `GDAL::RasterBandClassifier#equal_count_ranges` has a minimum spacing between breakpoints.
- [DEV-361] Move extension methods from ffi-gdal to ffi-gdal-extensions.

### Fixed

- [DEV-34269] Ensure `GDAL::RasterBandClassifier#equal_count_ranges` returns a range when only one is requested.
- [DEV-20247] Fix return value for `OGR::Layer::Extensions#point_values` when no features are found.

## 0.1.1 / 2016-12-29

- Fix broken loading when `LWGEOM_LIBRARY_PATH` is specified

## 0.1.0 / 2016-05-27

- Add `OGR::GeometryExtensions::LWGeomWrappers` with `#make_valid`.
- EWKB support for reading and writing geometries.
