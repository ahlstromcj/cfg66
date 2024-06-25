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
 * \updates       2024-06-22
 * \license       See above.
 *
 *  In an application, we want to access options via the triplet of
 *  "configuration type / section name / option name". This triplet
 *  corresponds to "inisections / inisection / option".
 *
 *  Here is a use case where we get an options object so that we
 *  can call an options member function such as setting_line(optionname):
 *
 *      -#  Find the inisections object based on its configuration type.
 *      -#  Find the options object based on its section name.
 *      -#  Call optionsobject.setting_line(optionname).
 *
 *  We consolidate the first two steps into a call to:
 *
 *      inimap::find_options(cfgtype, sectionname)
 *
 *  Here is a use case where we get the complete specification of an
 *  option:
 *
 *      -#  Find the inisection object based on its configuration type and
 *          section name.
 *      -#  Use inisection::find_option_spec(optionname) to get the
 *          specification.
 *      -#  Do something with one of the members of the options::spec
 *          structure.
 *
 *  We consolidate the first two steps into a call to:
 *
 *      inimap::find_options_spec(cfgtype, sectionname, optionname)
 *
 * TODO:
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

/**
 *  Note that there are both const and non-const accessors for these members.
 */

inimap::inimap () :
    m_multi_parser  (),
    m_sections_map  ()
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
        auto r = sections_map().insert(p);          /* another copy         */
        result = r.second;
        if (result)
        {
            result = multi_parser().cli_mappings_add(spec);
        }
#if defined PLATFORM_DEBUG
        else
            printf("Unable to insert sections %s\n", cfgtype.c_str());
#endif
    }
    return result;
}

/*------------------------------------------------------------------------
 * Finding an inisections object
 *------------------------------------------------------------------------*/

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
 *  The casting done below follows the advice of Scott Meyers in
 *  "Effective C++", 3rd edition, in section "Avoiding Duplication in
 *  const and Non-const Member Functions.", pp. 23-26.
 *
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

/*------------------------------------------------------------------------
 * Finding an inisection object
 *------------------------------------------------------------------------*/

const inisection &
inimap::find_inisection
(
    const std::string & cfgtype,
    const std::string & sectionname
) const
{
    static inisection s_inactive_inisection;
    const inisections & sects = find_inisections(cfgtype);
    if (sects.active())
        return sects.find_inisection(sectionname);
    else
        return s_inactive_inisection;
}

inisection &
inimap::find_inisection
(
    const std::string & cfgtype,
    const std::string & sectionname
)
{
    return const_cast<inisection &>
    (
        static_cast<const inimap &>(*this).find_inisection
        (
            cfgtype, sectionname
        )
    );
}

/*------------------------------------------------------------------------
 * Finding an options object
 *------------------------------------------------------------------------*/

const options &
inimap::find_options
(
    const std::string & cfgtype,
    const std::string & sectionname
) const
{
    static options s_inactive_options;
    const inisections & sects = find_inisections(cfgtype);
    if (sects.active())
        return sects.find_options(sectionname);
    else
        return s_inactive_options;
}

options &
inimap::find_options
(
    const std::string & cfgtype,
    const std::string & sectionname
)
{
    return const_cast<options &>
    (
        static_cast<const inimap &>(*this).find_options(cfgtype, sectionname)
    );
}

/*------------------------------------------------------------------------
 * Finding an options::spec object
 *------------------------------------------------------------------------*/

const options::spec &
inimap::find_options_spec
(
    const std::string & cfgtype,
    const std::string & sectionname,
    const std::string & optionname
) const
{
    static options::spec s_inactive_spec;
    const inisection & sect = find_inisection(cfgtype, sectionname);
    if (sect.active())
        return sect.find_option_spec(optionname);
    else
        return s_inactive_spec;
}

options::spec &
inimap::find_options_spec
(
    const std::string & cfgtype,
    const std::string & sectionname,
    const std::string & optionname
)
{
    return const_cast<options::spec &>
    (
        static_cast<const inimap &>(*this).find_options_spec
        (
            cfgtype, sectionname, optionname
        )
    );
}

}           // namespace cfg

/*
 * inimap.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
