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
 * \updates       2024-07-05
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
#include "cfg/inimanager.hpp"           /* cfg::inimanager class, etc.      */

#if ! defined USE_STD_COUT_CERR
#include "util/msgfunctions.hpp"        /* util::error_message()            */
#endif

#include "ctrl_spec.hpp"

#if 0
#include "drums_spec.hpp"
#include "mutes_spec.hpp"
#include "palette_spec.hpp"
#include "playlist_spec.hpp"
#include "rc_spec.hpp"
#include "session_spec.hpp"
#include "usr_spec.hpp"
#endif

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
 *      bool success = configuration_set.parse_cli(argc, argv);
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
    cfg::inimanager configuration_set;
    cfg::set_client_name("iniset");

    bool success = configuration_set.add_inisections
    (
        "ctrl", cfg::ctrl_data                          /* ctrl_spec.hpp    */
    );
    if (success)
    {
        cli::multiparser & clip = configuration_set.multi_parser();
        success = clip.parse(argc, argv);
        if (success)
        {
            if (clip.show_information_only())
            {
                rcode = EXIT_SUCCESS;
            }
            else
            {
                if (success)
                {
                    rcode = EXIT_SUCCESS;
                }

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
    return rcode;
}

/*
 * ini_set_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

