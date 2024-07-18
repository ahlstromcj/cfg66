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
 * \updates       2024-07-15
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

/**
 *  Create an inisections that has only one inisection, that being one
 *  holding only the "global" options. See the inisection default constructor.
 */

inisections::inisections () :
    m_app_version   ("Cfg66 stock configuration"),
    m_directory     (),
    m_name          (),
    m_extension     (),
    m_config_type   (),
    m_description   ("A stock configuration, not a file."),
    m_section_list  ()
{
    inisection ini;
    add(ini);
}

/**
 *  Create an inisections, filling in members by splitting the
 *  INI file name.
 */

inisections::inisections (const std::string & ininame) :
    m_app_version   ("Cfg66 configuration file"),
    m_directory     (),
    m_name          (),
    m_extension     (),
    m_config_type   (),
    m_description   ("A generic configuration file."),
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

        if (m_extension.empty())
        {
            if (! ininame.empty())
            {
                (void) util::filename_split(ininame, m_directory, m_name);
                m_extension = util::file_extension(m_name);
            }
        }
        if (m_extension[0] == '.')
            m_config_type = m_extension.substr(1);
        else
            m_config_type = m_extension;
    }

    /*
     * specref = std::reference_wrapper<inisection::specification>
     * specrefs = std::vector<specref>
     *
     * for (auto & sec : spec.file_sections)   // vector of specref (wrappers)
     *
     *  std::reference_wrapper<inisection::specification> sec :
     *      spec.file_sections
     */

#if defined PLATFORM_DEBUG
    size_t index = 0;
#endif

    for (auto sec : spec.file_sections)
    {

#if defined PLATFORM_DEBUG
        const inisection::specification & s = spec.file_sections[index].get();
        std::string desc = s.sec_description.substr(0, 16);
        printf
        (
            "inispec[%2d]: name = '%s', desc = '%s', count = %d\n",
            int(index++), s.sec_name.c_str(), desc.c_str(),
            int(s.sec_optionlist.size())
        );
#endif

        /*
         * Create a new inisection. Using the stored std::ref() object and
         * the file-extension. Don't override the section name in
         * that constructor, use the name in the inisection::specification.
         */

        inisection ini(sec.get(), m_extension); /* leave 3rd argument empty */
        add(ini);

        /*
         * TODO. Provide a way to add all named options to the globql inimanager
         */
    }
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

    for (auto & sec : section_list())
        result += sec.settings_text();

    return result;
}

std::string
inisections::cli_help_text () const
{
    std::string result;
    for (auto & sec : section_list())
        result += sec.cli_help_text();

    return result;
}

std::string
inisections::help_text () const
{
    std::string result;
    for (auto & sec : section_list())
        result += sec.help_text();

    return result;
}

/*------------------------------------------------------------------------
 * Finding an inisection object
 *------------------------------------------------------------------------*/

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
    static inisection s_inactive_inisection{! options::stock};
    for (const auto & section : section_list())
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

/*------------------------------------------------------------------------
 * Finding an options object
 *------------------------------------------------------------------------*/

/**
 *  An options object is useful for calling its various functions
 *  to get to a specific spec field by name. To get it, we first need
 *  to get an inisection object.
 */

const options &
inisections::find_options (const std::string & sectionname) const
{
    static options s_inactive_options{! options::stock};
    const inisection & section = find_inisection(sectionname);
    if (section.active())
        return section.option_set();
    else
        return s_inactive_options;
}

options &
inisections::find_options (const std::string & sectionname)
{
    return const_cast<options &>
    (
        static_cast<const inisections &>(*this).find_options(sectionname)
    );
}

/**
 *  Provides a way to add options to a section.
 *
 * \param specs
 *      Provides the options to be added.
 *
 * \param sectionname
 *      The optional section name, such as "[ports]". If empty (the default),
 *      then the options are added to the stock/global options which have
 *      no INI file or section.
 *
 * \return
 *      Returns true if the additions succeeded.
 */

bool
inisections::add_options
(
    const options::container & specs,
    const std::string & sectionname
)
{
    options & opts = find_options(sectionname);
    bool result = opts.active();
    if (result)
        result = opts.add(specs);

    return result;
}

/*------------------------------------------------------------------------
 * Finding an options::spec by brute-force lookup
 *------------------------------------------------------------------------*/

/**
 *  Finds an options specification by iteration through the contained
 *  inisection objects. This is a brute force lookup.
 */

const options::spec &
inisections::find_option_spec (const std::string & name) const
{
    static options::spec s_inactive_spec;
    for (const auto & section : section_list())
    {
        const options::spec & opt = section.find_option_spec(name);
        if (! options::inactive(opt))
            return opt;
    }
    return s_inactive_spec;
}

options::spec &
inisections::find_option_spec (const std::string & name)
{
    return const_cast<options::spec &>
    (
        static_cast<const inisections &>(*this).find_option_spec(name)
    );
}

/*------------------------------------------------------------------------
 * Free functions
 *------------------------------------------------------------------------*/

const inisection &
get_inifile_cfg66_section ()
{
    static inisection s_ini_section(inifile_cfg66_data, global);
    return s_ini_section;
}

const inisection &
get_inifile_comment_section ()
{
    static inisection s_ini_section(inifile_comment_data, global);
    return s_ini_section;
}

}           // namespace cfg

/*
 * inisections.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

