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
 * \file          ftswalker_test.cpp
 *
 *      A test-file for the ftswalker file-tree trtaversal module.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2025-03-10
 * \updates       2025-03-25
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout, set::cerr             */

#include "cfg/appinfo.hpp"              /* cfg::appinfo functions           */
#include "cli/parser.hpp"               /* cli::parser, etc.                */
#include "util/ftswalker.hpp"           /* util::ftswalker big-endian code  */
#include "util/filefunctions.hpp"       /* util::file_exists(), etc.        */
#include "util/msgfunctions.hpp"        /* util::file_message(), etc.       */

namespace                               /* anonymous namespace              */
{

/*
 * Application information.
 */

cfg::appinfo s_application_info
{
    cfg::appkind::test,                 // "test"
    "ftswalker_test",                   // _app_name (mandatory!)
    "0.1",                              // _app_version
    "[fts]",                            // _main_cfg_section_name
    "",                                 // _home_cfg_directory
    "fts.bogus",                        // _home_cfg_file
    "fts",                              // _client_name (fake session wart)
    "",                                 // _app_tag
    "",                                 // _arg_0
    "CFG66",                            // _package_name
    "",                                 // _session_tag
    "",                                 // _app_icon_name (empty by default)
    "",                                 // _app_version_text derived
    "",                                 // _api_engine (empty by default)
    "0.4",                              // _api_version (empty by default)
    "",                                 // _gui_version (bogus here)
    "fts66",                            // _client_name_short
    "tag"                               // _client_name_tag
};

/*
 * Explanation text.
 */

const std::string s_help_intro
{
    "Not all of the above options are fully supported.\n\n"
    "This test program illustrates/tests the util::ftswalker class.\n"
    "For a list of build and run-time details, use the --description\n"
    "command-line option.\n"
};

const std::string s_desc_intro
{
    "This test exercises the util::ftswalker file traversal code.\n"
};

bool
fts_get_file_list_test ()
{
    const std::string rootdir { "tests/data/fts" };
    const std::string target { "session.fts" };
    bool result = util::fts_find_file(rootdir, target);
    return result;
}

bool
fts_callback_test ()
{
    const std::string rootdir { "tests/data/fts" };
    util::ftswalker walker(rootdir);
    util::info_message("Default directory traversal....");

    bool result = walker.process_files(util::fts_show_target);
    if (result)
    {
        util::info_message("Compare-files-before-directories traversal....");
        result = walker.process_files
        (
            util::fts_show_target, "", util::compare_files_before_dirs
        );
    }
    return result;
}

/**
 *  A test of copying a directory hierarchy, which we need to recreate the test
 *  data inside the build directory.  There are two stratagies for directory
 *  traversal. Here is the ordering of each; for brevity, D == directory,
 *  F = File, and DP = Postorder:
 *
 * Copying tests/data/fts to DEST (a path) to result in DEST/fts/....:
 *
 * [fts] Default directory traversal....
 * D:  tests/data/fts                             Make DEST/fts directory
 * D:  tests/data/fts/session_2                   Make DEST/fts/session_2 directory
 * F:  tests/data/fts/session_2/session.fts       Copy session.fts to DEST/fts/session_2
 * DP: tests/data/fts/session_2                   ------
 * D:  tests/data/fts/session_3                   Make DEST/fts/session_3 directory
 * D:  tests/data/fts/session_3/set_1             Make DEST/fts/session_3/set_1 directory
 * F:  tests/data/fts/session_3/set_1/session.fts Copy session.fts to DEST/fts/session_3/set_1
 * DP: tests/data/fts/session_3/set_1             ------
 * D:  tests/data/fts/session_3/set_2             Make DEST/fts/session_3/set_2 directory
 * F:  tests/data/fts/session_3/set_2/session.fts Copy session.fts to DEST/fts/session_3/set_2
 * DP: tests/data/fts/session_3/set_2             ------
 * DP: tests/data/fts/session_3                   ------
 * F:  tests/data/fts/session.fts                 Copy session.fts to DEST/fts
 * DP: tests/data/fts                             ------
 *
 * [fts] Compare-files-before-directories traversal....
 * D:  tests/data/fts                             Make DEST/fts directory
 * F:  tests/data/fts/session.fts                 Copy session.fts to DEST/fts
 * D:  tests/data/fts/session_2                   Make DEST/fts/session_2 directory
 * F:  tests/data/fts/session_2/session.fts       Copy session.fts to DEST/fts/session_2
 * DP: tests/data/fts/session_2                   ------
 * D:  tests/data/fts/session_3                   Make DEST/fts/session_3 directory
 * D:  tests/data/fts/session_3/set_1             Make DEST/fts/session_3/set_1 directory
 * F:  tests/data/fts/session_3/set_1/session.fts Copy session.fts to DEST/fts/session_3/set_1
 * DP: tests/data/fts/session_3/set_1             ------
 * D:  tests/data/fts/session_3/set_2             Make DEST/fts/session_3/set_2 directory
 * F:  tests/data/fts/session_3/set_2/session.fts Copy session.fts to DEST/fts/session_3/set_2
 * DP: tests/data/fts/session_3/set_2             ------
 * DP: tests/data/fts/session_3                   ------
 * DP: tests/data/fts                             ------
 *
 * Using the second method looks to be a tad more straight-forward.
 *
 * Note that this code is essentially the same as the free function
 * fts_copy_directory() function in the ftswalker module.
 */

bool
fts_copy_test ()
{
    const std::string rootdir { "tests/data/fts" };
    const std::string destdir { "build/tests" };   /* -> "build/tests/fts/..." */
    util::ftswalker walker(rootdir);
    bool result = walker.process_files
    (
        util::fts_item_copy, destdir, util::compare_files_before_dirs
    );
    if (result)
        result = util::file_is_directory("build/tests/fts");

    return result;
}

/**
 *  A test of deleting the files and directories created in fts_copy_test().
 */

bool
fts_delete_test ()
{
    const std::string rootdir{ "build/tests/fts" };
    const std::string matcher{ };               /* remove all directories   */
    util::ftswalker walker(rootdir);
    bool result = walker.process_files
    (
        util::fts_item_delete, "", util::compare_files_before_dirs
    );
    if (result)
        result = ! util::file_exists("build/tests/fts");

    return result;
}

}       // namespace anonymous

/*
 *  main() routine. Rather than call cfg::set_client_name(),
 *  cfg::set_app_version(), etc., we use a structure to set the application
 *  information.
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    cli::parser clip;                   /* provides global/stock options    */
    bool success = clip.parse(argc, argv);
    if (success)
        success = cfg::initialize_appinfo(s_application_info, argv[0]);

    if (success)
    {
        if (clip.show_information_only())
        {
            if (clip.help_request())
            {
                std::cout << s_help_intro << std::endl;
            }
            if (clip.description_request())
            {
                std::cout
                    << s_desc_intro << "\n"
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
        {
            /*
             * Runs one or all of the test of the helpers module.
             */

            success = fts_get_file_list_test();
            if (success)
            {
                /*
                 * Basic test of the ftswalker callback mechanism.
                 */

                success = fts_callback_test();
            }
            if (success)
                success = fts_copy_test();

            if (success)
                success = fts_delete_test();

            if (success)
            {
                /*
                 * This test works only with nsmd running.
                 */

                const std::string rootdir{"/run/user/1000/nsm"};
                util::ftswalker walker(rootdir);
                lib66::tokenization results;
                (void) walker.find_regular_files(results);
            }
        }
        if (success)
        {
            std::cout << "util::ftswalker C++ test succeeded" << std::endl;
            rcode = EXIT_SUCCESS;
        }
        else
            std::cerr << "util::ftswalker C++ test failed" << std::endl;
    }
    return rcode;
}

/*
 * ftswalker_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp nowrap
 */

