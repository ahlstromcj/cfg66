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
 * \file          msgfunctions.cpp
 *
 *  This module defines some informative functions that are actually
 *  better off as functions.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2018-11-10
 * \updates       2024-10-28
 * \license       GNU GPLv2 or above
 *
 *  One of the big features of some of these functions is writing the name
 *  of the application in color before each message that is put out.
 */

#include <cstring>                      /* std::strlen(3)                   */
#include <cstdarg>                      /* see "man stdarg(3)"              */
#include <iostream>

#include "cpp_types.hpp"                /* not_nullptr(), lib66::msglevel   */
#include "platform_macros.h"            /* detects the build platform       */
#include "cfg/appinfo.hpp"              /* application info functions       */
#include "util/msgfunctions.hpp"        /* basic message functions          */

#if defined PLATFORM_UNIX

#include <unistd.h>                     /* C::write(2), STDOUT_FILENO, etc  */
#define S_WRITE     write               /* POSIX write(2)                   */

#elif defined PLATFORM_WINDOWS            /* Microsoft platform               */

#include <io.h>                         /* C::_write()                      */

#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2

#if defined PLATFORM_MSVC               /* Microsoft compiler vs MingW      */
#define S_WRITE     _write              /* Microsoft's write()              */
#else                                   /* probably Cygwin or Mingw         */
#define S_WRITE     write               /* POSIX write(2)                   */
#endif

#else                                   /* Other environments, like Msys2   */

#include <unistd.h>                     /* C::write(2), STDOUT_FILENO, etc  */
#define S_WRITE     write               /* POSIX write(2)                   */

#endif

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace util
{

/**
 *  Functions to allow independence from cli::parser. The parser will
 *  also set these options.
 */

static bool s_is_quiet = false;
static bool s_is_verbose = false;
static bool s_is_investigate = false;

void
set_quiet (bool flag)
{
    s_is_quiet = flag;
}

bool
quiet ()
{
    return s_is_quiet;
}

void
set_verbose (bool flag)
{
    s_is_verbose = flag;
}

bool
verbose ()
{
    return s_is_verbose;
}

void
set_investigate (bool flag)
{
    s_is_investigate = flag;
}

bool
investigate ()
{
    return s_is_investigate;
}

/**
 *  Provided for convenience and for avoid those annoying warnings about
 *  "declared with attribute warn_unused_result [-Wunused-result]".
 */

static void
write_msg (int fd, const char * msg, size_t count)
{
    if (S_WRITE(fd, msg, count) == (-1))
    {
        /*
         *  Generally should fail only if interrupted by a signal-handler
         *  before any bytes are written.  See the man-page for write(2).
         */
    }
}

/**
 *  Meant for use in signal handlers.  For the colors, hardwired here, see
 *  s_level_colors in the appinfo.cpp module. The "cfg66" tag is
 *  black (no error) or red (error).  The text is black.  The character count
 *  is programmer supplied (see the comments).
 *
 *  No longer needed: static const char * s_label = "[cfg66] ";
 */

static const char * s_start = "[\033[1;30mcfg66\033[0m] \033[1;30m";    // 26
static const char * s_error = "[\033[1;31mcfg66\033[0m] \033[1;30m";    // 26
static const char * s_eol   = "\033[0m\n";                              //  5
static const char * s_nl    = "\n";                                     //  1

void
async_safe_strprint (const char * msg, bool colorit)
{
    if (not_nullptr(msg))
    {
        size_t count = std::strlen(msg);
        if (count > 0)
        {
            if (cfg::is_a_tty(STDOUT_FILENO) && colorit)
            {
                write_msg(STDOUT_FILENO, s_start, 26);
                write_msg(STDOUT_FILENO, msg, count);
                write_msg(STDOUT_FILENO, s_eol, 5);
            }
            else
            {
                const char * label = cfg::get_client_name().c_str();
                size_t szlabel = cfg::get_client_name().size() + 2; // "[xxx]"
                write_msg(STDOUT_FILENO, label, szlabel);
                write_msg(STDOUT_FILENO, msg, count);
                write_msg(STDOUT_FILENO, s_nl, 1);
            }
        }
    }
}

void
async_safe_errprint (const char * msg, bool colorit)
{
    if (not_nullptr(msg))
    {
        size_t count = std::strlen(msg);
        if (count > 0)
        {
            if (cfg::is_a_tty(STDERR_FILENO) && colorit)
            {
                write_msg(STDERR_FILENO, s_error, 26);
                write_msg(STDERR_FILENO, msg, count);
                write_msg(STDERR_FILENO, s_eol, 5);
            }
            else
            {
                const char * label = cfg::get_client_name().c_str();
                size_t szlabel = cfg::get_client_name().size() + 2; // "[xxx]"
                write_msg(STDERR_FILENO, label, szlabel);
                write_msg(STDERR_FILENO, msg, count);
                write_msg(STDERR_FILENO, s_nl, 1);
            }
        }
    }
}

/**
 *  This function assumes the programmer knows what she's doing.  The pointer
 *  should be good and the buffer should be 24 characters.  After getting the
 *  digits, the count is the number of digits, which is 1 at a minimum.
 *
 * \param destination
 *      Provides a 24-byte buffer to hold the resulting string.  Assumed to be
 *      valid and at least that large, for speed.
 *
 * \param number
 *      The unsigned value to convert to an ASCII null-terminated string.
 *
 * \param spacebefore
 *      If true (the default), then output a space first.  This helps in
 *      printing a number of values rapidly in a row.
 */

void
async_safe_utoa (char * destination, unsigned number, bool spacebefore)
{
    const unsigned ascii_base = unsigned('0');
    char reversed[c_async_safe_utoa_size];
    int count = 0;
    do
    {
        unsigned remainder = number % 10;
        reversed[count++] = char(remainder) + ascii_base;
        number /= 10;

    } while (number != 0);

    int index = 0;
    int limit = count;
    if (spacebefore)
    {
        destination[index++] = ' ';
        ++limit;
    }
    for ( ; index < limit; ++index)
    {
        --count;
        destination[index] = reversed[count];
    }
    destination[index] = 0;             /* append the string terminator */
}

/**
 *  Common-code for console informationational messages.  Adds markers and a
 *  newline.
 *
 * \param msg
 *      The message to print, sans the newline.
 *
 * \param data
 *      Additional information about the message. Optional.
 *
 * \return
 *      Returns true, so that the caller can show the message and return the
 *      status at the same time.
 */

bool
info_message (const std::string & msg, const std::string & data)
{
    if (verbose())
    {
        std::cout << cfg::get_client_tag(lib66::msglevel::info) << " " << msg;
        if (! data.empty())
            std::cout << ": " << data;

        if (! msg.empty())
            std::cout << std::endl;
    }
    return true;
}

bool
status_message (const std::string & msg, const std::string & data)
{
    std::cout << cfg::get_client_tag(lib66::msglevel::status) << " " << msg;
    if (! data.empty())
        std::cout << ": " << data;

    if (! msg.empty())
        std::cout << std::endl;

    return true;
}

bool
session_message (const std::string & msg, const std::string & data)
{
    std::cout << cfg::get_client_tag(lib66::msglevel::session) << " " << msg;
    if (! data.empty())
        std::cout << ": " << data;

    if (! msg.empty())
        std::cout << std::endl;

    return true;
}

/**
 *  Common-code for console warning messages.  Adds markers and a newline.
 *
 * \param msg
 *      The message to print, sans the newline.
 *
 * \param data
 *      Additional information about the message. Optional.
 *
 * \return
 *      Returns true, so that the caller can show the message and return the
 *      status at the same time.
 */

bool
warn_message (const std::string & msg, const std::string & data)
{
    std::cerr << cfg::get_client_tag(lib66::msglevel::warn) << " " << msg;
    if (! data.empty())
        std::cerr << ": " << data;

    if (! msg.empty())
        std::cerr << std::endl;

    return true;
}

/**
 *  Common-code for error messages.  Adds markers, and returns false.
 *
 * \param msg
 *      The message to print, sans the newline.
 *
 * \param data
 *      Additional information about the message. Optional.
 *
 * \return
 *      Returns false for convenience/brevity in setting function return
 *      values.
 */

bool
error_message (const std::string & msg, const std::string & data)
{
    std::cerr << cfg::get_client_tag(lib66::msglevel::error) << " " << msg;
    if (! data.empty())
        std::cerr << ": " << data;

    if (! msg.empty())
        std::cerr << std::endl;

    return false;
}

/**
 *  More sneaky escape sequences for coloring.
 */

static const char * s_black  = "\033[1;30m";
static const char * s_normal = "\033[0m";

/**
 *  Common-code for debug messages.  Adds markers, and returns false.
 *
 * \param msg
 *      The message to print, sans the newline.
 *
 * \param data
 *      Additional information about the error. Optional.
 *
 * \return
 *      Returns true.  The return value here is rarely used, if at all.
 */

bool
debug_message (const std::string & msg, const std::string & data)
{
    if (investigate())
    {
        std::cerr << cfg::get_client_tag(lib66::msglevel::debug) << " ";
        if (cfg::is_a_tty(STDERR_FILENO))
            std::cerr << s_black;

        std::cerr << msg;
        if (! data.empty())
            std::cerr << ": " << data;

        if (! msg.empty())
        {
            if (cfg::is_a_tty(STDERR_FILENO))
                std::cerr << s_normal << std::endl;
            else
                std::cerr << std::endl;
        }
    }
    return true;
}

/**
 *  Common-code for error messages involving file issues, a very common use
 *  case in error-reporting.  Adds markers, and returns false.
 *
 * \param tag
 *      The message to print, sans the newline.
 *
 * \param path
 *      The name of the file to be shown.
 *
 * \return
 *      Returns false for convenience/brevity in setting function return
 *      values.
 */

bool
file_error (const std::string & tag, const std::string & path)
{
    std::cerr << cfg::get_client_tag(lib66::msglevel::error) << " "
        << tag << ": '" << path << "'" << std::endl;

    return false;
}

/**
 *  Shows a path-name (or other C++ string) as an info message.  This output
 *  is not contingent upon debugging or verbosity.
 *
 * \param tag
 *      Provides the text to precede the name of the path.
 *
 * \param path
 *      Provides the path-name to print.  This message can be something other
 *      than a path-name, by the way.
 */

void
file_message (const std::string & tag, const std::string & path)
{
    std::cout << cfg::get_client_tag(lib66::msglevel::status) << " "
        << tag << ": '" << path << "'" << std::endl;
}

/**
 *  This function just prints a colored tag to the proper output based on
 *  message level.
 */

void
print_client_tag (lib66::msglevel el)
{
    std::string tag = cfg::get_client_tag(el);
    bool iserror = el == lib66::msglevel::error ||
        el == lib66::msglevel::warn ||
        el == lib66::msglevel::debug;

    tag += " ";
    if (iserror)
        std::cerr << tag;
    else
        std::cout << tag;
}

/**
 *  Takes a format string and a variable-argument list and returns the
 *  formatted string.
 *
 *  Although currently a public function, its usage is meant to be internal
 *  for the msgprintf() function.  See that function's description.
 *
 *  C++11 is required, due to the use of va_copy().
 *
 * \param fmt
 *      Provides the printf() format string.
 *
 * \param args
 *      Provides the variable-argument list.
 *
 * \return
 *      Returns the formatted string.  If an error occurs, the string is
 *      empty.
 */

static std::string
formatted (const std::string & fmt, va_list args)
{
    std::string result;
    va_list args_copy;                                      /* Step 2       */
    va_copy(args_copy, args);

    const char * const szfmt = fmt.c_str();
    int ilen = std::vsnprintf(NULL, 0, szfmt, args_copy);
    va_end(args_copy);
    if (ilen > 0)
    {
        std::vector<char> dest(ilen + 1);                   /* Step 3       */
        std::vsnprintf(dest.data(), dest.size(), szfmt, args);
        result = std::string(dest.data(), dest.size() - 1);
    }
    va_end(args);
    return result;
}

/**
 *  Shows a boolean value as a "true/false" info message.
 *
 * \param tag
 *      Provides the text to precede the boolean value.
 *
 * \param flag
 *      Provides the boolean value to print.
 */

void
boolprint (const std::string & tag, bool flag)
{
    std::string fmt = tag + " %s";
    msgprintf(lib66::msglevel::info, fmt, flag ? "true" : "false");
}

/**
 *  Shows a boolean value as an "on/off" info message.
 *
 * \param tag
 *      Provides the text to precede the boolean value.
 *
 * \param flag
 *      Provides the boolean value to print.
 */

void
toggleprint (const std::string & tag, bool flag)
{
    std::string fmt = tag + " %s";
    msgprintf(lib66::msglevel::info, fmt, flag ? "on" : "off");
}

/**
 *  Formats a variable list of input and returns the formatted string.
 *  Not the fastest function ever, but useful.  Adapted from the last sample
 *  in:
 *
 *      https://stackoverflow.com/questions/19009094/
 *              c-variable-arguments-with-stdstring-only
 *
 *  -#  Initialize the usage of a variable-argument array.
 *  -#  Acquire the destination size from a copy of the variable-argument
 *      array; mock the formatting with vsnprintf() and a null destination to
 *      find out how many characters will be constructed.  The copy is
 *      necessary to avoid messing up the original variable-argument array.
 *  -#  Return a formatted string without needing memory management, and
 *      without assuming any compiler/platform-specific behavior.
 *
 * \param lev
 *      Indicates the desired message level: info, warn, or error.
 *
 * \param fmt
 *      Indicates the desired format for the message.  Use "%s" for strings.
 *
 * \param ...
 *      Provides the printf() parameters for the format string.  Please note
 *      that C++ strings cannot be used directly... std::string::c_str() must
 *      be used.
 */

void
msgprintf (lib66::msglevel lev, std::string fmt, ...)
{
    if (! fmt.empty())
    {
        /*
         * cppcheck: Using reference 'fmt' as parameter for va_start() results
         * in undefined behaviour.  Also claims we need to add a va_end(), so
         * we did, below, on 2019-04-21.
         */

        va_list args;                                       /* Step 1       */
        va_start(args, fmt);

        std::string output = formatted(fmt, args);          /* Steps 2 & 3  */
        switch (lev)
        {
        case lib66::msglevel::none:

            std::cout
                << cfg::get_client_tag(lev) << " " << output << std::endl;
            break;

        case lib66::msglevel::info:

            if (verbose())
            {
                std::cout
                    << cfg::get_client_tag(lev) << " " << output << std::endl;
            }
            break;

        case lib66::msglevel::status:
        case lib66::msglevel::session:

            std::cout
                << cfg::get_client_tag(lev) << " " << output << std::endl;
            break;

        case lib66::msglevel::warn:
        case lib66::msglevel::error:
        case lib66::msglevel::debug:

            std::cerr
                << cfg::get_client_tag(lev) << " " << output << std::endl;
            break;
        }
        va_end(args);                                       /* 2019-04-21   */
    }
}

/**
 *  Acts like msgprintf(), but returns the result as a string, and doesn't
 *  bother with "info level" and whether we're debugging or not.
 *
 * \param fmt
 *      Indicates the desired format for the message.  Use "%s" for strings.
 *
 * \param ...
 *      Provides the printf() parameters for the format string.  Please note
 *      that C++ strings cannot be used directly... std::string::c_str() must
 *      be used.
 */

std::string
msgsnprintf (std::string fmt, ...)
{
    std::string result;
    if (! fmt.empty())
    {
        va_list args;
        va_start(args, fmt);
        result = formatted(fmt, args);
        va_end(args);
    }
    return result;
}

}           // namespace util

/*
 * msgfunctions.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

