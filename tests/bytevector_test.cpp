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
 * \file          bytevector_test.cpp
 *
 *      A test-file for the rudimentary CLI parser class and C API.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-05-17
 * \updates       2024-05-17
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/appinfo.hpp"              /* cfg::appinfo functions           */
#include "util/bytevector.hpp"          /* util::bytevector big-endian code */

/*
 * Application information.
 */

static cfg::appinfo s_application_info
{
    cfg::appkind::cli,                  // "cli"
    "bytevector_test",                  // _app_name (mandatory!)
    "0.0",                              // _app_version
    "",                                 // _home_cfg_directory
    "bvectest.bogus",                   // _home_cfg_file
    "bvectest",                         // _client_name (fake session wart)
    "",                                 // _app_tag
    "",                                 // _arg_0
    "CFG66",                            // _package_name
    "",                                 // _session_tag
    "",                                 // _app_icon_name (empty by default)
    "",                                 // _app_version_text derived
    "",                                 // _api_engine (empty by default)
    "0.1",                              // _api_version (empty by default)
    "",                                 // _gui_version (bogus here)
    "bvec",                             // _client_name_short
    "tag"                               // _client_name_tag
};

/*
 * Explanation text.
 */

static const std::string s_help_intro
{
    "This test program illustrates/tests the util::bytevector class.\n"
};

/*
 *  String I/O test.
 */

static bool
basic_string_io ()
{
    std::string fname{"tests/data/1Bar.midi"};
    util::bytevector bv0;
    bv0.assign(fname);

    std::string all = bv0.peek_string();
    bool result = all == fname;
    std::cout << "The whole string: '" << all << "'" << std::endl;

    return result;
}

#if defined USE_THIS_TEST

/*
 *  File I/O test.
 */

static bool
big_endian_file_io ()
{
    std::string fname{"tests/data/1Bar.midi"};
    bool result;
    return false;
}

#endif

/*
 * main() routine
 */

int
main (int /* argc */ , char * argv [])
{
    int rcode = EXIT_FAILURE;
    bool success = cfg::initialize_appinfo(s_application_info, argv[0]);
    if (success)
    {
        success = basic_string_io();
        std::cout << cfg::get_build_details() << std::endl;
        if (success)
            std::cout << "util::bytevector C++ test succeeded" << std::endl;
        else
            std::cout << "util::bytevector C++ test failed" << std::endl;
    }
    return rcode;
}

/*
 * bytevector_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

