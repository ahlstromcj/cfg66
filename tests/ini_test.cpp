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
 * \file          ini_test.cpp
 *
 *      A test-file for the rudimentary CLI parser class and C API.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-07-25
 * \updates       2024-06-15
 * \license       See above.
 *
 *  Rationale:
 *
 *      The location of configuration starts out determined by:
 *
 *      -   Conventional defaults for the operating system
 *          -   "~/.config/"
 *          -   "~/AppData/Local/"
 *          -   plus the application's directory (e.g. "seq66v2").
 *          -   There is no real "data" directory, but one (e.g. "midi")
 *              can be specified.
 *      -   If a session manager like Non/New Session Manager is running
 *          the application, it sets the configuration. For example,
 *          "~/NSM Sessions/fixes/seq66.nXJNO". Seq66 adds the "config"
 *          directory and the data directory called "midi".
 *      -   At that location, the 'session' file is located with the
 *          default name. It can determine the locations of all other
 *          configuration files.
 *      -   The command line can change:
 *          -   "home". Changes the directories noted above.
 *          -   "config". The base name of the 'session' file.
 *          -   This alternate file can change all the other configuration
 *              files as is part of the 'session' functionality.
 *          -   "data". This sub-directory holds data files or files
 *              acted on.  "midi".
 *
 *  Things tested:
 *
 *      1.  Creating a simple CLI parser from a cfg::options object made from
 *          a cfg::options::container.
 *      2.  Adding some options from a cfg::inisection to this list.
 *      3.  Parsing the whole set.
 *      4.  Dumping the cfg::inisection options. The dumping of simpler
 *          is done in the options_test program.
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/inifile.hpp"              /* cfg::inifile class, etc.     */
#include "cfg/inisections.hpp"          /* cfg::inisections class, etc.     */
#include "cfg/options.hpp"              /* cfg::options class               */
#include "cli/parser.hpp"               /* cli::parser class                */

/*
 * inisections::specification objects
 *
 *  The following string keeps track of the characters used so far.
 *
 *  '!' means that the character is "reserved" and should not be used in the
 *      user's programs.
 *  'x' means the character is used in the program.
 *  ':' means it is used in the program and requires an argument.
 *  'a' indicates we could repurpose the key with minimal impact.
 *  '*' indicates the option is "reserved" for overflow options (-o and
 *      --option); those options have no character-code.
 *
\verbatim
        0123456789#@AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz
                         ::   ::  :!!!:xx x!     *   x       x!!  :
\endverbatim
 *
 * Codes used:
 *
 * 'rc':
 *
 *     -c   --config = 'session' file-specification (a 'session' setting)
 *     -f   --rc = base-name of an 'rc' file
 *     -F   --usr = base-name of a 'usr' file
 *     -D   --data = directory-specification (overrides a 'session' setting)
 *     -H   --home = directory-specification (overrides a 'session' setting)
 *     -h   --help
 *     -I   --inspect [WHAT DOES THIS DO?]
 *     -i   --investigate
 *     -J   --transport-type = none, slave, master, conditional
 *     -j   --jack = boolean
 *     -K   --inverse = boolean
 *     -L   --load-most-recent = boolean
 *     -o   --options = overflow options
 *     -q   --ppqn = PPQN value
 *     -u   --usr-save = boolean
 *     -V   --verbose
 *     -v   --version
 *     -X   --playlist = base-name of a 'playlist' file
 *
 */

#include "ctrl_spec.hpp"
#include "drums_spec.hpp"
#include "mutes_spec.hpp"
#include "palette_spec.hpp"
#include "playlist_spec.hpp"
#include "rc_spec.hpp"
#include "session_spec.hpp"
#include "usr_spec.hpp"

/*------------------------------------------------------------------------
 * Minimal option list
 *------------------------------------------------------------------------*/

/**
 *  Test command-line options. Very simple, but note that options like
 *  "--help" are added "under the covers" by cfg::parser.
 */

static cfg::options::container s_test_options
{
    {
        "test",
        {
            'a', "boolean", cfg::options::enabled,
            "false", "false", false, false,
            "If specified, testing!", false
        }
    }
};

/**
 *  Below we set up two INI "files", on with an 'exp' extension and one
 *  with an 'expex' extension. The cfg::inisections::specification for
 *  each contains two cfg::inisection::specification structures.
 *
 *  We want to be able to construct an inifiles describing them, then
 *  write them to their files.
 *
 *  Once we have those done, then we want to be able to test reading them
 *  back.
 *
 *  Lastly, we want to start with no data structures, and reconstruct them
 *  from the two files.
 */

/*------------------------------------------------------------------------
 * Fake 'exp' file options
 *------------------------------------------------------------------------*/

/**
 *  Test options. Note that the newlines are not necessary (they
 *  can be replaced by spaces). For descriptions we now treat
 *  the newlines (and other white space) as space characters.
 */

static cfg::inisection::specification s_simple_ini_spec
{
    "[experiments]",
    {
        "This section is just a bunch of dummies for some simple tests.\n"
        "By itself, it shows the output string for a list of described\n"
        "options. This description string is deliberately long to show\n"
        "how word-wrapping and commenting with '#' can work. Also note\n"
        "that white space such as newlines are converted to spaces for\n"
        "this purpose. Further note that this output can be dumped to\n"
        "a configuration file to save the settings. Lastly, note that,\n"
        "by default, options are output ordered alphabetically by their\n"
        "long option name."
    },
    {
        {
            "boolean-value",
            {
                'b', "boolean", cfg::options::enabled,
                "false", "", false, false,
                "Boolean test option. Booleans must have either the value "
                    "'true' or 'false'. (We made this description long "
                    "deliberately in order to fully test the hanging-indent "
                    "algorithm in the strfunctions module.)",
                false
            }
        },
        {
            "filename-value",
            {
                'f', "filename", cfg::options::enabled,
                "~/.config/seq66v2/seq66v2.session", "", false, false,
                "This is a filename test option with a bogus dummy value. "
                    "The circumflex represent the user's HOME + configuration "
                    "directory.",
                false
            }
        },
        {
            "floating-value",
            {
                'd', "floating", cfg::options::enabled,
                "0.0", "3.141592653", false, false,
                "Floating-point (or double) test option.", false
            }
        },
        {
            "integer-value",
            {
                'i', "integer", cfg::options::enabled,
                "0", "549", false, false,
                "Integer test option.", false
            }
        },
        {
            "count",
            {
                'L', "list", cfg::options::enabled,
                "0", "", false, false,
                "A list must have the name 'count'; caller provides the list.",
                false
            }
        },
        {
            "overflow-option",      /* key is different from option name    */
            {
                cfg::options::code_null,
                "overflow", cfg::options::enabled,
                "null", "This is a bogus overflow option ", false, false,
                "This is an 'overflow' option; still figuring how to process "
                    "and display these options. "
                    "An overflow option is one supported via the --option "
                    "option. This option allows for additional options when "
                    "one runs out of single-character codes to use.",
                false
            }
        },
        {
            "string-value",
            {
                cfg::options::code_null,
                "string", cfg::options::enabled,
                "I am a string.", "", false, false,
                "String test option. Here, there is no single-character "
                "representation for this option.", false
            }
        }
    }
};

static cfg::inisection::specification s_section_spec
{
    "[section-test]",
    {
        "This \"section\" option shows how only one bald string is\n"
        "contained in a section option.\n"
    },
    {
        {
            "value",
            {
                cfg::options::code_null,
                "section", cfg::options::disabled,      /* non-CLI option   */
                "This is actually a multi-line '[section]' value.\n"
                "This is the second line of the [section] value.\n"
                "Normally, this is the ONLY value in an [section] option."
                ,
                ""
                ,
                false, false,
                "Section option, still figuring how to process.", false
            }
        },
    }
};

static cfg::inisections::specification exp_file_data
{
    "exp", "~/.config/experiment/", "exp.session",
    "This is a test INI file specification.",
    {
        s_simple_ini_spec,
        s_section_spec
    }
};

/*------------------------------------------------------------------------
 * Fake 'expex' file options
 *------------------------------------------------------------------------*/

static cfg::inisection::specification s_expex_ini_spec
{
    "[extended-experiments]",
    {
        "This section is another a bunch of dummies for some simple tests.\n"
    },
    {
        {
            "ex-value",
            {
                'x', "integer", cfg::options::enabled,
                "0", "", false, false,
                "Extended integer test option.", false
            }
        },
        {
            "test-value",
            {
                't', "integer", cfg::options::enabled,
                "999", "", false, false,
                "Another extended integer test option.", false
            }
        }
    }
};

static cfg::inisection::specification s_dummy_ini_spec
{
    "[dummy-experiments]",
    {
        "Another section of dummies for some simple tests.\n"
    },
    {
        {
            "ex-dummy",
            {
                cfg::options::code_null, "integer", cfg::options::enabled,
                "0", "", false, false,
                "Extended integer test option.", false
            }
        },
        {
            "test-dummy",
            {
                cfg::options::code_null, "integer", cfg::options::enabled,
                "999", "", false, false,
                "Another extended integer test option.", false
            }
        }
    }
};

static cfg::inisections::specification expex_file_data
{
    "expex", "~/.config/experiment/", "expex.session",
    "This is another test INI file specification.",
    {
        s_expex_ini_spec,
        s_dummy_ini_spec
    }
};

/*
 * Explanation text.
 */

static const std::string s_help_intro
{
    "This test program illustrates/tests the handling of INI-style data in\n"
    "the cfg66 library.\n\n"
};

/*
 * main() routine
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    cfg::options optionset(s_test_options);
    cli::parser clip(optionset);
    bool success = clip.parse(argc, argv);      /* NO OPTIONS YET */
    if (success)
    {
        rcode = EXIT_SUCCESS;

        /*
         *  The application can substitute its own code for the common
         *  options, which are always present.
         */

        if (clip.help_request())
        {
            std::cout
                << "HELP TEXT:\n\n"
                << s_help_intro << clip.help_text()
                ;
        }
        if (clip.version_request())
        {
            std::cout
                << "VERSION TEXT: (TODO)\n\n"
                << "Version 0.0.0" << std::endl;  /* TODO! */
        }
        if (clip.verbose_request())
        {
            std::cout
                << "VERBOSE operation. Let's show the option list.\n"
                << clip.help_text()
                ;
        }
        if (clip.description_request())
        {
            std::cout
                << "DESCRIPTION TEXT:\n\n"
                << "Description:\n" << clip.description_text()
                ;
        }
        if (clip.use_log_file())
        {
            std::cout
                << "Using log file '" << clip.log_file() << "'" << std::endl
                ;

        }

#if defined USE_SIMPLE_TEST
        cfg::inisection experiment{s_simple_ini_spec};
        success = ! experiment.option_set().empty();
        if (success)
        {
            std::cout
                << "EXPERIMENT SETTINGS:\n\n"
                << experiment.settings_text() << std::endl
                ;

            /*
             * Now add experiment options to the parser.
             */

            clip.add(experiment.option_set());
        }
#else
        cfg::inisections ini_experiment("experiment", exp_file_data);
        std::string settings = ini_experiment.settings_text();
        std::cout
            << "INIFILE SETTINGS:\n\n"
            << settings
            ;
#endif
        if (success)
        {
            if (clip.help_request())
            {
                std::cout
                    << "Command-line --help Text:\n\n"
                    << clip.help_text()
                    ;
            }
            else
            {
                cfg::inisections sections("foo", exp_file_data);
                cfg::inifile f(sections, "foo", "session");
                success = f.write();
            }
        }
    }
    else
        std::cerr << "An option was not found" << std::endl;

    if (success)
        std::cout << "cfg::inisection C++ test succeeded" << std::endl;
    else
        std::cerr << "cfg::inisection C++ test failed" << std::endl;

    return rcode;
}

/*
 * ini_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

