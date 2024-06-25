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
 * \file          multiparser.cpp
 *
 *      Provides a way to associate command-line options with INI entries.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-24
 * \updates       2024-06-25
 * \license       See above.
 *
 *      The limitations of command-line options as implemented in cli::parser
 *      are space and applicability to a single configuration file and single
 *      option section. One way around this limitation would be to make long
 *      options such as "rc-jack-transport-transport-type=conditional". Ugh.
 *
 *      Another option is to map the single-character code and long name of
 *      the option to the "config-type / sections-name / option-name" trio,
 *      and use those string to navigate to the desired option. In this case,
 *      we have to use a brute-force search to find the option code or name.
 */

#include "c_macros.h"                   /* not_nullptr()                    */
#include "cli/multiparser.hpp"          /* cli::multiparser class           */

namespace cli
{

#if defined EXPOSE_THIS_SAMPLE

/**
 *  We're going to create this map programmatically.
 */

static multiparser::map s_sample_map
{
    {
        { "d", "record-by-channel" },
        { "rc",  "[midi-clock-mod-ticks]", "record-by-channel" },
    },
    {
        { "",  "scale"  ....       },
    }
    {
        { "usr", "[user-interface-settings]", "window-scale"   },
    }
};

#endif  // defined EXPOSE_THIS_SAMPLE

/**
 *  Empty options set.
 */

static cfg::options s_dummy_options;

/**
 *  Constructors. The first creates an empty options container.
 */

multiparser::multiparser () :
    parser          (s_dummy_options),
    m_cli_mappings  ()
{
    // no code
}

bool
multiparser::cli_mappings_add (cfg::inisections::specification & spec)
{
    for (auto sec : spec.file_sections)     /* vector of specref (wrappers) */
    {
    }
    return false;       // TODO
}

}           // namespace cli

/*
 * multiparser.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

