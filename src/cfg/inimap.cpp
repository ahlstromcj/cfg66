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
 * \updates       2024-06-21
 * \license       See above.
 *
 *  In an application, we want to access options via the triplet of
 *  "configuration type / section name / option name". This triplet
 *  corresponds to "inisections / inisection / option".
 *
 *  Then we want a parser that, unlike cli::parser, can contain multiple
 *  cfg::options objects.
 *
 */

#include "cfg/inimap.hpp"               /* cfg::inimap class                */

namespace cfg
{

/*------------------------------------------------------------------------
 * inimap
 *------------------------------------------------------------------------*/

inimap::inimap () : m_sections_map   ()
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
inimap::add_inisections
(
    const std::string & cfgtype,
    inisections::specification & spec
)
{
    bool result = ! cfgtype.empty();
    if (result)
    {
        inisections sec{cfgtype, spec};
        auto p = std::make_pair(cfgtype, sec);      /* does it make a copy? */
        auto r = m_sections_map.insert(p);          /* another copy         */
        result = r.second;
#if defined PLATFORM_DEBUG
        if (! result)
            printf("Unable to insert sections %s\n", cfgtype.c_str());
#endif
    }
    return result;
}

/**
 *  Look up an inisections in this inimap object using the configuration
 *  type.
 *
 *  Returns a reference to a an inactive inisection if not found.
 *  These functions must not call each other, else... recursion.
 *
 *  And how do we prevent the caller from modifying the first one?
 *  The caller must check with inisection::active().
 */

const inisections &
inimap::find_inisections (const std::string & cfgtype) const
{
    static inisections s_inactive_inisections;
    for (const auto & sections : sections_map())
    {
        if (sections.second.config_type() == cfgtype)
            return sections.second;
    }
    return s_inactive_inisections;
}

/**
 *  In C++17 we can replace "static_cast<const inimap &>(*this)"
 *  with "std::as_const(*this)".
 */

inisections &
inimap::find_inisections (const std::string & cfgtype)
{
    return const_cast<inisections &>
    (
        static_cast<const inimap &>(*this).find_inisections(cfgtype)
    );
}

}           // namespace cfg

/*
 * inimap.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
