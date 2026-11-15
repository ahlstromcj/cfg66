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
 * \file          filecomparison_test.cpp
 *
 *      A test-file for the filecomparison file-tree traversal module.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2026-11-15
 * \updates       2026-11-15
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout, set::cerr             */

#include "cfg/appinfo.hpp"              /* cfg::appinfo functions           */
#include "cli/parser.hpp"               /* cli::parser, etc.                */
#include "util/filecomparison.hpp"      /* util::filecomparison code        */
#include "util/msgfunctions.hpp"        /* util::file_message(), etc.       */
#include "util/strfunctions.hpp"        /* util::glob_to_regex(), etc.      */

namespace                               /* anonymous namespace              */
{

/*
 * Application information.
 */

cfg::appinfo s_application_info
{
    cfg::appkind::test,                 // "test"
    "filecomparison_test",              // _app_name (mandatory!)
    "0.1",                              // _app_version
    "[compare]",                        // _main_cfg_section_name
    "",                                 // _home_cfg_directory
    "compare.bogus",                    // _home_cfg_file
    "compare",                          // _client_name (fake session wart)
    "",                                 // _app_tag
    "",                                 // _arg_0
    "CFG66",                            // _package_name
    "",                                 // _session_tag
    "",                                 // _app_icon_name (empty by default)
    "",                                 // _app_version_text derived
    "",                                 // _api_engine (empty by default)
    "0.4",                              // _api_version (empty by default)
    "",                                 // _gui_version (bogus here)
    "cfg66",                            // _client_name_short
    "tag"                               // _client_name_tag
};

/*
 * Explanation text.
 */

const std::string s_help_intro
{
    "Not all of the above options are fully supported.\n\n"
    "This test program illustrates/tests the util::filecomparison module.\n"
    "To see all of the files operated on, use the --verbose option.\n"
    "For a list of build and run-time details, use the --description\n"
    "command-line option.\n"
};

const std::string s_desc_intro
{
    "This test exercises the util::filecomparison code.\n"
};

/**
 *  These tests use two text files, but treats them like binary files.
 */

bool
binary_comparisons ()
{
    static const std::string s_text_1
    {
        "tests/data/comparison/text-1.text"
    };
    static const std::string s_text_2
    {
        "tests/data/comparison/text-1-identical.text"
    };
    static const std::string s_text_3
    {
        "tests/data/comparison/text-1-one-byte-off.text"
    };
    bool result { util::compare_binary_files(s_text_1, s_text_2) };
    if (result)
    {
        bool match { util::compare_binary_files(s_text_1, s_text_3) };
        result = ! match;
        if (! result)
        {
            std::cerr
                << s_text_1 << " and " << s_text_3
                << " compared as binary-identical!"
                << std::endl
                ;
        }
    }
    else
    {
        std::cerr
            << s_text_1 << " and " << s_text_2
            << " did not compare as binary-identical."
            << std::endl
            ;
    }
    return result;
}

bool
line_comparisons ()
{
    static const std::string s_text_1
    {
        "tests/data/comparison/text-1.text"
    };
    static const std::string s_text_2
    {
        "tests/data/comparison/text-1-identical.text"
    };
    static const std::string s_text_3
    {
        "tests/data/comparison/text-1-one-byte-off.text"
    };
    static const std::string s_text_4
    {
        "tests/data/comparison/text-1-lines.text"
    };
    bool result { util::compare_line_files(s_text_1, s_text_2) };
    if (result)
    {
        bool match { util::compare_line_files(s_text_1, s_text_3) };
        result = ! match;
        if (result)
        {
            match = util::compare_line_files(s_text_1, s_text_4);
            result = ! match;
            if (! result)
            {
                std::cerr
                    << s_text_1 << " and " << s_text_4
                    << " compared as line-identical!"
                    << std::endl
                    ;
            }
        }
        else
        {
            std::cerr
                << s_text_1 << " and " << s_text_3
                << " compared as line-identical!"
                << std::endl
                ;
        }
    }
    else
    {
        std::cerr
            << s_text_1 << " and " << s_text_2
            << " did not compare as line-identical!"
            << std::endl
            ;
    }
    return result;
}

bool
content_comparisons ()
{
    static const std::string s_text_1
    {
        "tests/data/comparison/text-1.text"
    };
    static const std::string s_text_2
    {
        "tests/data/comparison/text-1-lines.text"
    };
    bool result { util::compare_file_content(s_text_1, s_text_2) };
    if (result)
    {
        //
    }
    else
    {
        std::cerr
            << s_text_1 << " and " << s_text_2
            << " did not compare as content-identical."
            << std::endl
            ;
    }
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
            util::status_message("A binary comparison");
            success = binary_comparisons();
            if (success)
            {
                util::status_message("An exact lines comparison");
                success = line_comparisons();
            }
            if (success)
            {
                util::status_message("A content comparison");
                success = content_comparisons();
            }
        }
        if (success)
        {
            std::cout
                << "util::filecomparison C++ tests succeeded"
                << std::endl
                ;
            rcode = EXIT_SUCCESS;
        }
        else
        {
            std::cerr
                << " A util::filecomparison C++ test failed"
                << std::endl
                ;
        }
    }
    return rcode;
}

/*
 * filecomparison_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp nowrap
 */
