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
 * \updates       2026-07-16
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/options.hpp"              /* cfg::options class               */
#include "cli/parser.hpp"               /* cli::parser class                */
#include "test_spec.hpp"                /* s_test_options container         */

namespace       // anonymous
{

/**
 *  The options in s_test_options are stored in a simple
 *  cfg::options::container. The option names are:
 *
 *      alertable         boolean
 *      canned-code       boolean
 *      dead-code         boolean
 *      disabled          boolean
 *      ethernet          boolean
 *      fast-code         boolean
 *      find-me           boolean
 *      flux              floating
 *      loop-count        integer
 *      username          string
 *
 *  The global (all apps) or "stock" options are:
 *
 *      description       boolean
 *      help              boolean
 *      inspect           boolean
 *      investigate       boolean
 *      log               string
 *      option            overflow
 *      quiet             boolean
 *      verbose           boolean
 *      version           boolean
 *
 *  A more extensive list of options is tested in the ini_test program.
 *
 *  Note that there are three ways to change a variable on the command-line:
 *
 *      --loop-count 1
 *      --loop-count:1
 *      --loop-count=1
 */

/*
 * Explanation text.
 */

const std::string s_help_intro
{
    "This test program illustrates/tests the configuration-file parser of\n"
    "the cfg66 library.  The options available are as follows:\n\n"
};

bool
ambiguity_tests (cfg::options & optset)                 // clip.option_set()
{
    int count;
    lib66::tokenization matches;
    bool result { optset.find_option("description", count, matches) };
    if (result)
    {
        result = count == 1 && matches.size() == 1;
        if (result)
        {
            result = optset.find_option("dead-code", count, matches);
            if (result)
                result = count == 1 && matches.size() == 1;

            if (result)
            {
                /*
                 * This function should fail, returning a result of
                 * false, which indicates more than one potential
                 * match was found.
                 */

                result = optset.find_option("de", count, matches);
                if (! result)
                {
                    result = count == 2 && matches.size() == 2;
                    if (result)
                    {
                        std::string msg
                        {
                            cfg::find_option_warning("de", matches)
                        };
                        std::cerr << msg << std::endl;
#if 0
                    std::cerr
                        << "Option '--"
                        << "de' is ambiguous. Matching options are "
                        << "'--" << matches[0]
                        << "' and '--" << matches[1] << "'"
                        << std::endl
                        ;
#endif
                    }
                }
            }
        }
    }
    return result;
}

}           // namespace anonymous

/*
 * main() routine
 */

int
main (int argc, char * argv [])
{
    int rcode { EXIT_FAILURE };
    cli::parser clip(s_test_options, "", "");  /* no config file or section */
    bool success { clip.parse(argc, argv) };
    if (success)
    {
        rcode = EXIT_SUCCESS;

        /*
         *  The application can substitute its own code for the common
         *  options, which are always present.
         *
         *  Note that parser::show_information_only() handles (more
         *  completely) help_request(), description_request(), and
         *  version_request(). Here we just want to valid detection
         *  of the options.
         */

        if (clip.description_request())
        {
            std::cout << "Description:\n" << clip.description_text();
        }
        if (clip.help_request())
        {
            std::cout << s_help_intro << clip.help_text();
        }
        if (clip.inspect_request())
        {
            std::cout << "The '--inspect' option was supplied." << std::endl;
        }
        if (clip.investigate_request())
        {
            std::cout
                << "The '--investigate' option was supplied." << std::endl;
        }
        if (clip.use_log_file())
        {
            std::cout
                << "Using log file '" << clip.log_file() << "'" << std::endl
                ;
        }

        /*
         * TODO: test overflow options (--option)
         */

        if (clip.quiet_request())
        {
            std::cout << "The '--quiet' option was supplied." << std::endl;
        }
        if (clip.verbose_request())
        {
            std::cout
                << "Verbose operation. Let's show the option list.\n"
                << clip.help_text()
                ;
        }
        if (clip.verbose_request())
        {
            std::cout
                << "Verbose operation. Let's show the option list.\n"
                << clip.help_text()
                ;
        }
        if (clip.version_request())
        {
            std::cout << "Version 0.0.0" << std::endl;  /* TODO! */
        }

        /*
         * For checking, we directly access the option set using the
         * functions for a specific type, and we also check
         * string values. BEWARE! Spelling counts!  :-D
         */

        cfg::options & optset { clip.option_set() };
        success = clip.change_value("alertable", "true");
        if (success)
        {
            success = optset.boolean_value("alertable") == true;
            if (success)
                success = clip.value("alertable") == "true";
        }
        if (success)
        {
            /*
             * There is no user-name option. It's a spelling error for
             * "username"
             */

            success = ! clip.change_value("user-name", "C. Ahlstrom");
            if (success)
            {
                std::string v { clip.value("loop-count") };
                std::cout << "loop-count = " << v << std::endl;
                success = clip.change_value("loop-count", "28");
                if (success)
                {
                    success = optset.integer_value("loop-count") == 28;
                    if (success)
                    {
                        v = clip.value("loop-count");
                        std::cout << "loop-count = " << v << std::endl;
                        success = v == "28";
                    }

                    if (! success)
                        std::cerr
                            << "Parsing integer option failed" << std::endl;
                }
                if (success)
                {
                    v = clip.value("flux");
                    std::cout << "flux = " << v << std::endl;
                    success = clip.change_value("flux", "3.14");
                    if (success)
                    {
                        /*
                         * Here, the conversion of the string "3.14" yields
                         * a value of 3.1400001. In general, floating
                         * values will be off a bit, so we use the
                         * approximates() function defined in the options
                         * module. The 3rd parameter defaults to 0.001
                         * times the first parameter.
                         */

                        float f { optset.floating_value("flux") };
                        success = cfg::approximates(f, 3.14F);
                        v = clip.value("flux");
                        std::cout << "flux = " << v << std::endl;
                    }
                    if (success)
                        success = clip.value("flux") == "3.14";

                    if (! success)
                        std::cerr
                            << "Parsing floating option failed" << std::endl;
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
            cfg::options & opts { clip.option_set() };
            int mini, maxi;
            int defalti { opts.integer_value_range("loop-count", mini, maxi) };
            success = defalti == 0 && mini == 0 && maxi == 99;
            if (success)
            {
                float minf, maxf;
                float defaltf
                {
                    opts.floating_value_range("flux", minf, maxf)
                };
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

            if (success)
            {
                success = ambiguity_tests(opts);
                if (! success)
                    std::cerr << "The find_option() test failed" << std::endl;
            }
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
