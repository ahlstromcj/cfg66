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
 * \file          inisections.cpp
 *
 *    Provides a way to more comprehensively automate the reading and
 *    writing of INI-style files.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2024-06-20
 * \license       See above.
 *
 * Operations to support:
 *
 *      -   Creating the necessary objects. The source data is a list of
 *          options objects: { filename, sectionname, vector of options::spec }
 *          objects. A vector is used because it already supports an
 *          std::initializer list.  We use a free-function as a factory
 *          for this list currently, so we don't have to implement an
 *          initializer list.
 *          -   The optionsmap contains all options, each with the source file
 *              name and a pointer to the options object holding it. This
 *              map contains are all the option-names the application
 *              expects to find. The pointer gives access to the filename,
 *              section name, and option data through the options object.
 *          -   The inisections object holds the actual options objects,
 *              keyed by section name.
 *          -   The inifiles object holds a map of the inisections, keyed by
 *              file name.
 *      -   When reading a configuration file in a multi-config-file app:
 *          -   Get the file-extension or file-name, such as "rc" (or
 *              "xxxx.rc") from the inifiles map.
 *          -   From the entry found, get the section-name (or iterate through
 *              the list of section names). The found entry is the
 *              options object for that section.
 *          -   In the options object, look up the configuration property
 *              name.
 *          -   Modify the property in place.
 *      -   To look up a value, provide only the option name (given that
 *          we insure that name is unique between configuration files).
 *          -   Look it up in the optionsmap, which contains *all*
 *              settings and a pointer to the appropriate options object.
 *          -   Look up the option name in the options object, and modify
 *              it.
 *
 * Current process:
 *
 *      -   Initializer lists (e.g. rc_spec.hpp):
 *          -   On or more inisection::specifications for the section
 *              description and a relatively small set of options.
 *          -   A copy of inisection::specification for comments.
 *          -   An inisections::specification to provide location and description
 *              plus a list of inisection::specifications for the file.
 *              These are std::reference_wrappers for the inisection data.
 *      -   Creations of inisections from the inisection::specifications.
 */

#include "cfg/inisections.hpp"          /* cfg::inisections classes         */
#include "util/filefunctions.hpp"       /* util::filename_split() etc.      */

namespace cfg
{

/*------------------------------------------------------------------------
 * inisections
 *------------------------------------------------------------------------*/

/*
 * Internal
 */

static std::string s_stock_file_intro
{
    "This file holds configuration data for Cfg66-compliant applications.\n"
    "It follows a format similar to the INI files of MS-DOS."
};

inisections::inisections (const std::string & ininame) :
    m_app_version   ("Cfg66-based configuration file"),
    m_directory     (),
    m_name          (),
    m_extension     (),
    m_config_type   (),
    m_description   ("This is a generic configuration file."),
    m_section_list  ()
{
    if (! ininame.empty())
    {
        (void) util::filename_split(ininame, m_directory, m_name);
        m_extension = util::file_extension(m_name);
        if (m_extension[0] == '.')
            m_config_type = m_extension.substr(1);
        else
            m_config_type = m_extension;
    }
}

/**
 *  Create an INI file object and populate it with [sections] filled with
 *  "options".
 *
 * \param ininame
 *      The full path specification to the INI file, such as
 *      "~/.config/seq66v2/special-session/seq66v2.session".
 *
 * \param spec
 *      This structure contains:
 *
 *          -   The type of INI file, such as 'session' or 'rc'. It is
 *              essentially a file extension.
 *          -   The home directory, which might be prepended to a bare
 *              file-name.
 *          -   The name of the home configuration file.
 *          -   A description that goes near the top of the INI file when
 *              it is written to.
 *          -   A list of inisection::specification objects. This object
 *              contains a section name and description plus a container
 *              of named options::spec structures.
 */

inisections::inisections
(
    const std::string & ininame,
    inisections::specification & spec
) :
    m_app_version   ("Cfg66-based application configuration file"),
    m_directory     (spec.file_directory),
    m_name          (spec.file_basename),
    m_extension     (spec.file_extension),
    m_config_type   (),
    m_description   (spec.file_description),
    m_section_list  ()
{
    if (! ininame.empty())
    {
        /* WHAT ABOUT the dir/base.ext info in the spec parameter!??? */

        (void) util::filename_split(ininame, m_directory, m_name);
        m_extension = util::file_extension(m_name);
        if (m_extension[0] == '.')
            m_config_type = m_extension.substr(1);
        else
            m_config_type = m_extension;
    }
    for (auto sec : spec.file_sections)     /* vector of specref (wrappers) */
    {
        /*
         * Create a new inisection. Using the stored std::ref() object and
         * the file-extension. Don't override the section name in
         * that constructor, use the name in the inisection::specification.
         */

        inisection ini(sec.get(), m_extension); /* leave 3rd argument empty */
        add(ini);

        /*
         * TODO. Provide a way to add all named options to the globql inimap
         */
    }
}

/**
 *  Look up an inisection in this inisections object using the section name.
 *  Returns a reference to a an inactive inisection if not found.
 *  These functions must not call each other, else... recursion.
 *
 *  And how do we prevent the caller from modifying the first one?
 *  The caller must check with inisection::active().
 */

const inisection &
inisections::find_inisection (const std::string & sectionname) const
{
    static inisection s_inactive_inisection;
    for (const auto & section : m_section_list)
    {
        if (section.name() == sectionname)
            return section;
    }
    return s_inactive_inisection;
}

/**
 *  In C++17 we can replace "static_cast<const inisections &>(*this)"
 *  with "std::as_const(*this)".
 */

inisection &
inisections::find_inisection (const std::string & sectionname)
{
    return const_cast<inisection &>
    (
        static_cast<const inisections &>(*this).find_inisection(sectionname)
    );
}

/**
 *  In the actual file writing, we will let the enclosing file class (a
 *  variant on cfg::configfile) write the date first.
 */

std::string
inisections::settings_text () const
{
    std::string filespec = util::filename_concatenate(m_directory, m_name);
    std::string result = "# ";
    result += m_app_version + "\n# INI: "; // result += s_stock_file_intro + "\n"
    result += filespec + "\n# ";
    result += m_description + "\n#";

    for (auto sec : m_section_list)
        result += sec.settings_text();

    return result;
}

#if defined CFG66_USE_INIFILES

/*------------------------------------------------------------------------
 * inifiles
 *------------------------------------------------------------------------*/

inifiles::inifiles (const std::string & name) :
    m_name          (name),
    m_file_list     (),
    m_all_options   ()
{
    // no code
}

/**
 *  Adds an inisections to the list. In addition, it adds all of the options from
 *  all the inisections into the inimap.
 */

bool
inifiles::add (const inisections & file)
{
    m_file_list.push_back(file);

    inisections & ncfile = const_cast<inisections &>(file);
    bool result = ncfile.add_options_to_map(m_all_options);
    return result;
}

#endif      // defined CFG66_USE_INIFILES

/*------------------------------------------------------------------------
 * Free functions
 *------------------------------------------------------------------------*/

const inisection &
get_inifile_cfg66_section ()
{
    static inisection s_ini_section(inifile_cfg66_data, "");
    return s_ini_section;
}

const inisection &
get_inifile_comment_section ()
{
    static inisection s_ini_section(inifile_comment_data, "");
    return s_ini_section;
}

}           // namespace cfg

/*
 * inisections.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

