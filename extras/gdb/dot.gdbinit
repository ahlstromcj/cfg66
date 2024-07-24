#******************************************************************************
#  gdbinit (~/.gdbinit)
#------------------------------------------------------------------------------
#
#  Maintainer:    Chris Ahlstrom
#  Last Change:   2013-09-16 to 2024-07-24
#  Project:       Portable Configuration
#  License:       None.  Use it in any manner whatsover, and don't blame me.
#  Usage:
#
#     This file provides my personal version of .gdbinit.
#     It is a personal settings file for the GNU debugger.
#
#     To use it, simply copy it to your home directory in this manner:
#
#        cp gdbinit ~/.gdbinit
#
#------------------------------------------------------------------------------

set print pretty on
set print elements 0
set pagination off
handle SIG32 nostop

define pvec
   if $argc == 2
      set $elem = $arg0.size()
      if $arg1 >= $arg0.size()
         printf "Error, %s.size() = %d, last element:\n", "$arg0", $arg0.size()
         set $elem = $arg1 -1
      end
      print/x *($arg0._M_impl._M_start + $elem)@1
   else
      print/x *($arg0._M_impl._M_start)@$arg0.size()
   end
end

document pvec
Display vector contents in hex notation.
Usage: pvec v [ index ]
v is the name of the vector.  index is an optional argument specifying the
element to display.
end

#------------------------------------------------------------------------------
# Installation: Change to the desired directory and run this command to get
# the python std-handling modules:
#
# 		svn co svn://gcc.gnu.org/svn/gcc/trunk/libstdc++-v3/python
#
#------------------------------------------------------------------------------

python
import sys
sys.path.insert(0, '/home/ahlstrom/.config/gdb/python')
from libstdcxx.v6.printers import register_libstdcxx_printers
register_libstdcxx_printers (None)
end

#------------------------------------------------------------------------------
# vim: ts=3 noet ft=gdb syntax=gdb
#------------------------------------------------------------------------------
