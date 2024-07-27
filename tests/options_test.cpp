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
 * \file          options_test.cpp
 *
 *      A test-file for the rudimentary CLI parser class and C API.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-01-12
 * \updates       2023-08-03
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/options.hpp"              /* cfg::options class               */
#include "cli/parser.hpp"               /* cli::parser class                */
#include "test_spec.hpp"                /* s_test_options container         */

/**
 *  A more extensive list of options is tested in the ini_test program.
 */


/*
 * Explanation text.
 */

static const std::string s_help_intro
{
    "This test program illustrates/tests the configuration-file parser of\n"
    "the cfg66 library.  The options available are as follows:\n\n"
};

/*
 * main() routine
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    cli::parser clip(s_test_options, "", "");
    bool success = clip.parse(argc, argv);
    if (success)
    {
        rcode = EXIT_SUCCESS;

        /*
         *  The application can substitute its own code for the common
         *  options, which are always present.
         */

        if (clip.help_request())
        {
            std::cout << s_help_intro << clip.help_text();
        }
        if (clip.version_request())
        {
            std::cout << "Version 0.0.0" << std::endl;  /* TODO! */
        }
        if (clip.verbose_request())
        {
            std::cout
                << "Verbose operation. Let's show the option list.\n"
                << clip.help_text()
                ;
        }
        if (clip.description_request())
        {
            std::cout << "Description:\n" << clip.description_text();
        }
        if (clip.use_log_file())
        {
            std::cout
                << "Using log file '" << clip.log_file() << "'" << std::endl
                ;
        }
        success = clip.change_value("alertable", "true");
        if (success)
        {
            /*
             * There is no user-name option. It's a spelling error for
             * "username"
             */

            success = ! clip.change_value("user-name", "C. Ahlstrom");
            if (success)
            {
                success = clip.change_value("loop-count", "28");
                if (success)
                {
                    success = clip.change_value("flux", "3.14");
                    if (! success)
                        std::cerr << "Parsing float option failed" << std::endl;
                }
            }
            else
                std::cerr << "Parsing non-existent option failed" << std::endl;

            if (success)
            {
                success = ! clip.change_value("", "");
                if (success)
                    success = ! clip.change_value("dummy", "true");
            }
        }
        else
            std::cerr << "Boolean parsing failed" << std::endl;

        /*
         * And now for some more extensive tests. Also see the history_test
         * ini_test programs.
         */

        if (success)
        {
            cfg::options & opts = clip.option_set();
            int mini, maxi;
            int defalti = opts.integer_value_range("loop-count", mini, maxi);
            success = defalti == 0 && mini == 0 && maxi == 99;
            if (success)
            {
                float minf, maxf;
                float defaltf = opts.floating_value_range("flux", minf, maxf);
                success =
                (
                    cfg::approximates(defaltf, 0.1) &&
                    cfg::approximates(minf, 0.0) &&
                    cfg::approximates(maxf, 50.0)
                );
                if (success)
                {
                    // more tests?
                }
                else
                    std::cerr << "Float range check failed" << std::endl;
            }
            else
                std::cerr << "Integer range check failed" << std::endl;
        }
    }
    else
        std::cerr << "Command-line parsing failed" << std::endl;

    if (success)
        std::cout << "cli::parser C++ test succeeded" << std::endl;
    else
        std::cerr << "cli::parser C++ test failed" << std::endl;

    return rcode;
}

/*
 * options_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

