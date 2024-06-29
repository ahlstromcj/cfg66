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
 * \file          cliparser_test.cpp
 *
 *      A test-file for the rudimentary CLI parser class and C API.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2024-06-29
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/appinfo.hpp"              /* cfg::appinfo                     */
#include "cli/parser.hpp"               /* cli::parser, etc.                */
#include "util/filefunctions.hpp"       /* util::file_write_string()        */
#include "test_spec.hpp"                /* s_test_options container         */

/*
 * Explanation text.
 */

static const std::string s_help_intro
{
    "This test program illustrates & tests the C++ interface of the CLI\n"
    "parser of the cfg66 library.  The options available are as follows:\n"
};

static const std::string s_desc_intro
{
    "The --description option can be used by an application to display a brief\n"
    "summary of the purpose of an application.\n"
};

/*
 * main() routine
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    cli::parser clip(s_test_options);               /* see test_spec.hpp    */
    bool success = clip.parse(argc, argv);
    cfg::set_app_version("0.2.0");
    if (success)
    {
        bool show_results = true;
        bool findme_active = clip.check_option(argc, argv, "find-me", false);
        if (findme_active)
            std::cout << "--find-me option found." << std::endl;

        rcode = EXIT_SUCCESS;
        if (clip.show_information_only())
        {
            show_results = false;
            if (clip.help_request())
            {
                std::cout
                    << s_help_intro << "\n"
                    << "Other things to try: \n"
                    << "   Use --find-me as the first argument.\n"
                    << "   Use --find-me --dead-code to verify both "
                       "are detected.\n"
                    << std::endl;
            }
            if (clip.description_request())
            {
                std::cout << s_desc_intro << std::endl;
            }
            if (clip.version_request())
            {
                // Anything?
            }
        }
        if (clip.verbose_request())
        {
            std::cout
                << "Verbose operation. Let's also show the option list.\n"
                << clip.help_text()
                ;
        }
        if (clip.inspect_request())
        {
            std::cerr << "--inspect unsupported in this program." << std::endl;
            success = false;
        }
        if (clip.investigate_request())
        {
            std::cerr
                << "--investigate unsupported in this program."
                << std::endl
                ;
            success = false;
        }
        if (success && clip.use_log_file())
        {
            std::string msg
            {
"This file is the result of writing to this log file from the application\n"
"'cliparser_test'. This is a test log file, written to directly...\n"
"no usage of xpc::reroute_stdio() from the Xpc66 library.\n"
            };
            std::cout
                << "Using log file '" << clip.log_file() << "'" << std::endl
                ;
            success = util::file_write_string(clip.log_file(), msg);
        }
        if (success && show_results)
        {
            if (argc > 1)
            {
                if (findme_active)
                {
                    std::cout << "Looking for '--find-me'... ";
                    bool found = clip.check_option
                    (
                        argc, argv, "--find-me", false
                    );
                    if (found)
                    {
                        std::cout << "found." << std::endl;
                        std::cout << "Looking for 'dead-code'" << std::endl;
                        found = clip.check_option(argc, argv, "dead-code");
                        if (found)
                        {
                            std::cout
                                << "found." << std::endl
                                << "find-me tests succeeded." << std::endl
                                ;
                        }
                        else
                        {
                            std::cerr
                                << "not found." << std::endl
                                << "find-me tests failed." << std::endl
                                ;
                        }
                    }
                }
                else
                {
                    std::string dbgtxt = clip.debug_text(cfg::options::stock);
                    std::cout
                        << "Verify that setting(s) were effective. "
                           "Changed options are 'modified'.\n"
                    ;
                    if (clip.use_log_file())
                    {
                        success = util::file_write_string
                        (
                            clip.log_file(), dbgtxt
                        );
                    }
                    else
                        std::cout << dbgtxt << std::endl;
                }
            }
            else
            {
                success = clip.change_value("alertable", "true");
                if (success)
                    success = clip.change_value("username", "C. Ahlstrom");

                if (success)
                    success = clip.change_value("loop-count", "28");

                if (success)
                {
                    success = ! clip.change_value("", "");
                    if (success)
                        success = ! clip.change_value("dummy", "true");
                }
            }
        }
    }
    if (success)
    {
        std::cout << "cli::parser C++ test succeeded." << std::endl;
        if (! clip.help_request())
            std::cout << "Use --help to see all the options." << std::endl;
    }
    else
    {
        std::cout << "cli::parser C++ test failed!" << std::endl;
        if (clip.inspect_request() || clip.investigate_request())
            std::cout << "Deliberately!" << std::endl;
        else
            std::cout << "Use --help to see the options." << std::endl;
    }
    return rcode;
}

/*
 * cliparser_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

