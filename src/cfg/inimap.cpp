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
 * \file          inimap.cpp
 *
 *    Provides a way to more comprehensively automate the reading and
 *    writing of INI-style files.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-19
 * \updates       2024-06-19
 * \license       See above.
 *
 */

#include "cfg/inimap.hpp"               /* cfg::inimap class                */

namespace cfg
{

/*------------------------------------------------------------------------
 * inimap
 *------------------------------------------------------------------------*/

inimap::inimap () : m_option_map   ()
{
    // no code
}

/**
 *  This function looks at an inisections::specification. Each of those
 *  contains a list of inisection::specifications, and each of those contains
 *  a set of options in a cfg::options::container.
 *
 *  We loop through the inisection::specifications in the
 *  inisections::specification, and add each of them to the map.
 *
 *  If this function fails, it means a section name is not unique. All section
 *  names must be unique.
 *
 *  What about [comments] and [Cfg66] though? They are basically read-only and
 *  will never be modified, except via access from each INI file module. They
 *  don't need to be in this map.  We should skip them.
 */

bool
inimap::add_option (const std::string & option_name, options::spec & op)
{
    bool result = ! option_name.empty();
    if (result)
    {
        auto p = std::make_pair(option_name, std::ref(op));
        auto r = m_option_map.insert(p);
        result = r.second;
        if (! result)
            printf("Unable to insert option %s\n", option_name.c_str());
    }
    return result;
}

#if defined CFG66_USE_INIMAP

/*
 *  In the for-loop, 'opt' is an options::option object. The 'auto' keyword
 *  can obscure the code, but listen to Scott Meyers!
 *
 *  option_set() returns a cfg::options object; option_pairs() returns a tuple
 *  of the option name and the option spec structure.
 */

bool
inisection::add_options_to_map (inimap & mapp)
{
    bool result = true;
    for (auto & opt : option_set().option_pairs())
    {
        result = mapp.add_option(opt.first, opt.second);
        if (! result)
            break;
    }
    return result;
}

bool
inisections::add_options_to_map (inimap & mapp)
{
    bool result = true;
    for (auto sect : section_list())
    {
        result = sect.add_options_to_map(mapp);
        if (! result)
            break;
    }
    return result;
}

#endif  // defined CFG66_USE_INIMAP

}           // namespace cfg

/*
 * inimap.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

