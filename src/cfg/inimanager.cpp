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
 * \file          inimanager.cpp
 *
 *    Provides a way to more comprehensively automate the reading and
 *    writing of INI-style files.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-19
 * \updates       2024-09-03
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
 *      inimanager::find_options(cfgtype, sectionname)
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
 *      inimanager::find_options_spec(cfgtype, sectionname, optionname)
 *
 *  Then we want a parser that, unlike cli::parser, can contain multiple
 *  cfg::options objects: cli::multiparser.
 */

#include "cfg/inifile.hpp"              /* cfg::inifile class               */
#include "cfg/inimanager.hpp"           /* cfg::inimanager class            */
#include "util/msgfunctions.hpp"        /* util::error_message(), etc.      */

namespace cfg
{

/*------------------------------------------------------------------------
 * inimanager
 *------------------------------------------------------------------------*/

/**
 *  Creates the "stock" inisections object (and its "stock" inisection and
 *  options objects.
 *
 *  -   inisections()
 *      -   inisection()
 *          -   options() : add(global_options()) and initialize()
 */

inimanager::inimanager () :
    m_multi_parser  (*this),
    m_sections_map  ()
{
    inisections sec;
    auto p = std::make_pair("", sec);               /* does it make a copy? */
    auto r = sections_map().insert(p);              /* another copy         */
    bool ok = r.second;
    if (ok)
    {
        const options & opts = sec.find_options();  /* find stock options   */
        if (opts.active())
        {
            (void) multi_parser().cli_mappings_add(opts.option_pairs());
        }
    }
}

/**
 *  Provides for the addition of other stock/global options before
 *  (later) adding addition inisections.
 */

inimanager::inimanager (const options::container & additional) :
    m_multi_parser  (*this),
    m_sections_map  ()
{
    inisections sec;
    bool ok = sec.add_options(additional);
    if (! ok)
    {
        util::error_message
        (
            "Failed to add additional global options", "inimanager"
        );
    }

    auto p = std::make_pair(global, sec);           /* does it make a copy? */
    auto r = sections_map().insert(p);              /* another copy         */
    ok = r.second;
    if (ok)
    {
        const options & opts = sec.find_options();  /* find global options  */
        if (opts.active())
        {
            (void) multi_parser().cli_mappings_add(opts.option_pairs());
        }
    }
    else
    {
        util::error_message("Failed to add inisection", "inimanager");
    }
}

/**
 *  This function looks at an inisections::specification. Each of those
 *  contains a list of inisection::specifications, and each of those contains
 *  a set of options in a cfg::options::container.
 *
 *  We first make an inisections object and add it to the sections map. Next,
 *  we loop through the inisection::specifications in the
 *  inisections::specification, and add each of them to the multiparser,
 *  which will help in looking up command-line options in every INI file
 *  (inisections) used by the application.
 *
 *  What about [comments] and [Cfg66] though? They are basically read-only and
 *  will never be modified, except via access from each INI file module. They
 *  don't need to be in this map.  We should skip them.
 *
 *  Also note that this function needs to be called for each INI file
 *  (represented by an inisections) needed by the application.
 */

bool
inimanager::add_inisections (inisections::specification & spec)
{
    std::string cfgtype = spec.file_extension;      /* configuration type   */
    bool result = ! cfgtype.empty();
    if (result)
    {
        inisections sec{spec, cfgtype};
        auto p = std::make_pair(cfgtype, sec);      /* does it make a copy? */
        auto r = sections_map().insert(p);          /* another copy         */
        result = r.second;
        if (result)
        {
            result = multi_parser().cli_mappings_add(spec);
        }
        else
            util::error_message("Unable to insert sections", cfgtype);
    }
    return result;
}

bool
inimanager::add_inisections (inimanager::sections_specs & ops)
{
    bool result = true;
    for (auto & secptr : ops)
    {
        result = add_inisections(*secptr);
        if (! result)
            break;
    }
    return result;
}

/*------------------------------------------------------------------------
 * Finding an inisections object
 *------------------------------------------------------------------------*/

/**
 *  Look up an inisections in this inimanager object using the configuration
 *  type.
 *
 *  Returns a reference to a an inactive inisection if not found.
 *  These functions must not call each other, else... recursion.
 *
 *  And how do we prevent the caller from modifying the first one?
 *  The caller must check with inisection::active().
 */

const inisections &
inimanager::find_inisections (const std::string & cfgtype) const
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
 *  In C++17 we can replace "static_cast<const inimanager &>(*this)"
 *  with "std::as_const(*this)".
 */

inisections &
inimanager::find_inisections (const std::string & cfgtype)
{
    return const_cast<inisections &>
    (
        static_cast<const inimanager &>(*this).find_inisections(cfgtype)
    );
}

/*------------------------------------------------------------------------
 * Read/write an inisections object
 *------------------------------------------------------------------------*/

bool
inimanager::read_sections
(
    const std::string & fname,
    const std::string & cfgtype     /* could be found in fname, perhaps */
)
{
    const cfg::inisections & rcs = find_inisections(cfgtype);
    bool result = rcs.active() && ! fname.empty();
    if (result)
    {
        cfg::inifile f_in(rcs, fname, cfgtype);
        result = f_in.parse();
        if (! result)
            util::error_message("Read failed", fname);
    }
    else
    {
        util::error_message("No options to read", fname);
        result = false;
    }
    return result;
}

bool
inimanager::write_sections
(
    const std::string & fname,
    const std::string & cfgtype     /* could be found in fname, perhaps */
)
{
    const cfg::inisections & rcs = find_inisections(cfgtype);
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
        util::error_message("No options to write", fname);
        result = false;
    }
    return result;
}

/*------------------------------------------------------------------------
 * Finding an inisection object
 *------------------------------------------------------------------------*/

const inisection &
inimanager::find_inisection
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
inimanager::find_inisection
(
    const std::string & cfgtype,
    const std::string & sectionname
)
{
    return const_cast<inisection &>
    (
        static_cast<const inimanager &>(*this).find_inisection
        (
            cfgtype, sectionname
        )
    );
}

/*------------------------------------------------------------------------
 * Finding an options object
 *------------------------------------------------------------------------*/

const options &
inimanager::find_options
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
inimanager::find_options
(
    const std::string & cfgtype,
    const std::string & sectionname
)
{
    return const_cast<options &>
    (
        static_cast<const inimanager &>(*this).find_options
        (
            cfgtype, sectionname
        )
    );
}

/*------------------------------------------------------------------------
 * Finding an options::spec object
 *------------------------------------------------------------------------*/

const options::spec &
inimanager::find_options_spec
(
    const std::string & optionname,
    const std::string & cfgtype,
    const std::string & sectionname
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
inimanager::find_options_spec
(
    const std::string & optionname,
    const std::string & cfgtype,
    const std::string & sectionname
)
{
    return const_cast<options::spec &>
    (
        static_cast<const inimanager &>(*this).find_options_spec
        (
            cfgtype, sectionname, optionname
        )
    );
}

/*------------------------------------------------------------------------
 * Text strings for help, etc.
 *------------------------------------------------------------------------*/

std::string
inimanager::cli_help_text () const
{
    std::string result;
    for (const auto & sections : sections_map())
        result += sections.second.cli_help_text();

    return result;
}

std::string
inimanager::help_text () const
{
    std::string result;
    for (const auto & sections : sections_map())
        result += sections.second.help_text();

    return result;
}

std::string
inimanager::debug_text () const
{
    std::string result;
    for (const auto & sections : sections_map())
        result += sections.second.debug_text();

    return result;
}

/*------------------------------------------------------------------------
 * Options accessors
 *------------------------------------------------------------------------*/

std::string
inimanager::value
(
    const std::string & name,
    const std::string & cfgtype,
    const std::string & sectionname
) const
{
    const std::string s_dummy;
    const options & opts = find_options(cfgtype, sectionname);
    if (opts.active())
        return opts.value(name);
    else
        return s_dummy;
}

void
inimanager::value
(
    const std::string & name,
    const std::string & value,
    const std::string & cfgtype,
    const std::string & sectionname
)
{
    options & opts = find_options(cfgtype, sectionname);
    if (opts.active())
        opts.value(name, value);
}

bool
inimanager::boolean_value
(
    const std::string & name,
    const std::string & cfgtype,
    const std::string & sectionname
) const
{
    const options & opts = find_options(cfgtype, sectionname);
    if (opts.active())
        return opts.boolean_value(name);
    else
        return false;
}

void
inimanager::boolean_value
(
    const std::string & name,
    bool value,
    const std::string & cfgtype,
    const std::string & sectionname
)
{
    options & opts = find_options(cfgtype, sectionname);
    if (opts.active())
        opts.boolean_value(name, value);
}

int
inimanager::integer_value
(
    const std::string & name,
    const std::string & cfgtype,
    const std::string & sectionname
) const
{
    const options & opts = find_options(cfgtype, sectionname);
    if (opts.active())
        return opts.integer_value(name);
    else
        return (-1);        /* or INT_MAX? */
}

void
inimanager::integer_value
(
    const std::string & name,
    int value,
    const std::string & cfgtype,
    const std::string & sectionname
)
{
    options & opts = find_options(cfgtype, sectionname);
    if (opts.active())
        opts.integer_value(name, value);
}

float
inimanager::floating_value
(
    const std::string & name,
    const std::string & cfgtype,
    const std::string & sectionname
) const
{
    const options & opts = find_options(cfgtype, sectionname);
    if (opts.active())
        return opts.floating_value(name);
    else
        return (-1.0);      /* or FLOAT_MAX? */
}

void
inimanager::floating_value
(
    const std::string & name,
    float value,
    const std::string & cfgtype,
    const std::string & sectionname
)
{
    options & opts = find_options(cfgtype, sectionname);
    if (opts.active())
        opts.floating_value(name, value);
}

}           // namespace cfg

/*
 * inimanager.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
