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
 * \file          manager_test.cpp
 *
 *      A test-file for the session::manager class.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-01-26
 * \updates       2024-07-08
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/appinfo.hpp"              /* cfg::appinfo functions           */
#include "session/manager.hpp"          /* session::manager class           */

/**
 *  App Info
 */

static cfg::appinfo s_application_info
{
    cfg::appkind::cli,                  // "cli"
    "manager_test",                     // _app_name (mandatory!)
    "0.0",                              // _app_version
    "[Mgr66]",                          // _main_cfg_section_name
    "",                                 // _home_cfg_directory
    "mgrtest.bogus",                    // _home_cfg_file
    "mgrtest.123",                      // _client_name (fake session wart)
    "",                                 // _app_tag
    "",                                 // _arg_0
    "CFG66",                            // _package_name
    "",                                 // _session_tag
    "mgricon",                          // _app_icon_name (empty by default)
    "",                                 // _app_version_text derived
    "nginx",                            // _api_engine (empty by default)
    "1.0",                              // _api_version (empty by default)
    "qt 5.5",                           // _gui_version (bogus here)
    "",                                 // _client_name_short
    ""                                  // _client_name_tag
};

/**
 *  Test manager.
 */

/*
 * Explanation text.
 */

static const std::string s_help_intro
{
    "This test program illustrates/tests the session::manager class.\n"
};

/*
 *  Smoke test.  Uses default constructors.
 */

static bool
simple_smoke_test ()
{
    // session::manager appmgr;    // (argc, argv);  TODO TODO
    std::cerr << "session::manager C++ test not yet ready" << std::endl;
    return false;
}

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
        success = simple_smoke_test();
        std::cout << cfg::get_build_details() << std::endl;
        if (success)
            std::cout << "session::manager C++ test succeeded" << std::endl;
        else
            std::cout << "session::manager C++ test failed" << std::endl;
    }
    return rcode;
}

/*
 * manager_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

