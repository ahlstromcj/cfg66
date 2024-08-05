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
 * \updates       2024-08-05
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/appinfo.hpp"              /* cfg::appinfo                     */
#include "cli/parser.hpp"               /* cli::parser, etc.                */
#include "util/filefunctions.hpp"       /* util::file_write_string()        */
#include "test_spec.hpp"                /* s_test_options container         */

#if ! defined USE_STD_COUT_CERR
#include "util/msgfunctions.hpp"        /* util::error_message()            */
#endif

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
    cfg::set_client_name("cli");                    /* for error_message()  */
    cfg::set_app_version("0.3.0");
    cli::parser clip{s_test_options};               /* see test_spec.hpp    */
    bool success = clip.parse(argc, argv);
    if (success)
    {
        std::string msg = "Option codes: " + clip.code_list();
#if defined USE_STD_COUT_CERR
        std::cout << msg << std::endl;
#else
        util::status_message(msg);                  /* (info needs verbose) */
#endif

        bool show_results = true;
        bool findme_active = clip.check_option(argc, argv, "find-me", false);
#if defined USE_STD_COUT_CERR
        if (findme_active)
            std::cout << "--find-me option found." << std::endl;
#else
        if (findme_active)
            util::status_message("--find-me option found.");
#endif

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
#if defined USE_STD_COUT_CERR
            std::cerr << "--inspect unsupported in this program" << std::endl;
#else
            util::error_message("--inspect unsupported in this program");
#endif
            success = false;
        }
        if (clip.investigate_request())
        {
#if defined USE_STD_COUT_CERR
            std::cerr
                << "--investigate unsupported in this program"
                << std::endl
                ;
#else
            util::error_message("--investigate unsupported in this program");
#endif
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
                        std::cout << "Looking for 'dead-code'...";
                        found = clip.check_option(argc, argv, "dead-code");
                        if (found)
                            std::cout << "found." << std::endl;
                        else
                            std::cout << "not provided." << std::endl;
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
                {
                    success = clip.change_value("username", "C. Ahlstrom");
                    if (success)
                    {
                        std::string name = clip.value("u");
                        success = name == "C. Ahlstrom";
                    }
                }
                if (success)
                    success = clip.change_value("loop-count", "28");

                if (success)
                {
                    success = ! clip.change_value("", "");
                    if (success)
                        success = ! clip.change_value("dummy", "true");
                }
                if (success)
                {
                    std::string dbgtxt = clip.debug_text(cfg::options::stock);
                    std::cout << dbgtxt << std::endl;
                }
            }
        }
        if (success)
        {
#if defined USE_STD_COUT_CERR
            std::cout << "cli::parser C++ test succeeded" << std::endl;
            if (! clip.help_request())
                std::cout << "Use --help to see all the options." << std::endl;
#else
            util::status_message("cli::parser C++ test succeeded");
            if (! clip.help_request())
                util::status_message("Use --help to see all the options.");
#endif
        }
        else
        {
#if defined USE_STD_COUT_CERR
            std::cout << "cli::parser C++ test failed!" << std::endl;
            if (clip.inspect_request() || clip.investigate_request())
                std::cout << "Deliberately!" << std::endl;
            else
                std::cout << "Use --help to see the options." << std::endl;
#else
            util::error_message("cli::parser C++ test failed");
            if (clip.inspect_request() || clip.investigate_request())
                util::status_message("Deliberately!");
            else
                util::status_message("Use --help to see the options.");
#endif
        }
    }
    else
    {
        std::string errmsg = "Setup or parsing error: ";
        errmsg += clip.error_msg();
#if defined USE_STD_COUT_CERR
        std::cerr << errmsg << std::endl;
#else
        util::error_message(errmsg);
#endif
    }
    return rcode;
}

/*
 * cliparser_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

