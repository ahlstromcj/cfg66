#*****************************************************************************
# meson.build (cfg66)
#-----------------------------------------------------------------------------
##
# \file        meson.build
# \library     cfg66
# \author      Chris Ahlstrom
# \date        2022-06-22
# \updates     2024-06-30
# \license     $XPC_SUITE_GPL_LICENSE$
#
#  This file is part of the "cfg66" library. It was part of the libs66
#  collection of libraries, but is now separate and stands alone.
#
#  The "cfg66" library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by the
#  Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
#
#  The "cfg66" library is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#  more details.
#
#  You should have received a copy of the GNU General Public License along with
#  the "cfg66" library. If not, see <https://www.gnu.org/licenses/>.
#
#-----------------------------------------------------------------------------

project('cfg66',
   [ 'c', 'cpp' ],
   version : '0.2',
   license : 'GPLv3',
   meson_version : '>=1.1.0',
   default_options : [
      'c_std=c11', 'cpp_std=c++14', 'warning_level=3',
      'default_library=both'
      ]
   )

#-----------------------------------------------------------------------------
# Information for this sub-project.
#-----------------------------------------------------------------------------
#
#  Tricky: the *.pc file must have a base name matching the name of the
#  subproject *.wrap file.
#
#-----------------------------------------------------------------------------

cfg66_pkg_description = 'Refactors base Seq66 configuration/utility modules.'
cfg66_pkg_version = meson.project_version()
cfg66_api_version = '0.2'
cfg66_info_date = '2024-06-30'
cfg66_info_build_type = get_option('buildtype')
cfg66_info_build_root = meson.project_build_root()
cfg66_info_lib_type = get_option('default_library')
cfg66_info_project_root = meson.project_source_root()
cfg66_debug = get_option('buildtype').contains('debug')
cfg66_project_base = 'cfg66-@0@'.format(cfg66_api_version)
cfg66_pkgconfig_name = 'libcfg66'
cfg66_subproj = meson.is_subproject()

# -fno-inline-functions does not work.

if cfg66_debug
   if not cfg66_subproj
      add_project_arguments(
         [ '-DDEBUG', '-fno-inline-functions' ],
         language : 'cpp'
         )
   endif
endif

#-----------------------------------------------------------------------------
# Easy access to directory options. Commented out the unnecessary ones.
# Many of these definitions are simply informative.
#-----------------------------------------------------------------------------

cfg66_dir = 'cfg66-@0@'.format(cfg66_api_version)
cfg66_prefix = get_option('prefix')
cfg66_bindir = join_paths(cfg66_prefix, get_option('bindir'))
cfg66_includedir = join_paths(cfg66_prefix, get_option('includedir'), cfg66_dir)
cfg66_libdir = join_paths(cfg66_prefix, get_option('libdir'), cfg66_dir)
cfg66_datadir = join_paths(cfg66_prefix, get_option('datadir'), cfg66_dir)
cfg66_docdir = join_paths(cfg66_prefix, cfg66_datadir, 'doc', cfg66_dir)
libraries_top = meson.project_source_root()
libcfg66_include_top = join_paths(libraries_top, 'include')

#-----------------------------------------------------------------------------
# The *.pc file goes into $libdir/cfg66-0.x/pkgconfig instead of
# $libdir/pkgconfig as the meson documentation asserts.  Not sure why, but
# wiring the expected directory here works.
#-----------------------------------------------------------------------------

alt_pkgconfig_libdir = join_paths(
   cfg66_prefix, get_option('libdir'), 'pkgconfig'
   )

#-----------------------------------------------------------------------------
# libcfg66_headers provides the list of all headers in the project. These are
# currently all installable, public headers. They are set in
# include/meson.build.
#
# libcfg66_sources provides the source-code files. They are set in
# src/meson.build.
#-----------------------------------------------------------------------------

libcfg66_headers = []
libcfg66_sources = []
build_args = []

#-----------------------------------------------------------------------------
# Each include file directory is added to the list passed to the compiler.
# We prefer to use the style '#include "cfg/header.hpp".
#-----------------------------------------------------------------------------

libcfg66_includes = include_directories('.',    # libcfg66_include_top,
   'include',
   'include/cfg',
   'include/cli',
   'include/session',
   'include/util'
   )

#-----------------------------------------------------------------------------
# Used by the cfg66_version() function.
#-----------------------------------------------------------------------------

build_args += [
   '-DCFG66_NAME=' + '"' + meson.project_name() + '"',
   '-DCFG66_VERSION=' + '"' + meson.project_version() + '"'
   ]

#-----------------------------------------------------------------------------
# Only make public interfaces visible
#-----------------------------------------------------------------------------

'''
if target_machine.system() == 'windows' or target_machine.system() == 'cygwin'
   build_args += '-DLIBPUBLIC="__declspec(dllexport)"'
else
   build_args += '-DLIBPUBLIC=__attribute__((visibility("default")))'
endif
'''

#-----------------------------------------------------------------------------
# Set up some "HAVE" macros.  By default the header file is generated in the
# "build" directory. If configure_file() is moved to include/meson.build,
# the the header goes into "build/include", which is what we want.
#-----------------------------------------------------------------------------

cc = meson.get_compiler('cpp')
cdata = configuration_data()
cdata.set('limits_h', cc.has_header('limits.h'))

#-----------------------------------------------------------------------------
# Sub-directories. 'tests' has to be descended into later.
#-----------------------------------------------------------------------------

subdir('include')
subdir('src')

#-----------------------------------------------------------------------------
# Dependencies on Linux
#-----------------------------------------------------------------------------
#
# None, yet.
#
#-----------------------------------------------------------------------------

empty_depends = [ ]

#-----------------------------------------------------------------------------
# We recommemd using a recent version of meson by installing it outside the
# Linux distro's repository.  For example, meson on Ubuntu is pretty damn
# old.
#
# The choice of library type is of some import for debugging:
#
#     library:          Defaults to a shared library. The User setting is
#                       'default_library'.  See project() above.
#     shared_library:   Cannot see source files until stepped into.
#     static_library:   Can see the source files when debugging.
#
# This one causes the linker to not see the library functions, wotta pain:
#
#     gnu_symbol_visibility: 'hidden'
#
# Restore it later. Also see the https://gcc.gnu.org/wiki/Visibility site.
#
#-----------------------------------------------------------------------------

cfg66_library_build = library(
   cfg66_project_base,
   libcfg66_sources,
   install : not cfg66_subproj,
   install_dir : cfg66_libdir,
   cpp_args : build_args,
   dependencies : empty_depends,
   include_directories : [ libcfg66_includes ]
   )

#-----------------------------------------------------------------------------
# Make this library usable as a Meson subproject.  This allows a subproject to
# easily specify how it should be used. This makes it interchangeable with the
# same dependency that is provided externally (i.e. installed) by the system.
#-----------------------------------------------------------------------------

libcfg66_dep = declare_dependency(
   include_directories : [ libcfg66_includes ],
   link_with : [ cfg66_library_build ]
   )

#-----------------------------------------------------------------------------
# Make this library usable from the system's package manager.
#-----------------------------------------------------------------------------

install_headers(libcfg66_headers, subdir : cfg66_dir, preserve_path : true)

#-----------------------------------------------------------------------------
# Package config
#-----------------------------------------------------------------------------
#  On Arch Linux, the .pc file is installed to /usr/local/lib/pkgconfig.
#  On Ubuntu, the .pc file is installed to
#     /usr/local/lib/x86_64-linux-gnu/pkgconfig.
#-----------------------------------------------------------------------------

if not cfg66_subproj

   pkg_mod = import('pkgconfig')
   pkg_mod.generate(
      cfg66_library_build,
      filebase : cfg66_pkgconfig_name,
      version : cfg66_pkg_version,
      name : cfg66_project_base,
      description : cfg66_pkg_description,
      install_dir : alt_pkgconfig_libdir,
      subdirs : cfg66_dir,
      libraries : cfg66_library_build,
      url : 'https://github.com/ahlstromcj/cfg66',
      )

# TODO: figure out where to put and install the logo

   install_data(
      files(
         'ChangeLog',
         'LICENSE.md',
         'README.md',
         'doc/cfg66-library-guide.pdf'
         ),
      install_dir : get_option('docdir')
      )

   if get_option('enable-tests')
      subdir('tests')
   endif

endif

#-----------------------------------------------------------------------------
# Info
#-----------------------------------------------------------------------------

summary(
   {
      'Date' : cfg66_info_date,
      'Filebase' : cfg66_project_base,
      'Version' : cfg66_pkg_version,
      'Base' : cfg66_project_base,
      'Description' : cfg66_pkg_description,
      'Source Root' : cfg66_info_project_root,
      'Headers Root' : libcfg66_include_top,
      'Sub Dir' : cfg66_project_base,
      'Build Type' : cfg66_info_build_type,
      'Build Root' : cfg66_info_build_root,
      'Library Type' : cfg66_info_lib_type,
      'Subproject' : cfg66_subproj
   },
   section : 'Cfg66 Project'
   )

summary(
   {
      'prefix' : cfg66_prefix,
      'includedir' : cfg66_includedir,
      'bindir' : cfg66_bindir,
      'libdir' : cfg66_libdir,
      'datadir' : cfg66_datadir,
      'docdir' : cfg66_docdir
   },
   section : 'Cfg66 Install Directories'
   )

#****************************************************************************
# meson.build (cfg66)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=meson
#----------------------------------------------------------------------------
