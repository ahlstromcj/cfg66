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
 * \updates       2024-06-28
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


#if 0
#include "ctrl_spec.hpp"
#include "drums_spec.hpp"
#include "mutes_spec.hpp"
#include "palette_spec.hpp"
#include "playlist_spec.hpp"
#include "rc_spec.hpp"
#include "session_spec.hpp"
#include "usr_spec.hpp"
#endif

/*
 * main() routine
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    cfg::inimanager configuration_set;      /* note! adds "stock" options,      */
    const cli::multiparser & clip = configuration_set.multi_parser();
    cfg::set_client_name("iniset");

    bool success = clip.parse(argc, argv);
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

        if (success)
            std::cout << "cfg::inimanager C++ test succeeded" << std::endl;
        else
            std::cerr << "cfg::inimanager C++ test failed" << std::endl;
    }
    return rcode;
}

/*
 * ini_set_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

