# FFI::GDAL::Extensions

[ffi-gdal](https://bitbucket.org/agrian/ffi-gdal) is a wrapper around the GDAL
(and OGR) library, but since that is written in C/C++, much of its API feels
like those languages. Methods found in this gem extend that gem by a) making
it feel more Rubyesque, and b) adding some new functionality.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'ffi-gdal-extensions'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install ffi-gdal-extensions

## Usage

TODO: Write usage instructions here

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run
`rake spec` to run the tests. You can also run `bin/console` for an interactive
prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To
release a new version, update the version number in `version.rb`, and then run
`bundle exec rake release`, which will create a git tag for the version, push
git commits and tags, and push the `.gem` file to
[rubygems.org](https://rubygems.org).

### Development Using Docker

The `docker-compose.yml` file provides two configurations for running tests
against:

- `Dockerfile.gdal3`
  - GDAL 3.4.2
  - `liblwgeom` 2.5.5
  - Ruby 3.1.1
- `Dockerfile.gdal2`
  - GDAL 2.4.4
  - `liblwgeom` 2.5.5
  - Ruby 3.1.1

Running tests looks something like:

...for GDAL3:

```
docker-compose run gdal3 bundle exec rake spec
```

...for GDAL2:

```
docker-compose run gdal2 bundle exec rake spec
```
## Contributing

Bug reports and pull requests are welcome on BitBucket at
https://bitbucket.org/agrian/ffi-gdal-extensions. This project is intended to be
a safe, welcoming space for collaboration, and contributors are expected to
adhere to the [Contributor Covenant](http://contributor-covenant.org) code of
conduct.
