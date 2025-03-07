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
 * \file          ini_set_test.cpp
 *
 *      A test-file for the INI-related tests, except for inimanager.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-27
 * \updates       2024-08-08
 * \license       See above.
 *
 *  See the ini_test module for information. This module goes beyond that
 *  to handle multiple INI configuration files.
 *
 *  At this date, we are starting simple and then adding support for various
 *  configuration complexities.
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/appinfo.hpp"              /* cfg::appinfo functions           */
#include "cfg/inifile.hpp"              /* cfg::inifile class, etc.         */
#include "cfg/inimanager.hpp"           /* cfg::inimanager class, etc.      */
#include "util/filefunctions.hpp"       /* util::file_write_string()        */

#if ! defined USE_STD_COUT_CERR
#include "util/msgfunctions.hpp"        /* util::error_message()            */
#endif

/*
 * Sample data.
 */

#include "rc_spec.hpp"                  /* chunk of data for an 'rc' file   */
#include "small_spec.hpp"               /* small for easier debugging       */
#include "session_spec.hpp"             /* for evaluation of this method    */

#undef  USE_ALT_TEST                    /* define for quick experiments     */

#if 0
#include "ctrl_spec.hpp"
#include "drums_spec.hpp"
#include "mutes_spec.hpp"
#include "palette_spec.hpp"
#include "playlist_spec.hpp"
#include "usr_spec.hpp"
#endif

/*
 *  Contains additions to the stock command-line options. These are options
 *  that are always present, and not associated with an INI file and INI
 *  section.
 *
 *  These options represent stuff we want to do with this test application.
 */

static cfg::options::container s_test_options
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
        "test",
        {
            't', cfg::options::kind::boolean, cfg::options::enabled,
            "false", "", false, false,
            "If specified, testing!", false
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
 *  Handle the non-global options.
 */

static bool
handle_section_options ()
{
    return false;           // TODO
}

/**
 *  Find the global options first, and then display their "debug" text. The
 *  sets of options shown are those added via calls to add_inisections():
 *
 *      -   Global options.
 *      -   'rc' options.
 *      -   'small' options.
 */

static bool
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

/**
 *  Write the sections provided. The file-name is taken from the command-line,
 *  and we replace the extension with the configuration type (e.g. "rc").
 */

static bool
write_sections (const cfg::inimanager & ccfg, const std::string & cfgtype)
{
    const cfg::inisections & rcs = ccfg.find_inisections(cfgtype);
    std::string fname = ccfg.value("write");
    bool result = rcs.active() && ! fname.empty();
    if (result)
    {
        cfg::inifile f_out(rcs, fname, cfgtype);
        result = f_out.write();
        if (! result)
            util::error_message("Write failed", fname);
    }
    else
    {
        /*
         * Note that util::error_message() could also
         * be used.  In fact, let's use it.
         *
         * std::cerr << "No options to write" << std::endl;
         */

        util::error_message("No options to write", fname);
        result = false;
    }
    return result;
}

/*
 * Process:
 *
 *  The first step is to create a cfg::inimanager; it is a default
 *  constructor. It creates a default cfg::inisections; it is a "stock"
 *  inisections object. It creates a default cfg::inisection; it is
 *  also a "stock" inisection object. It creates a default cfg::options
 *  object; it populates its cfg::options::container with the "stock"
 *  options. It also sets each option value to its default and clears
 *  the "modified" flag.
 *
 *  Thus, the path to a "stock" option goes from inimanager to the
 *  "stock" inisections, "stock" inisection, and its options container.
 *
 *  In addition to the option container, the long form of the options
 *  names are stored in a vector. Then the inisection is copied and
 *  the original inisection goes out-of-scope.
 *
 *  In the cli::multiparser object, each option is analyzed. If the
 *  option has a code, the <code, name> pair is added to the codes map
 *  so that the long form of the option name be looked up by code
 *  character.
 *
 *  The long name of the option is used in multiparser::cli_mappings_add()
 *  to associate the option name with a configuration type (i.e a
 *  file extension) and a configuration section.
 *
 *  TODO: Since the multiparser inherits an empty cfg::options object,
 *        we need to override the cli::parser::parse() function to
 *        to the lookup sequence using the inimanager. Get the long
 *        name from the code if necessary, look up the configuration
 *        type and section from the long name, then get to the correct
 *        inisection, and access its options container.
 *
 *  -----------------
 *
 *  There are two equivalent ways to do the command-line parsing, the
 *  indirect and the direct.
 *
 *      bool success = cfg_set.parse_cli(argc, argv);
 *      bool success = clip.parse(argc, argv);
 *
 *  To use the second form, "clip" must be non-const, as parsing will
 *  generally modify the options stored by its containing class.
 *
 *  Note that creating a cfg::inimanager adds "stock" options,
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    cfg::inimanager cfg_set(s_test_options);    /* add the test options     */
    cfg::set_client_name("iniset");

    bool success = cfg_set.add_inisections(cfg::small_data); /* small_spec  */
    if (success)
        success = cfg_set.add_inisections(cfg::rc_data);

    if (success)
        success = cfg_set.add_inisections(cfg::session_data);

#if defined USE_ALT_TEST                        /* normally undefined       */
    std::string clihelp = cfg_set.cli_help_text();
    std::cout << clihelp << std::endl;
    return EXIT_SUCCESS;
#else
    if (success)
    {
        cli::multiparser & clip = cfg_set.multi_parser();
        success = clip.parse(argc, argv);
        if (success)
        {
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
                rcode = EXIT_SUCCESS;
            }
            else
            {
                bool do_test = cfg_set.boolean_value("test");
                bool do_list = cfg_set.boolean_value("list");
                bool do_read = ! cfg_set.value("read").empty();
                bool do_write = ! cfg_set.value("write").empty();
                bool do_list_only = ! do_read && ! do_write;
                if (do_list_only)
                {
                    success = list_sections
                    (
                        cfg_set, clip.use_log_file(), clip.log_file()
                    );
                }
                if (do_test)
                {
                    std::cout
                        << "--test selected, more to come"
                        << std::endl
                        ;
                }
                if (do_read)
                {
                    /*
                     * For a decent test, read tests/data/ini_set_test.rc,
                     * redirect --list to a log file, and verify the results
                     * of parsing.
                     */

                    std::string fname;
                    const cfg::inimanager & ccfg = cfg_set;
                    const cfg::inisections & rcs = ccfg.find_inisections("rc");
                    if (rcs.active())
                    {
                        fname = cfg_set.value("read");

                        cfg::inifile f_in(rcs, fname, "rc");
                        success = f_in.parse();
                        if (success)
                        {
                            if (do_list)            /* --list --read=file   */
                            {
                                success = list_sections
                                (
                                    cfg_set, clip.use_log_file(),
                                    clip.log_file()
                                );
                            }
                            if (success && do_write)
                            {
                                success = write_sections(ccfg, "rc");

                                /*
                                 * if (success)
                                 *     success = write_sections(ccfg, "small");
                                 */
                            }
                        }
                        else
                            util::error_message("Read failed", fname);
                    }
                    else
                    {
                        util::error_message("No options to parse", fname);
                        success = false;
                    }
                }
                else if (do_write)
                {
                    /*
                     * Compare to the writing done in ini_test.cpp.
                     */

                    const cfg::inimanager & ccfg = cfg_set;
                    success = write_sections(ccfg, "rc");
                    if (success)
                        success = write_sections(ccfg, "small");

                    if (success)
                        success = write_sections(ccfg, "session");
                }
                else if (handle_section_options())
                {
                    // anything usful?
                }
                else
                {
                    /*****
                    if ( ! clip.use_log_file())
                    {
                        std::cerr
                            << "No action specified; see --help"
                            << std::endl
                            ;
                        success = false;
                    }
                     *****/
                }
                rcode = success ? EXIT_SUCCESS : EXIT_FAILURE ;

#if defined USE_STD_COUT_CERR
                if (success)
                    std::cout << "cfg::inimanager C++ test passed" << std::endl;
                else
                    std::cerr << "cfg::inimanager C++ test failed" << std::endl;
#else
                if (success)
                    util::status_message("cfg::inimanager C++ test passed");
                else
                    util::error_message("cfg::inimanager C++ test failed");
#endif
            }
        }
    }
#endif
    return rcode;
}

/*
 * ini_set_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

