#*****************************************************************************
# meson.build (cfg66/tests)
#-----------------------------------------------------------------------------
##
# \file        tests/meson.build
# \library     cfg66
# \author      Chris Ahlstrom
# \date        2022-06-22
# \updates     2023-07-28
# \license     $XPC_SUITE_GPL_LICENSE$
#
#  This file is part of the "cfg66" library. See the top-level meson.build
#  file for license information.
#
#-----------------------------------------------------------------------------

cliparser_test_c_exe = executable(
   'cliparser_test_c',
   sources : [ 'cliparser_test_c.c' ],
   dependencies : [ cfg66_dep ]
   )

cliparser_test_exe = executable(
   'cliparser_test',
   sources : [ 'cliparser_test.cpp' ],
   dependencies : [ cfg66_dep ]
   )

history_test_exe = executable(
   'history_test',
   sources :  [ 'history_test.cpp' ],
   dependencies : [ cfg66_dep ]
   )

manager_test_exe = executable(
   'manager_test',
   sources : [ 'manager_test.cpp' ],
   dependencies : [ cfg66_dep ]
   )

options_test_exe = executable(
   'options_test',
   sources :  [ 'options_test.cpp' ],
   dependencies : [ cfg66_dep ]
   )

ini_test_exe = executable(
   'ini_test',
   sources : [ 'ini_test.cpp' ],
   dependencies : [ cfg66_dep ]
   )

test('C Parser Test', cliparser_test_c_exe)
test('C++ Parser Test', cliparser_test_exe)
test('C++ Manager Test', manager_test_exe)
test('C++ Options Test', options_test_exe)
test('C++ INI Test', ini_test_exe)
test('C++ History Test', history_test_exe)
   
#****************************************************************************
# meson.build (cfg66/tests)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=meson
#----------------------------------------------------------------------------

