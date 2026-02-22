#if ! defined CFG66_UTIL_STRINGCONVERSIONS_HPP
#define CFG66_UTIL_STRINGCONVERSIONS_HPP

/*
 * Copyright (C) 2015 Tim Mayberry <mojofunk@gmail.com>
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
 * \updates       2026-02-22
 * \version       $Revision$
 *
 *  This module is a reworking of the PBD code as used in the source
 *  code (string_convert.cpp) for the Ardour DAW project.
 *
 *  We removed all the debug code such as the following. Let the caller
 *  decide how to handle an error.
 */

#include <string>
#include <cstdint>

/**
 * Locale independent and thread-safe string conversion utility functions.  All
 * conversions are done as if they were performed in the C locale without
 * actually changing the current locale.
 */

namespace util
{

extern bool bool_to_string (bool val, std::string & str);
extern bool int16_to_string (int16_t val, std::string & str);
extern bool uint16_to_string (uint16_t val, std::string & str);
extern bool int32_to_string (int32_t val, std::string & str);
extern bool uint32_to_string (uint32_t val, std::string & str);
extern bool int64_to_string (int64_t val, std::string & str);
extern bool uint64_to_string (uint64_t val, std::string & str);
extern bool float_to_string (float val, std::string & str);
extern bool double_to_string (double val, std::string & str);
extern bool string_to_bool (const std::string & str, bool & val);
extern bool string_to_int16 (const std::string & str, int16_t & val);
extern bool string_to_uint16 (const std::string & str, uint16_t & val);
extern bool string_to_int32 (const std::string & str, int32_t & val);
extern bool string_to_uint32 (const std::string & str, uint32_t & val);
extern bool string_to_int64 (const std::string & str, int64_t & val);
extern bool string_to_uint64 (const std::string & str, uint64_t & val);
extern bool string_to_float (const std::string & str, float & val);
extern bool string_to_double (const std::string & str, double & val);
extern std::string capitalize (const std::string & str);
extern std::string uncapitalize (const std::string & str);
extern std::string abbreviation                         /* short_version()  */
(
    std::string orig, std::string::size_type targetlength = 3
);
extern std::string url_decode (std::string const & url);

/**
 *  This will cause a compile time error if this function is ever
 *  instantiated, which is useful to catch unintended conversions.
 */

template <class T>
inline bool
to_string (T val, std::string & str)
{
    typename T::TO_STRING_TEMPLATE_NOT_DEFINED_FOR_THIS_TYPE invalid_type;
    (void) val; (void) str;
    return false;
}

template <class T>
inline bool
to_string (bool val, std::string & str)
{
    return bool_to_string(val, str);
}

template <class T>
inline bool
to_string (int8_t val, std::string & str)
{
    return int16_to_string(val, str);
}

template <class T>
inline bool
to_string (uint8_t val, std::string & str)
{
    return uint16_to_string(val, str);
}

template <class T>
inline bool
to_string (int16_t val, std::string & str)
{
    return int16_to_string(val, str);
}

template <class T>
inline bool
to_string (uint16_t val, std::string & str)
{
    return uint16_to_string(val, str);
}

template <class T>
inline bool
to_string (int32_t val, std::string & str)
{
    return int32_to_string(val, str);
}

template <class T>
inline bool
to_string (uint32_t val, std::string & str)
{
    return uint32_to_string(val, str);
}

template <class T>
inline bool
to_string (int64_t val, std::string & str)
{
    return int64_to_string(val, str);
}

template <class T>
inline bool
to_string (uint64_t val, std::string & str)
{
    return uint64_to_string(val, str);
}

template <class T>
inline bool
to_string (float val, std::string & str)
{
    return float_to_string(val, str);
}

template <class T>
inline bool
to_string (double val, std::string & str)
{
    return double_to_string(val, str);
}

/**
 *  This will cause a compile time error if this function is ever
 *  instantiated, which is useful to catch unintended conversions.
 */

template <class T>
inline bool string_to (const std::string & str, T & val)
{
    typename T::TO_STRING_TEMPLATE_NOT_DEFINED_FOR_THIS_TYPE invalid_type;
    (void) val; (void) str;
    return false;
}

template <class T>
inline bool string_to (const std::string & str, bool & val)
{
    return string_to_bool(str, val);
}

template <class T>
inline bool string_to (const std::string & str, int8_t & val)
{
    int16_t tmp { val };
    bool success { string_to_int16(str, tmp) };
    if (! success)
        return false;

    val = tmp;
    return true;
}

template <class T>
inline bool string_to (const std::string & str, uint8_t & val)
{
    uint16_t tmp { val };
    bool success { string_to_uint16(str, tmp) };
    if (! success)
        return false;

    val = tmp;
    return true;
}

template <class T>
inline bool string_to (const std::string & str, int16_t & val)
{
    return string_to_int16(str, val);
}

template <class T>
inline bool string_to (const std::string & str, uint16_t & val)
{
    return string_to_uint16(str, val);
}

template <class T>
inline bool string_to (const std::string & str, int32_t & val)
{
    return string_to_int32(str, val);
}

template <class T>
inline bool string_to (const std::string & str, uint32_t & val)
{
    return string_to_uint32(str, val);
}

template <class T>
inline bool string_to (const std::string & str, int64_t & val)
{
    return string_to_int64(str, val);
}

template <class T>
inline bool string_to (const std::string & str, uint64_t & val)
{
    return string_to_uint64(str, val);
}

template <class T>
inline bool string_to (const std::string & str, float & val)
{
    return string_to_float(str, val);
}

template <class T>
inline bool string_to (const std::string & str, double & val)
{
    return string_to_double(str, val);
}

/*
 * Variation that disregards conversion errors.
 *
 *  This will cause a compile time error if this function is ever
 *  instantiated, which is useful to catch unintended conversions.
 */

template <class T>
inline std::string
to_string (T val)
{
    typename T::TO_STRING_TEMPLATE_NOT_DEFINED_FOR_THIS_TYPE invalid_type;
    (void) val;
    return std::string();
}

template <>
inline std::string
to_string (bool val)
{
    std::string tmp;
    bool_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (int8_t val)
{
    std::string tmp;
    int16_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (uint8_t val)
{
    std::string tmp;
    uint16_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (int16_t val)
{
    std::string tmp;
    int16_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (uint16_t val)
{
    std::string tmp;
    uint16_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (int32_t val)
{
    std::string tmp;
    int32_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (uint32_t val)
{
    std::string tmp;
    uint32_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (int64_t val)
{
    std::string tmp;
    int64_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (uint64_t val)
{
    std::string tmp;
    uint64_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (float val)
{
    std::string tmp;
    float_to_string(val, tmp);
    return tmp;
}

template <>
inline std::string
to_string (double val)
{
    std::string tmp;
    double_to_string(val, tmp);
    return tmp;
}

/**
 *  This will cause a compile time error if this function is ever
 *  instantiated, which is useful to catch unintended conversions.
 */

template <class T>
inline T
string_to (const std::string & str)
{
    typename T::STRING_TO_TEMPLATE_NOT_DEFINED_FOR_THIS_TYPE invalid_type;
    (void) str;
    return T();
}

template <>
inline bool
string_to (const std::string & str)
{
    bool tmp;
    string_to_bool(str, tmp);
    return tmp;
}

template <>
inline int8_t
string_to (const std::string & str)
{
    int16_t tmp;
    string_to_int16(str, tmp);
    return (int8_t) tmp;
}

template <>
inline uint8_t
string_to (const std::string & str)
{
    uint16_t tmp;
    string_to_uint16(str, tmp);
    return (uint8_t) tmp;
}

template <>
inline int16_t
string_to (const std::string & str)
{
    int16_t tmp;
    string_to_int16(str, tmp);
    return tmp;
}

template <>
inline uint16_t
string_to (const std::string & str)
{
    uint16_t tmp;
    string_to_uint16(str, tmp);
    return tmp;
}

template <>
inline int32_t
string_to (const std::string & str)
{
    int32_t tmp;
    string_to_int32(str, tmp);
    return tmp;
}

template <>
inline uint32_t
string_to (const std::string & str)
{
    uint32_t tmp;
    string_to_uint32(str, tmp);
    return tmp;
}

template <>
inline int64_t
string_to (const std::string & str)
{
    int64_t tmp;
    string_to_int64(str, tmp);
    return tmp;
}

template <>
inline uint64_t
string_to (const std::string & str)
{
    uint64_t tmp;
    string_to_uint64(str, tmp);
    return tmp;
}

template <>
inline float
string_to (const std::string & str)
{
    float tmp;
    string_to_float(str, tmp);
    return tmp;
}

template <>
inline double
string_to (const std::string & str)
{
    double tmp;
    string_to_double(str, tmp);
    return tmp;
}

}               // namespace util

#endif          // CFG66_UTIL_STRINGCONVERSIONS_HPP

/*
 * strconversions.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
