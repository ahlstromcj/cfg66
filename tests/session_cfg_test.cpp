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
 * \file          session_cfg_test.cpp
 *
 *      A test-file for the session::climanager class.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-10-30
 * \updates       2024-10-30
 * \license       See above.
 *
 *  Also includes testing of session::manager.
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/appinfo.hpp"              /* cfg::appinfo functions           */
//#include "cfg/inimanager.hpp"           /* cfg::inimanager class            */
//#include "session/climanager.hpp"       /* session::climanager class        */
#include "session/layout.hpp"           /* session::layout class            */
//#include "util/filefunctions.hpp"       /* util::file_write_string()        */
//#include "util/msgfunctions.hpp"        /* util::error_message()            */

/*
 * Sample data.
 */

//#include "rc_spec.hpp"                  /* chunk of data for an 'rc' file   */
//#include "small_spec.hpp"               /* small for easier debugging       */
//#include "session_spec.hpp"             /* for evaluation of this method    */

namespace
{

/**
 *  App Info
 */

cfg::appinfo s_application_info
{
    cfg::appkind::cli,                  // "cli"
    "session_cfg_test",                 // m_app_name (mandatory!)
    "0.0",                              // m_app_version
    "[Sess66]",                         // m_main_cfg_section_name
    "",                                 // m_home_cfg_directory
    "sestest.bogus",                    // m_home_cfg_file
    "sestest.123",                      // m_client_name (fake session wart)
    "",                                 // m_app_tag
    "",                                 // m_arg_0
    "CFG66",                            // m_package_name
    "",                                 // m_session_tag
    "sesicon",                          // m_app_icon_name (empty by default)
    "",                                 // m_app_version_text derived
    "cfg66",                            // m_api_engine (empty by default)
    "1.0",                              // m_api_version (empty by default)
    "",                                 // m_gui_version (none)
    "",                                 // m_client_name_short
    ""                                  // m_client_name_tag
};

/**
 *  Default session options for initialization. See the session::configuration
 *  and session::layout classes.
 */

session::layout::session_configuration s_session_cfg
{
    {                                   // sc_cfg66_main
        true,                           //      scfg_auto_option_save
        true,                           //      scfg_auto_data_save
        "$home",                        //      sfcg_home
        false,                          //      scfg_quiet
        true                            //      scfg_verbose
    },
    "This session configuration is built from a structure in the manager test.",
    3,                                  // sc_section_count
    {                                   // sc_section_list
        "session",
        "log",
        "rc"
    },
    1,                                  // sc_subdir_count
    {                                   // sc_subdirs
        "data"
    },
    {                                   // sc_entry_list
        {                               //      [session]
            "session",                  //          (redundant)
            true,                       //          "active"
            "cfg",                      //          "directory"
            "cfg66",                    //          "basename"
            ".session"                  //          "ext"
        },
        {
            "log", true, "logs", "", ".log"
        },
        {
            "rc", true, "cfg", "", ".rc"
        }
    }
};

#if 0

/**
 *  Contains additions to the stock command-line options. These are options
 *  that are always present, and not associated with an INI file and INI
 *  section.
 *
 *  These options represent stuff we want to do with this test application.
 */

cfg::options::container s_test_options
{
    /*
     * option_code, option_kind, option_cli_enabled,
     * option_default, option_value, option_read_from_cli, option_modified,
     * option_desc, option_built_in
     */
    {
        "list",
        {
            'l', cfg::options::kind::boolean, cfg::options::enabled,
            "false", "", false, false,
            "List all options and their values.", false
        }
    },
    {
        "read",
        {
            'r', cfg::options::kind::filename, cfg::options::enabled,
            "", "", false, false,
            "Read options from an 'xx' file.", false
        }
    },
    {
        "write",
        {
            'w', cfg::options::kind::filename, cfg::options::enabled,
            "", "", false, false,
            "Write options to an 'xx' file.", false
        }
    }
};

#endif  // 0

/*
 * Explanation text.
 */

const std::string s_help_intro
{
    "This test program illustrates/tests the session::configuration class.\n"
};

bool
read_test ()
{
    return true;
}

}   // namespace (anonymous)

/*
 * main() routine
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    bool success = cfg::initialize_appinfo(s_application_info, argv[0]);
    if (success)
    {
        success = read_test();
        if (success)
        {
            util::status_message("session::configuration C++ test succeeded");
            rcode = EXIT_SUCCESS;
        }
        else
            util::error_message("session::configuration C++ test failed");
    }
    return rcode;
}

/*
 * session_cfg_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

