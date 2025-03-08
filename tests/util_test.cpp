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
 * \updates       2025-02-22
 * \license       See above.
 *
 *  We generally test only newly-added functions here; others were
 *  tested in other working applications, such as Seq66.
 *
 *  Run this test from the root directory:
 *
 *      $ ./build/tests/util_test
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout, set::cerr             */

#include "util/filefunctions.hpp"       /* util::file_read_lines()          */
#include "util/msgfunctions.hpp"        /* util::string_format(), V()       */
#include "util/strfunctions.hpp"        /* util::string_format(), V()       */

/*
 * Application information.
 */

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
main (int /*argc*/, char * /*argv*/ [])
{
    int rcode = EXIT_FAILURE;
    std::string msg_1{"This is a message about"};
    std::string msg_2{"variadic functions"};
    std::string target{"MSG: This is a message about: variadic functions."};
    std::string output = util::string_format
    (
        "MSG: %s: %s.", V(msg_1), V(msg_2)  /* V() is "vararg" to get POD   */
    );
    bool success = output == target;
    if (success)
    {
        output = util::string_asprintf("MSG: %s: %s.", V(msg_1), V(msg_2));
        success = output == target;
    }
    if (success)
    {
        std::string file{"tests/data/lines.txt"};
        lib66::tokenization lines;
        success = util::file_read_lines(file, lines);
        if (success)
        {
            for (const auto & s : lines)
            {
                std::cout << s << std::endl;
            }
        }
        else
        {
            std::cerr
                << "Could not read lines from '"
                << file << "'"
                << std::endl
                ;
        }
    }
    if (success)
    {
        std::cout << "util C++ test succeeded" << std::endl;
        rcode = EXIT_SUCCESS;
    }
    else
        std::cerr << "util C++ test failed" << std::endl;

    return rcode;
}

/*
 * util_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

