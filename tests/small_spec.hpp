#if ! defined CFG66_CFG_SMALL_SPEC_HPP
#define CFG66_CFG_SMALL_SPEC_HPP

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
 * \file          small_spec.hpp
 *
 *      Provides a small sample file for debugging the rather complex layers
 *      of INI/CLI handling.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-07-06
 * \updates       2024-07-19
 * \license       See above.
 *
 */

#include "cfg/inisections.hpp"          /* cfg::inisections class           */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cfg
{

/*------------------------------------------------------------------------
 * 'small' file
 *------------------------------------------------------------------------*/

/*
 * Note that we use [misc] : last-used-dir instead of a section of that
 * name. Note that we now avoid conflicts with the rc_spec.hpp module.
 */

inisection::specification small_misc_data
{
    "[misc]",
    {
"Miscellaneous options."
    },
    {
        {
            "sets-mode-bak",        /* otherwise conflicts w/rc_spec.hpp    */
            {
                options::code_null, options::kind::string, options::enabled,
                "normal", "", false, false,
                "Mode for handling arming/muting during play-set changes.",
                false
            }
        },
        {
            "port-naming-bak",      /* otherwise conflicts w/rc_spec.hpp    */
            {
                options::code_null, options::kind::string, options::enabled,
                "short", "", false, false,
                "Port amount-of-label showing.", false
            }
        },
    }
};


inisection::specification small_interaction_data
{
    "[interaction-method]",
    {
"Sets mouse usage for drawing/editing patterns."
    },
    {
        {
            "snap-split",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "", false, false,
                "If true, split trigger at snap point.", false
            }
        },
        {
            "double-click-edit",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "If true, allow double-click edit.", false
            }
        }
    }
};

/*------------------------------------------------------------------------
 * All sections of the 'small' configuration
 *------------------------------------------------------------------------*/

const std::string small_extension{"small"};

inisection::specification small_comments = stock_comment_data();

inisection::specification small_cfg66_data
{
    "[Cfg66]",              /* can replace via set_main_cfg_section_name()  */
    {
"One generally needs to replace inifile_cfg66_data, a stock set of data,\n"
"with a more specific structure. We need to specify the config-type and\n"
"the version.\n"
    },
    {
        /*
         * Name, Name, Code, Kind, Enabled, Default, Value,
         * FromCli, Dirty, Description, Built-in
         */
        {
            "config-type",
            {
                options::code_null, options::kind::string, options::disabled,
                "small", "", false, false,
                "Small configuration file.", false
            }
        },
        {
            "version",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "", false, false,
                "Configuration file version.", false
            }
        }
    }
};

inisections::specification small_data
{
    "small",        /* the file extension for any 'small' file.             */
    "tests/data",   /* use value from appinfo's get_home_cfg_directory()    */
    "tests/data",   /* use value derived from appinfo's get_home_cfg_file() */
    "This 'small' file defines some basic features.",
    {
        std::ref(small_cfg66_data),                 // not inifile_cfg66_data)
        std::ref(small_comments),
        std::ref(small_misc_data),
        std::ref(small_interaction_data)
    }
};

}           // namespace cfg

#endif      // CFG66_CFG_SMALL_SPEC_HPP

/*
 * small_spec.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

