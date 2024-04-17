#*****************************************************************************
# meson.build (cfg66/include)
#-----------------------------------------------------------------------------
##
# \file        include/meson.build
# \library     cfg66
# \author      Chris Ahlstrom
# \date        2022-06-22
# \updates     2024-01-20
# \license     $XPC_SUITE_GPL_LICENSE$
#
#  This file is part of the "cfg66" library. See the top-level meson.build
#  file for license information.
#
#  Here we locate the headers.
#
#-----------------------------------------------------------------------------

libcfg66_headers += files(
   'cfg66.hpp',
   'cfg_build_macros.h',
   'c_macros.h',
   'cpp_types.hpp',
   'cfg/appinfo.hpp',
   'cfg/basesettings.hpp',
   'cfg/comments.hpp',
   'cfg/configfile.hpp',
   'cfg/history.hpp',
   'cfg/inisections.hpp',
   'cfg/memento.hpp',
   'cfg/options.hpp',
   'cfg/palette.hpp',
   'cfg_build_macros.h',
   'cli/cliparser_c.h',
   'cli/parser.hpp',
   'session/configuration.hpp',
   'session/directories.hpp',
   'session/manager.hpp',
   'util/filefunctions.hpp',
   'util/msgfunctions.hpp',
   'util/named_bools.hpp',
   'util/strfunctions.hpp'
   )

configure_file(
   input: 'cfg66-config.h.in',
   output: 'cfg66-config.h',
   configuration: cdata
   )

#-----------------------------------------------------------------------------
# Headers are installed via the main meson.build file.
#
#     install_headers(libcfg66_headers)
#-----------------------------------------------------------------------------

#****************************************************************************
# meson.build (cfg66/include)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=meson
#----------------------------------------------------------------------------
