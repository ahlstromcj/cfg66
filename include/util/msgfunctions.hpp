#if ! defined CFG66_UTIL_MSGFUNCTIONS_HPP
#define CFG66_UTIL_MSGFUNCTIONS_HPP

/*
 *  This file is part of cfg66.
 *
 *  cfg66 is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  cfg66 is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with cfg66; if not, write to the Free Software Foundation, Inc., 59 Temple
 *  Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          msgfunctions.hpp
 *
 *    This module provides macros for generating simple messages, MIDI
 *    parameters, and more.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom and other authors; see documentation
 * \date          2018-11-10
 * \updates       2025-02-13
 * \version       $Revision$
 * \license       GNU GPL v2 or above
 *
 *  This file defines functions for C++11 (or greater) code.
 *
 *  These items were extracted from Seq66's C++ basic_macros module.
 */

#include "cpp_types.hpp"                /* string, vector, lib66::msglevel  */

#if ! defined CFG66_STRING_FORMAT_FUNCTION
#define CFG66_STRING_FORMAT_FUNCTION

#include <cstdio>                       /* std::snprintf() function         */
#include <memory>                       /* std::unique_ptr<> template class */

namespace util
{

/*
 *  Provides a function similar to asprintf(). It first gets the size
 *  of the result by passing nullptr and 0 as the destination buffer.
 *  It then allocates a buffer and does the actual writing.
 *  Compare to string_asnprintf() in the msgfunctions module.
 *
 *  Note that the V() macro must wrap std::string arguments.
 */

template<typename ... Args>
std::string string_format (const std::string & format, Args ... args)
{
    std::string result;
    size_t sz = std::snprintf(nullptr, 0, format.c_str(), args ...);
    if (sz > 0)
    {
        std::unique_ptr<char []> buf(new char[sz + 1]);
        std::snprintf(buf.get(), sz + 1, format.c_str(), args ...);
        result = std::string(buf.get(), buf.get() + sz);
    }
    return result;
}

}               // namespace util

/**
 *  Since strings are not POD, Clang will error on them when passed to
 *  a variadic function. We could use c_str() directly. Sigh. Let's
 *  make a simple macro for that. We use a macro to avoid multiple definitions
 *  of this function.
 */

#define STR(x)  const_cast<char *>(x.c_str())
#define CSTR(x) x.c_str()
#define V(x)    x.c_str()

#endif  // CFG66_STRING_FORMAT_FUNCTION

namespace util
{

/*
 *  Used in async_safe_utoa().
 */

const int c_async_safe_utoa_size = 24;

/*
 * Global functions. There is also a quiet and a verbose setting in
 * cli::parser.
 */

extern void set_quiet (bool flag);
extern bool quiet ();

extern void set_verbose (bool flag);
extern bool verbose ();

extern void set_investigate (bool flag);
extern bool investigate ();

/*
 *  Free functions in the util namespace.
 */

extern std::string string_asprintf (std::string fmt, ...);
extern bool info_message
(
    const std::string & msg,
    const std::string & data = ""
);
extern bool info_printf (std::string fmt, ...);
extern bool status_message
(
    const std::string & msg,
    const std::string & data = ""
);
extern bool status_printf (std::string fmt, ...);
extern bool warn_message
(
    const std::string & msg,
    const std::string & data = ""
);
extern bool warn_printf (std::string fmt, ...);
extern bool error_message
(
    const std::string & msg,
    const std::string & data = ""
);
extern bool error_printf (std::string fmt, ...);
extern bool debug_message
(
    const std::string & msg,
    const std::string & data = ""
);
extern bool session_message
(
    const std::string & msg,
    const std::string & data = ""
);
extern bool session_printf (std::string fmt, ...);
extern void file_message (const std::string & tag, const std::string & path);
extern bool file_error (const std::string & tag, const std::string & filename);
extern void print_client_tag (lib66::msglevel el);
extern void boolprint (const std::string & tag, bool flag);
extern void toggleprint (const std::string & tag, bool flag);
extern void async_safe_strprint (const char * msg, bool colorit = true);
extern void async_safe_errprint (const char * msg, bool colorit = true);
extern void async_safe_utoa
(
    char * destination,
    unsigned number,
    bool spacebefore = true
);
extern void msgprintf (lib66::msglevel lev, std::string fmt, ...);
extern std::string msgsnprintf (std::string fmt, ...);

}               // namespace util

#endif          // CFG66_UTIL_MSGFUNCTIONS_HPP

/*
 * msgfunctions.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

