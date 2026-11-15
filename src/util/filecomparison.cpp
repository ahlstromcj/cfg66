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
 * \file          filecomparison.cpp
 *
 *    Provides the implementations for robust comparison of binary and
 *    text files across Windows and UNIXen.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2026-11-15
 * \updates       2026-11-15
 * \version       $Revision$
 *
 *    Note that, while some functions use the C++17 std::filesystem
 *    construct, many are still basically C code. (The same is true of
 *    the ftswalker module).
 *
 * Notes on file comparison (from AI, so be careful bro!):
 *
 *  - Windows Line-Ending Translation. On Windows, opening a file without
 *    std::ios::binary (the default for std::ifstream and std::ofstream)
 *    causes the C++ runtime to translate line endings. Writing a single \n
 *    (0x0A) outputs two bytes: Carriage Return + Line Feed (\r\n, 0x0D 0x0A).
 *    Reading \r\n strips the \r and delivers only \n to your C++ string or
 *    buffer.
 *
 *    Therefore, on Windows, ignore the extra \r character and the exact
 *    file size when comparing text files.
 *
 *  - Windows Ctrl+Z (0x1A) Truncation. In Windows text mode, encountering the
 *    ASCII Substitute character (0x1A, or Ctrl+Z) prematurely signals
 *    End-of-File (EOF). If you open an image, zip archive, or serialized
 *    struct in text mode on Windows, your program will stop reading the
 *    moment a raw data byte happens to equal 0x1A.
 *
 *    Therefore, on Windows, ignore the extra Ctrl-Z character and the exact
 *    file size when comparing text files.
 *
 *    -----------------------------------------------------------------------
 *    Solution for text files: Read the file into lib66::tokenization with
 *    trimming and ignoring comments, then compare string-by-string.
 *    -----------------------------------------------------------------------
 *
 *  - Linux Unified Behavior. POSIX systems like Linux represent newlines with
 *    a single Line Feed (\n) and do not use special EOF characters inside
 *    files.  Consequently, the Linux C++ standard library ignores the
 *    std::ios::binary flag at the OS level, treating text and binary streams
 *    identically.
 *
 *  - Cross-Platform Portability Trap. Because Linux does not penalize
 *    omitting std::ios::binary when reading raw data, bugs often go unnoticed
 *    on Linux.  Compiling that same code on Windows will silently corrupt
 *    binary data—such as std::ofstream::write() calls—whenever bytes match
 *    0x0A or 0x1A. On Reddit's r/cpp_questions, some developers note that
 *    while line endings and Ctrl+Z are the main practical differences on
 *    Windows, std::ios::binary is strictly required by the C++ standard
 *    for portable unformatted I/O across all platforms.
 *
 * Best Practices for C++ File I/O
 *
 *  - Always specify std::ios::binary for non-text data. Pass std::ios::binary
 *    whenever reading or writing images, audio, compressed files, or raw
 *    structs via .read() and .write().
 *
 *  - Use std::ios::binary for exact byte counting. Open text files with
 *    std::ios::binary if you need tellg() offsets or
 *    std::filesystem::file_size() to match the exact number of characters
 *    read into memory.
 *
 *  - Omit std::ios::binary for native line endings. Leave std::ios::binary
 *    off only when reading or writing plain text files where you want Windows
 *    to automatically handle \r\n conversions.
 *
 * Reminders about the filefunctions module:
 *
 *  - size_t file_size (filename) mentions alternative implementations.
 *  - std::FILE * file_open (filename, mode) reads in specifid mode.
 *  - std::FILE * file_open_for_read (filename) reads in binary mode.
 *  - std::FILE * file_create_for_write (filename) creates in binary mode.
 *
 *  Also see fopen(3) for reminders about how the "mode" letters work.
 */

#include <cstring>                      /* std::memcmp()                    */
#include <filesystem>                   /* std::filesystem namespace        */
#include <fstream>                      /* std::ifstream class              */

#include "cfg66-config.h"               /* HAVE macros defined by meson     */
#include "c_macros.h"                   /* not_nullptr() macro, etc.        */
#include "platform_macros.h"            /* operating system/compiler macros */
#include "util/filecomparison.hpp"      /* free functions in util n'space   */
#include "util/filefunctions.hpp"       /* util::file_read_lines()          */

namespace util
{

#if defined PLATFORM_WINDOWS
#else
#endif

/*
 *  Almost equivalent to PLATFORM_WINDOWS.
 */

#if defined PLATFORM_MING_OR_WINDOWS
#endif  // defined PLATFORM_MING_OR_WINDOWS

/**
 *  Compares two binary files for an exact match. Useful in test programs.
 *
 *  We use std::filesystem rather than the filefunctions module. The
 *  steps are:
 *
 *      -   Check if both files exist and are regular files.
 *      -   Fast-fail if file sizes differ.
 *      -   Open both files in binary mode.
 *      -   Compare in 64 KB chunks.
 *
 *  An alternative for testing against a golden file would be to calculate
 *  the hash and compare it to the pre-calculated hash of the golden file.
 */

bool
compare_binary_files (const std::string & bfile0, const std::string & bfile1)
{
    namespace fs = std::filesystem;

    bool result
    {
        fs::is_regular_file(bfile0) && fs::is_regular_file(bfile1)
    };
    if (result)
        result = fs::file_size(bfile0) == fs::file_size(bfile1);

    if (result)
    {
        std::ifstream if0(bfile0, std::ios::binary);
        std::ifstream if1(bfile1, std::ios::binary);
        result = if0.is_open() && if1.is_open();
        if (result)
        {
            constexpr std::size_t buffsz { 65536 };     /* 64 Kb chunks     */
            std::vector<char> buff0(buffsz);
            std::vector<char> buff1(buffsz);
            while (if0 && if1)
            {
                if0.read(buff0.data(), buffsz);
                if1.read(buff1.data(), buffsz);

                std::streamsize count0 { if0.gcount() };
                std::streamsize count1 { if1.gcount() };
                if (count0 != count1)                   /* can this happen? */
                {
                    result = false;
                    break;
                }
                if (count0 > 0)
                {
                    int rc { std::memcmp(buff0.data(), buff1.data(), count0) };
                    result = rc == 0;
                    if (! result)
                        break;
                }
            }
        }
    }
    return result;
}

/**
 *  This function compares lines of a text file, even if blank.
 *  They must match exactly.
 */

bool
compare_line_files (const std::string & file0, const std::string & file1)
{
    namespace fs = std::filesystem;

    bool result
    {
        fs::is_regular_file(file0) && fs::is_regular_file(file1)
    };
    if (result)
    {
        std::ifstream if0(file0);
        std::ifstream if1(file1);
        result = if0.is_open() && if1.is_open();
        if (result)
        {
            std::string line0;
            std::string line1;
            for (;;)
            {
                (void) std::getline(if0, line0);
                (void) std::getline(if1, line1);
                if (if0 && if1)
                {
                    result = line0 == line1;
                    if (! result)
                        break;
                }
                else
                    break;
            }
        }
    }
    return result;
}

/**
 *  This function compares lines of a text file. But here, comments
 *  and whitespace are ignored or trimmed. Blank lines are ignored.
 *  We use an inefficient function in the filefunctions module, so
 *  this function is suitable for "small" configuration files or
 *  the "Midi66 Transcripts" of the Midi66 library.
 *
 *  The content lines must be of the same number and in the same order
 *  in each file, after stripping.
 */

bool
compare_file_content (const std::string & file0, const std::string & file1)
{
    lib66::tokenization lines0;
    lib66::tokenization lines1;
    bool result { file_read_lines(file0, lines0, true) };
    if (result)
        result = file_read_lines(file1, lines1, true);

    if (result)
        result = lines0 == lines1;

    return result;
}

}           // namespace util

/*
 * filecomparison.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
