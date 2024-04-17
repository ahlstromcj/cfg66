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
 * \file          cliparser_test_c.c
 *
 *      A test-file for the rudimentary CLI parser class and C API.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2023-07-24
 * \license       See above.
 *
 *  To do: add a help-line for each option.
 */

#include <stdio.h>                      /* printf() etc.                    */
#include <stdlib.h>                     /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <string.h>                     /* strcmp()                         */

#include "cli/cliparser_c.h"            /* typedef options_spec             */

/**
 *  Test options. Note that, in the create_option_list() call in the main()
 *  function, help and version options are automatically added. This is a C
 *  version of cli::options::options (name + spec).
 */

static options_spec s_test_options [] =
{
    /*
     *   Name          Code  Kind     Enabled   Default     Value      Dirty
     */
    {
        "alertable",    'a', "boolean", true,   "false",    "false",   false,
        "If specified, the application is alertable."
    },
    {
        "canned-code",  'c', "boolean", true,   "true",     "true",    false,
        "If specified, the application employs canned code."
    },
    {
        "username",     'u', "string",  true,   "Joe Cool", "",        false,
        "Specifies the user name (for permissions)."
    },
    {
        "loop-count",   'k', "integer", true,   "30",       "0",       false,
        "Specifies the number of loops to make."
    }
};

static size_t s_test_options_count =
    sizeof(s_test_options) / sizeof(options_spec);

/*
 * Explanation text.
 */

static const char * s_help_intro =
"This test program illustrates and tests the C interface of the command-line\n"
"parser of the cfg66 library.  The options available are as follows:\n\n"
    ;

/*
 * main() routine
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    bool success = create_option_list
    (
        &s_test_options[0], (int) s_test_options_count  /* a C-style cast   */
    );
    if (success)
    {
        if (parse_option_list(argc, argv))
        {
            rcode = EXIT_SUCCESS;
            if (help_request())
            {
                printf("%s", s_help_intro);
                printf("%s", help_text());
            }
            else if (version_request())
            {
                printf("Version 0.0.0\n");  /* TODO! */
            }
            else
            {
                char value_buffer[80];
                char * vb = &value_buffer[0];
                size_t vsz = sizeof(value_buffer) / sizeof(char);
                printf("Status before:\n%s", debug_text());
                success = change_value("alertable", "true", false); // not CLI
                if (success)
                {
                    success = change_value("canned-code", "false", false);
                }
                if (success)
                    success = change_value("username", "C. Ahlstrom", false);

                if (success)
                    success = change_value("loop-count", "28", false);

                if (success)
                {
                    success = ! change_value(nullptr, nullptr, false);
                    if (success)
                        success = ! change_value("dummy", "true", false);
                }
                printf("Status after:\n%s", debug_text());
                if (success)
                {
                    success = value("alertable", vb, vsz);
                    if (success)
                        success = strcmp(vb, "true") == 0;
                }
                if (success)
                {
                    success = value("username", vb, vsz);
                    if (success)
                        success = strcmp(vb, "C. Ahlstrom") == 0;
                }
                if (success)
                {
                    success = value("loop-count", vb, vsz);
                    if (success)
                        success = strcmp(vb, "28") == 0;
                }
            }
        }
    }
    if (success)
        printf("cliparser C test succeeded\n");
    else
        printf("cliparser C test failed\n");

    return rcode;
}

/*
 * cliparser_test_c.c
 *
 * vim: sw=4 ts=4 wm=4 et ft=c
 */

