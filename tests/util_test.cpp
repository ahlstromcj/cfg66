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
 * \file          util_test.cpp
 *
 *      A test-file for the some problematic messages code.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2025-02-07
 * \updates       2026-05-21
 * \license       See above.
 *
 *  We generally test only newly-added functions here; others were
 *  tested in other working applications, such as Seq66.
 *
 *  Run this test from the root directory:
 *
 *      $ ./build/cc/tests/util_test
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout, set::cerr             */

#include "cfg/appinfo.hpp"              /* cfg::get_xxx_details() functions */
#include "cli/parser.hpp"               /* cli::parser, etc.                */
#include "util/filefunctions.hpp"       /* util::file_read_lines()          */
#include "util/msgfunctions.hpp"        /* util::string_format(), V()       */
#include "util/strfunctions.hpp"        /* util::string_format(), V()       */

namespace       // anonymous
{

/*
 * Application information. None so far.
 */

/*
 * Tests
 */

/*
 *  Tests the function string_format() and string_asprintf() in the
 *  util namespace.
 */

bool
test_string_formatting ()
{
    std::string msg_1 { "This is a message about" };
    std::string msg_2 { "variadic functions" };
    std::string target
    {
        "MSG: This is a message about: variadic functions."
    };
    std::string output
    {
        util::string_format             /* V() is "vararg" to get POD   */
        (
            "MSG: %s: %s.", V(msg_1), V(msg_2)
        )
    };
    bool success { output == target };
    if (success)
    {
        output = util::string_asprintf("MSG: %s: %s.", V(msg_1), V(msg_2));
        success = output == target;
    }
    return success;
}

/*
 *  Tests util::file_read_lines(). Note that this function gets only
 *  lines that are not commented by a "#" and that are non-empty.
 */

bool
test_read_lines (bool verbose)
{
    std::string file { "tests/data/lines.txt" };
    lib66::tokenization lines;
    bool success = util::file_read_lines(file, lines);
    if (success)
    {
        int count { 0 };
        for (const auto & s : lines)
        {
            ++count;
            if (verbose)
                std::cout << s << std::endl;
        }
        success = count == 11;
    }
    else
    {
        std::cerr << "Could not read lines from '" << file << "'" << std::endl;
    }
    return success;
}

/*
 *  Tests util::get_xdg_runtime_directory().
 */

bool
test_xdg_runtime_directory ()
{
    std::string daemondir
    {
        util::get_xdg_runtime_directory("nsm", "d")
    };
    bool success = ! daemondir.empty();
    return success;
}

/*
 *  Tests util::tokenize_pairs() and util::lookup_token_pairs().
 *
 *  The raw string defined here is a little tricky. First, it starts with a
 *  newline. Second, the indenting spaces are included in the string.
 *  Third, a newline terminates every line.
 *
 *  So first we tokenize the raw string using "\n" as the delimiter.
 *  The first newline is not stored, but the last one is, so we
 *  pop that off for this test.
 *
 *  Also note that we use an "=" for the delimiter in the
 *  "name=value" pairs.
 */

bool
test_tokenize_pairs (bool verbose)
{
    const std::string c_os_release      /* taken from /etc/os-release       */
    {
        R"(
    PRETTY_NAME="Debian GNU/Linux forky/sid"
    NAME="Debian GNU/Linux"
    VERSION_CODENAME=forky
    ID=debian
    HOME_URL="https://www.debian.org/"
    SUPPORT_URL="https://www.debian.org/support"
    BUG_REPORT_URL="https://bugs.debian.org/"
        )"
    };
    lib66::tokenization c_lines
    {
        util::tokenize(c_os_release, "\n")
    };
    int count { 0 };
    c_lines.pop_back();
    if (verbose)
    {
        std::cout << c_os_release << std::endl;
        for (const auto & line : c_lines)
        {
            std::cout << "[" << count << "] " << line << std::endl;
            ++count;
        }
        std::cout << std::endl;
    }
    bool success { c_lines.size() == 7 };
    if (success)
    {
        lib66::tokenpairs tp { util::tokenize_pairs(c_lines, "=") };
        if (verbose)
        {
            count = 0;
            for (const auto & p : tp)
            {
                std::cout << "[" << count << "] "
                    << p.first << " = " << p.second
                    << std::endl
                    ;
                ++count;
            }
            std::cout << std::endl;
        }
        success = tp.size() == 7;
        if (success)
        {
            std::string value { util::lookup_token_pair(tp, "PRETTY_NAME") };
            if (verbose)
                std::cout << "PRETTY_NAME = '" << value << "'" << std::endl;

            success = value == "Debian GNU/Linux forky/sid";
        }
    }
    return success;
}

/*
 *  Tests util::capitalize() and util::uncapitalize()
 */

bool
test_capitalize_uncapitalize (bool verbose)
{
    const std::string downword { "linux" };
    const std::string upword { "Linux" };
    std::string capped { util::capitalize(downword) };
    std::string uncapped { util::uncapitalize(upword) };
    bool success { capped == upword && uncapped == downword };
    if (verbose && success)
    {
        std::cout
            << std::endl
            << downword << " ---> " << capped << " ---> " << uncapped << "\n"
            << std::endl
            ;
    }
    return success;
}

}               // namespace anonymous

/*
 *  main() routine.
 *
 *  Note the usage of the V() macro to return the string pointer
 *  (it's shorter than .c_str(), and not so ugly. See the
 *  msgfunctions and strfunctions header files. Without it,
 *  gcc won't complain, but the formatted data is garbage.
 *  Clang won't accept the code at all.
 */

int
main (int argc, char * argv [])
{
    int rcode { EXIT_FAILURE };
    cli::parser clip;                   /* provides global/stock options    */
    bool canrun { false };
    bool success { clip.parse(argc, argv) };
    if (success)
    {
        if (clip.show_information_only())
        {
            if (clip.help_request())
            {
                std::cout << "No additional help." << std::endl;
            }
            if (clip.description_request())
            {
                std::cout
                    << "Build details: \n\n" << cfg::get_build_details()
                    << "Run-time details: \n\n" << cfg::get_runtime_details()
                    ;
            }
            if (clip.version_request())
            {
                std::cout
                    << "There is still more to do in this app." << std::endl
                    ;
            }
        }
        else
            canrun = true;
    }
    if (canrun)
    {
        success = test_string_formatting();
        if (success)
            success = test_read_lines(clip.verbose());

        if (success)
            success = test_xdg_runtime_directory();

        if (success)
            success = test_tokenize_pairs(clip.verbose());

        if (success)
            success = test_capitalize_uncapitalize(clip.verbose());

        if (success)
        {
            std::cout << "util C++ test succeeded" << std::endl;
            rcode = EXIT_SUCCESS;
        }
        else
            std::cerr << "util C++ test failed" << std::endl;
    }
    return rcode;
}

/*
 * util_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
