#if ! defined CFG66_UTIL_STRFUNCTIONS_HPP
#define CFG66_UTIL_STRFUNCTIONS_HPP

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
 * \file          strfunctions.hpp
 *
 *    Provides the declarations for safe replacements for some C++
 *    file functions.
 *
 * \author        Chris Ahlstrom
 * \date          2018-11-23
 * \updates       2025-02-09
 * \version       $Revision$
 *
 *    Also see the strfunctions.cpp module.
 */

/**
 *  This function comes, slightly modified to avoid throwing an exception,
 *  from:
 *
 * https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
 *
 *  Extra space for '\0', but it won't be included in the result.
 *
 *  It is useful for C++11.  Once C++20 becomes common, the following could be
 *  used:
 *
\verbatim
 *      #include <format>
 *      std::string result = std::format("{} {}!", "Hello", "world");
\endverbatim
 */

#include "cpp_types.hpp"                /* std::string, tokenization alias  */

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

#define CSTR(x) x.c_str()
#define V(x)    x.c_str()

#endif  // CFG66_STRING_FORMAT_FUNCTION

namespace util
{

/**
 *  Lists of characters to trim from strings.
 */

const std::string CFG66_TRIM_CHARS        = " \t\r\n\v\f";
const std::string CFG66_TRIM_CHARS_QUOTES = " \t\r\n\v\f\"'";
const std::string CFG66_TRIM_CHARS_PATHS  = " /\\";
const std::string CFG66_WHITE_CHARS       = " \t\r\n\v\f";

/**
 *  Inline functions.  Simple tests for std::string::npos. Why? I dunno, less
 *  typing for the developer.
 */

inline bool
not_npos (std::string::size_type p)
{
    return p != std::string::npos;
}

inline bool
is_npos (std::string::size_type p)
{
    return p == std::string::npos;
}

/*
 * This free function is actually defined in msgprintf, but we might
 * need it without including msgprintf.hpp.
 */

extern std::string string_asprintf (std::string fmt, ...);

/*
 * Global (free) string functions.
 */

extern bool is_empty_string (const std::string & item);
extern std::string empty_string ();
extern bool is_questionable_string (const std::string & item);
extern const std::string & questionable_string ();
extern bool is_missing_string (const std::string & item);
extern bool contains (const std::string & original, const std::string & target);
extern bool contains (const std::string & original, char c);
extern std::string strip_comments (const std::string & item);
extern std::string strip_quotes (const std::string & item);
extern std::string next_quoted_string
(
    const std::string & source,
    std::string::size_type pos = 0
);
extern std::string next_bracketed_string
(
    const std::string & source,
    std::string::size_type pos = 0
);
extern std::string add_quotes (const std::string & item);
extern const std::string & double_quotes ();
extern bool strncompare
(
    const std::string & a, const std::string & b, size_t n = 0
);
extern bool strcasecompare (const std::string & a, const std::string & b);
extern std::string & ltrim
(
    std::string & str,
    const std::string & chars = CFG66_TRIM_CHARS
);
extern std::string & rtrim
(
    std::string & str,
    const std::string & chars = CFG66_TRIM_CHARS
);
extern std::string trim
(
    const std::string & str,
    const std::string & chars = CFG66_TRIM_CHARS
);
extern std::string string_replace
(
    const std::string & source,
    const std::string & target,
    const std::string & replacement,
    int n = -1
);
extern int hex_digit (char c);
extern std::string string_to_midi_bytes (const std::string & s, size_t lim = 0);
extern std::string midi_bytes_to_string (const std::string & s);
extern bool string_to_bool (const std::string & s, bool defalt = false);
extern bool string_to_time_signature
(
    const std::string & s,
    int & beats, int & width
);
extern std::string time_signature_string (int beats, int width);
extern bool string_to_int_pair
(
    const std::string & s,
    int & v1, int & v2,
    const std::string & delimiter = " "
);
extern double string_to_double
(
    const std::string & s,
    double defalt = 0.0,
    int rounding = 0
);
extern bool is_floating_string (const std::string & value);
extern std::string double_to_string(double value, int precision = 0);
extern float string_to_float
(
    const std::string & s, float defalt = 0.0, int rounding = 0
);
extern long string_to_long (const std::string & s, long defalt = 0L);
extern std::string long_to_string (long value);
extern unsigned long string_to_unsigned_long
(
    const std::string & s, unsigned long defalt = 0UL
);
extern unsigned string_to_unsigned
(
    const std::string & s, unsigned defalt = 0U
);
extern int string_to_int (const std::string & s, int defalt = 0);
extern std::string int_to_string (int value);
extern bool string_not_void (const std::string & s);
extern bool string_is_void (const std::string & s);
extern bool strings_match (const std::string & target, const std::string & x);
extern std::string tolower (const std::string & source);
extern std::string toupper (const std::string & source);
extern std::string capitalize (const std::string & source);
extern std::string bool_to_string (bool x, bool yesno = false);
extern char bool_to_char (bool x);
extern std::string pointer_to_string (void * ptr);
extern int tokenize_stanzas
(
    lib66::tokenization & tokens,
    const std::string & source,
    std::string::size_type bleft = 0,
    const std::string & brackets = ""
);
extern lib66::tokenization tokenize
(
    const std::string & source,
    const std::string & delimiters = " \t"
);
extern lib66::tokenization tokenize_quoted (const std::string & source);
extern std::string simplify (const std::string & source);
extern std::wstring widen_string (const std::string & source);
extern std::string word_wrap
(
    const std::string & source,
    size_t margin       = 80,
    char commentchar    = 0
);
extern std::string hanging_word_wrap
(
    const std::string & source,
    size_t leftmargin   = 28,
    size_t rightmargin  = 80
);
extern std::string line_comments
(
    const std::string & source,
    char commentchar = 0
);
extern std::string first_sentence
(
    const std::string & source,
    size_t limit    = 72,
    char ender      = '.'
);
extern int count_character (const std::string & s, char target = '\n');
extern bool target_terminated (const std::string & s, char target = '\n');

/*--------------------------------------------------------------------------
 * Functions that support nsm66. From NSM's file module.
 *--------------------------------------------------------------------------*/

extern std::string simple_hash (const std::string & s);

}           // namespace util

#endif      // CFG66_UTIL_STRFUNCTIONS_HPP

/*
 * strfunctions.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

