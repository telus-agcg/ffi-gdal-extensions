# Loads mkmf which is used to make makefiles for Ruby extensions
require 'mkmf'

# Give it a name
extension_name = 'agrian_gdal'


header_dirs =
  [
    ::RbConfig::CONFIG['includedir'],
    File.expand_path('~/local/include'),
    '/usr/local/include',
    '/usr/local/gdal/include',
    '/opt/local/include',
    '/opt/gdal/include',
    '/opt/include',
    '/Library/Frameworks/gdal.framework/unix/include',
    '/usr/gdal/include',
    '/usr/include'
  ]
lib_dirs =
  [
    ::RbConfig::CONFIG['libdir'],
    File.expand_path('~/local/lib'),
    '/usr/local/lib',
    '/usr/local/lib64',
    '/usr/local/gdal/lib',
    '/opt/local/lib',
    '/opt/gdal/lib',
    '/opt/lib',
    '/Library/Frameworks/gdal.framework/unix/lib',
    '/usr/lib',
    '/usr/lib64'
  ]

dir_config('libs', header_dirs, lib_dirs)

# The destination
dir_config(extension_name)

%w[gdal_alg gdal_alg_priv].each do |header_name|
  unless find_header("#{header_name}.h")
    abort "#{header_name} is missing.  please install gdal headers"
  end
end

# Do the work
create_makefile(extension_name)
