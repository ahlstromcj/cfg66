/*
 * Copyright (C) 2015-2017 Tim Mayberry <mojofunk@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  This file has been modified to be part of cfg66. It is a reworking
 *  of the PBD code as used in the source code for the Ardour DAW project.
 *  We got rid of dependencies on glib and pbd functions.
 */

/**
 * \file          strconversions.cpp
 *
 *    Provides the implementations for safe replacements for the various C
 *    file functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2026-02-20
 * \updates       2026-02-20
 * \version       $Revision$
 *
 *  This module is a reworking of the PBD code as used in the source
 *  code (string_convert.cpp) for the Ardour DAW project.
 *
 *  We removed all the debug code such as the following. Let the caller
 *  decide how to handle an error.
 *
 *      DEBUG_SCONVERT
 *      (
 *          string_compose("float_to_string conversion failure for %1", val)
 *      );
 */

#if ! defined __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <cerrno>                       /* provides the errno macro         */
#include <cinttypes>                    /* provides macros like SCNu16      */
#include <cstdio>                       /* std::sscanf()                    */
#include <limits>                       /* std::numeric_limits<>            */

#include "util/strconversions.hpp"      /* string / numeric conversions     */
#include "util/strfunctions.hpp"        /* other string functions & macros  */

// #include <glib.h>
// #include <glib/gprintf.h>
// #include "pbd/compose.h"
// #include "pbd/debug.h"
// #include "pbd/i18n.h"

// TO DO: incorporate potext

#define X_(Text) Text

#define CONVERT_BUFFER_SIZE     32

namespace util
{

bool
string_to_bool (const std::string & str, bool & val)
{
    val = util::string_to_bool_ex(str);
    return true;
}

bool
string_to_int16 (const std::string & str, int16_t & val)
{
    return std::sscanf(CSTR(str), "%" SCNi16, &val) == 1;
}

bool
string_to_uint16 (const std::string & str, uint16_t & val)
{
    return std::sscanf(CSTR(str), "%" SCNu16, &val) == 1;
}

bool
string_to_int32 (const std::string & str, int32_t & val)
{
    return std::sscanf(CSTR(str), "%" SCNi32, &val) == 1;
}

bool
string_to_uint32 (const std::string & str, uint32_t & val)
{
    return std::sscanf(CSTR(str), "%" SCNu32, &val) == 1;
}

bool
string_to_int64 (const std::string & str, int64_t & val)
{
    return std::sscanf(CSTR(str), "%" SCNi64, &val) == 1;
}

bool
string_to_uint64 (const std::string & str, uint64_t & val)
{
    return std::sscanf(CSTR(str), "%" SCNu64, &val) == 1;
}

/*
 * An internal helper function.
 */

template <class FloatType>
static bool
s_string_to_infinity (const std::string & str, FloatType & val)
{
    if
    (
        ! util::strncasecompare(str, X_("inf")) ||
        ! util::strncasecompare(str, X_("+inf")) ||
        ! util::strncasecompare(str, X_("INFINITY")) ||
        ! util::strncasecompare(str, X_("+INFINITY"))
    )
    {
        val = std::numeric_limits<FloatType>::infinity();
        return true;
    }
    else if
    (
        ! util::strncasecompare(str, X_("-inf")) ||
        ! util::strncasecompare(str, X_("-INFINITY"))
    )
    {
        val = -std::numeric_limits<FloatType>::infinity();
        return true;
    }
    return false;
}

/**
 * It is possible that the conversion was successful and another thread
 * has set errno meanwhile but as most conversions are currently not
 * checked for error conditions this is better than nothing.
 *
 * (No error-checking anymore.)
 *
 * An internal helper function.
 */

static bool
s_string_to_double (const std::string & str, double & val)
{
    val = util::string_to_double_ex(str);       /* 0.0 default, no rounding */
    return true;
}

bool
string_to_float (const std::string & str, float & val)
{
    double tmp;
    if (s_string_to_double(str, tmp))
    {
        val = float(tmp);
        return true;
    }
    if (s_string_to_infinity(str, val))
    {
        return true;
    }
    return false;
}

bool
string_to_double (const std::string & str, double & val)
{
    if (s_string_to_double(str, val))
    {
        return true;
    }
    if (s_string_to_infinity(str, val))
    {
        return true;
    }
    return false;
}

bool
bool_to_string (bool val, std::string & str)
{
    if (val)
    {
        str = X_("1");
    }
    else
    {
        str = X_("0");
    }
    return true;
}

bool
int16_to_string (int16_t val, std::string & str)
{
    char tmp [CONVERT_BUFFER_SIZE];
    int retval { snprintf(tmp, sizeof tmp, "%" PRIi16, val) };
    if (retval <= 0 || retval >= int(sizeof(tmp)))
        return false;

    str = tmp;
    return true;
}

bool
uint16_to_string (uint16_t val, std::string & str)
{
    char tmp [CONVERT_BUFFER_SIZE];
    int retval { snprintf(tmp, sizeof tmp, "%" PRIu16, val) };
    if (retval <= 0 || retval >= int(sizeof(tmp)))
        return false;

    str = tmp;
    return true;
}

bool
int32_to_string (int32_t val, std::string & str)
{
    char tmp [CONVERT_BUFFER_SIZE];
    int retval { snprintf(tmp, sizeof tmp, "%" PRIi32, val) };
    if (retval <= 0 || retval >= int(sizeof(tmp)))
        return false;

    str = tmp;
    return true;
}

bool
uint32_to_string (uint32_t val, std::string & str)
{
    char tmp [CONVERT_BUFFER_SIZE];
    int retval { snprintf (tmp, sizeof tmp, "%" PRIu32, val) };
    if (retval <= 0 || retval >= int(sizeof(tmp)))
        return false;

    str = tmp;
    return true;
}

bool
int64_to_string (int64_t val, std::string & str)
{
    char tmp [CONVERT_BUFFER_SIZE];
    int retval { snprintf (tmp, sizeof(tmp), "%" PRIi64, val) };
    if (retval <= 0 || retval >= int(sizeof(tmp)))
        return false;

    str = tmp;
    return true;
}

bool
uint64_to_string (uint64_t val, std::string & str)
{
    char tmp [CONVERT_BUFFER_SIZE];
    int retval { snprintf (tmp, sizeof tmp, "%" PRIu64, val) };
    if (retval <= 0 || retval >= int(sizeof(tmp)))
        return false;

    str = tmp;
    return true;
}

/*
 * An internal helper function.
 */

template <class FloatType>
static bool
s_infinity_to_string (FloatType val, std::string & str)
{
    if (val == std::numeric_limits<FloatType>::infinity())
    {
        str = "inf";
        return true;
    }
    else if (val == -std::numeric_limits<FloatType>::infinity())
    {
        str = "-inf";
        return true;
    }
    return false;
}

/*
 * An internal helper function.
 */

static bool
s_double_to_string (double val, std::string & str)
{
    str = util::double_to_string(val);              /* no precision */
    return true;
}

bool
float_to_string (float val, std::string & str)
{
    if (s_infinity_to_string(val, str))
    {
        return true;
    }
    if (s_double_to_string(val, str))
    {
        return true;
    }
    return false;
}

bool
double_to_string (double val, std::string & str)
{
    if (s_infinity_to_string(val, str))
    {
        return true;
    }
    if (s_double_to_string(val, str))
    {
        return true;
    }
    return false;
}

} // namespace util

/*
 * strconversions.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
