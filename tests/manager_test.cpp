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
 *      A test-file for the session::climanager class.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-01-26
 * \updates       2024-10-01
 * \license       See above.
 *
 *  Also includes testing of session::manager.
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/appinfo.hpp"              /* cfg::appinfo functions           */
#include "cfg/inimanager.hpp"           /* cfg::inimanager class            */
#include "session/climanager.hpp"       /* session::climanager class        */
#include "session/layout.hpp"           /* session::layout class            */
#include "util/filefunctions.hpp"       /* util::file_write_string()        */
#include "util/msgfunctions.hpp"        /* util::error_message()            */

/*
 * Sample data.
 */

#include "rc_spec.hpp"                  /* chunk of data for an 'rc' file   */
#include "small_spec.hpp"               /* small for easier debugging       */
#include "session_spec.hpp"             /* for evaluation of this method    */

namespace
{

/**
 *  App Info
 */

cfg::appinfo s_application_info
{
    cfg::appkind::cli,                  // "cli"
    "manager_test",                     // m_app_name (mandatory!)
    "0.0",                              // m_app_version
    "[Mgr66]",                          // m_main_cfg_section_name
    "",                                 // m_home_cfg_directory
    "mgrtest.bogus",                    // m_home_cfg_file
    "mgrtest.123",                      // m_client_name (fake session wart)
    "",                                 // m_app_tag
    "",                                 // m_arg_0
    "CFG66",                            // m_package_name
    "",                                 // m_session_tag
    "mgricon",                          // m_app_icon_name (empty by default)
    "",                                 // m_app_version_text derived
    "nginx",                            // m_api_engine (empty by default)
    "1.0",                              // m_api_version (empty by default)
    "qt 5.5",                           // m_gui_version (bogus here)
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

/**
 *  Consolidate the inisections::specifications into a vector of pointers.
 *  Replaces a few calls to add_inisections(inisections::specification &)
 *  with
 *  add_inisections(inimanager::sections_specs &)
 */

cfg::inimanager::sections_specs s_sections_data
{
    &cfg::session_data,
    &cfg::rc_data,
    &cfg::small_data
};

/**
 *  Test climanager.
 */

/*
 * Explanation text.
 */

const std::string s_help_intro
{
    "This test program illustrates/tests the session::climanager class.\n"
};

/**
 *  Find the global options first, and then display their "debug" text. The
 *  sets of options shown are those added via calls to add_inisections():
 *
 *      -   Global options.
 *      -   'rc' options.
 *      -   'small' options.
 */

bool
list_sections
(
    const cfg::inimanager & ccfg,
    bool use_log_file,
    const std::string & log_file
)
{
    std::string dbgtext = ccfg.debug_text();
    bool result = ! dbgtext.empty();
    if (use_log_file)
    {
        util::file_message("Appending list text to the log file", log_file);
        result = util::file_write_string(log_file, dbgtext);
    }
    else
        std::cout << dbgtext << std::endl;

    return result;
}

/*
 *  Smoke test.  Uses default constructors.
 */

bool
simple_smoke_test ()
{
    // session::climanager appmgr;    // (argc, argv);  TODO TODO
    std::cerr << "session::climanager C++ test not yet ready" << std::endl;
    return true;
}

bool
legacy_main_test (int argc, char * argv [])
{
    cfg::inimanager cfgmgr(s_test_options);    /* add test options */
    bool success = cfgmgr.add_inisections(s_sections_data);
    if (success)
    {
        session::directories dirs{};            /* TEMPORARY */
        session::climanager{dirs, cfgmgr};
        cli::multiparser & clip = cfgmgr.multi_parser();
        success = clip.parse(argc, argv);
        if (clip.use_log_file())
        {
            std::string msg
            {
"This file is the result of writing to this log file from the manager_test\n"
"program. This is a test log file, written to directly...\n"
            };
            std::cout
                << "Using log file '" << clip.log_file() << "'"
                << std::endl
                ;
            success = util::file_write_string(clip.log_file(), msg);
        }
        if (clip.show_information_only())
        {
            success = true;
        }
        else
        {
            bool do_list = cfgmgr.boolean_value("list");
            bool do_read = ! cfgmgr.value("read").empty();
            bool do_write = ! cfgmgr.value("write").empty();
            bool do_list_only = ! do_read && ! do_write;
            if (do_list_only)
            {
                success = list_sections
                (
                    cfgmgr, clip.use_log_file(), clip.log_file()
                );
            }
            if (do_read)
            {
                /*
                 * For a decent test, read tests/data/ini_set_test.rc,
                 * redirect --list to a log file, and verify the results
                 * of parsing.
                 */

                std::string fname{cfgmgr.value("read")};
                success = cfgmgr.read_sections(fname, "rc");
                if (success)
                {
                    if (do_list)            /* --list --read=file   */
                    {
                        success = list_sections
                        (
                            cfgmgr, clip.use_log_file(),
                            clip.log_file()
                        );
                    }
                    if (success && do_write)
                    {
                        success = cfgmgr.write_sections(fname, "rc");

                        /*
                         * if (success)
                         *     success = write_sections(ccfg, "small");
                         */
                    }
                }
            }
            else if (do_write)
            {
                std::string fname{cfgmgr.value("write")};
                success = cfgmgr.write_sections(fname, "rc");
                /*
                if (success)
                    success = write_sections(fname, "small");

                if (success)
                    success = write_sections(fname, "session");
                 */
            }
        }
    }
    return success;
}

#if defined USE_INIMANAGER_PASSALONG_FUNCTIONS

bool
main_test (int argc, char * argv [])
{
    cfg::inimanager cfgmgr(s_test_options);    /* add test options */
    bool success = cfgmgr.add_inisections(s_sections_data);
    if (success)
    {
        session::directories dirs{};            /* TEMPORARY */
        session::climanager{dirs, cfgmgr};
        cli::multiparser & clip = cfgmgr.multi_parser();
        success = clip.parse(argc, argv);
        if (clip.use_log_file())
        {
            std::string msg
            {
"This file is the result of writing to this log file from the manager_test\n"
"program. This is a test log file, written to directly...\n"
            };
            std::cout
                << "Using log file '" << clip.log_file() << "'"
                << std::endl
                ;
            success = util::file_write_string(clip.log_file(), msg);
        }
        if (clip.show_information_only())
        {
            success = true;
        }
        else
        {
            bool do_list = cfgmgr.boolean_value("list");
            bool do_read = ! cfgmgr.value("read").empty();
            bool do_write = ! cfgmgr.value("write").empty();
            bool do_list_only = ! do_read && ! do_write;
            if (do_list_only)
            {
                success = list_sections
                (
                    cfgmgr, clip.use_log_file(), clip.log_file()
                );
            }
            if (do_read)
            {
                /*
                 * For a decent test, read tests/data/ini_set_test.rc,
                 * redirect --list to a log file, and verify the results
                 * of parsing.
                 */

                std::string fname{cfgmgr.value("read")};
                success = cfgmgr.read_sections(fname, "rc");
                if (success)
                {
                    if (do_list)            /* --list --read=file   */
                    {
                        success = list_sections
                        (
                            cfgmgr, clip.use_log_file(),
                            clip.log_file()
                        );
                    }
                    if (success && do_write)
                    {
                        success = cfgmgr.write_sections(fname, "rc");

                        /*
                         * if (success)
                         *     success = write_sections(ccfg, "small");
                         */
                    }
                }
            }
            else if (do_write)
            {
                std::string fname{cfgmgr.value("write")};
                success = cfgmgr.write_sections(fname, "rc");
                /*
                if (success)
                    success = write_sections(fname, "small");

                if (success)
                    success = write_sections(fname, "session");
                 */
            }
        }
    }
    return success;
}


#endif  // defined USE_INIMANAGER_PASSALONG_FUNCTIONS

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
        success = simple_smoke_test();
        std::cout << cfg::get_build_details() << std::endl;
        if (success)
        {
            /*
             * The session::climanager has a cli::multiparser which has
             * a cfg::inimanager.
             *
             * Consider this constructor to avoid making a temporary
             * inimanager:
             *
             *      session::climanager
             *      (
             *          const cfg::options::container & additional,
             *          cfg::inimanager::sections_specs & ops
             *              - or - inisections::specification & op
             *          session::directories & fileentries,
             *          const std::string * caps
             *      );
             *
             * Plus forwarding functions fo add_inisections(),
             * read_sections(), write_sections(), parse(),
             * show_information_only(), [use_]log_file(), and all
             * the value() functions (setters and getters).  Worth it?
             */

            success = legacy_main_test(argc, argv);
            if (success)
            {
                util::status_message("session::climanager C++ test succeeded");
                rcode = EXIT_SUCCESS;
            }
        }
        else
            util::error_message("session::climanager C++ test failed");
    }
    return rcode;
}

/*
 * manager_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

