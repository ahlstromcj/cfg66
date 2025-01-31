#*****************************************************************************
# meson.build (cfg66/include)
#-----------------------------------------------------------------------------
##
# \file        include/meson.build
# \library     cfg66
# \author      Chris Ahlstrom
# \date        2022-06-22
# \updates     2025-01-31
# \license     $XPC_SUITE_GPL_LICENSE$
#
#  This file is part of the "cfg66" library. See the top-level meson.build
#  file for license information.
#
#  Here we locate the headers. The root of the header directory is 'include'.
#
#
#-----------------------------------------------------------------------------

libcfg66_headers += files(
#  'c_macros.h',
#  'cpp_types.hpp',
#  'platform_macros.h',
   'cfg66.hpp',
   'cfg/appinfo.hpp',
   'cfg/basesettings.hpp',
   'cfg/comments.hpp',
   'cfg/configfile.hpp',
   'cfg/history.hpp',
   'cfg/inifile.hpp',
   'cfg/inimanager.hpp',
   'cfg/inisection.hpp',
   'cfg/inisections.hpp',
   'cfg/memento.hpp',
   'cfg/options.hpp',
   'cfg/palette.hpp',
   'cfg/recent.hpp',
#  'cfg_build_macros.h',
   'cli/cliparser_c.h',
   'cli/multiparser.hpp',
   'cli/parser.hpp',
   'session/configfile.hpp',
   'session/climanager.hpp',
   'session/configuration.hpp',
   'session/directories.hpp',
   'session/layout.hpp',
   'session/manager.hpp',
   'util/bytevector.hpp',
   'util/filefunctions.hpp',
   'util/msgfunctions.hpp',
   'util/named_bools.hpp',
   'util/strfunctions.hpp'
   )

configure_file(
   input : 'cfg66-config.h.in',
   output : 'cfg66-config.h',
   configuration : cdata
   )

#****************************************************************************
# meson.build (cfg66/include)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=meson
#----------------------------------------------------------------------------
