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
 * \updates       2024-07-31
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
 *  No directory, file-name, extension, or configuration type.
 */

inisections::inisections () :
    m_app_version   ("Cfg66 stock configuration"),
    m_directory     (),
    m_base_name     (),
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
 *
 * \param ininame
 *      Provides the name of the INI file, preferably a full
 *      file-specification, to fill in the members for directory,
 *      base-name, and file-extension/configuration type.
 */

inisections::inisections (const std::string & ininame) :
    m_app_version   ("Cfg66 configuration file"),
    m_directory     (),
    m_base_name     (),
    m_extension     (),
    m_config_type   (),
    m_description   ("A generic configuration file."),
    m_section_list  ()
{
    inisection ini;
    add(ini);
    extract_file_values(ininame);                   /* works if non-empty   */
}

/**
 *  Create an INI file object and populate it with [sections] filled with
 *  "options".
 *
 * For-loop notes:
 *
 *      specref = std::reference_wrapper<inisection::specification>
 *      specrefs = std::vector<specref>
 *
 *      for (auto & sec : spec.file_sections)   // vector of specref wrappers
 *
 *          std::reference_wrapper<inisection::specification> sec :
 *              spec.file_sections
 *
 * \param spec
 *      This structure contains:
 *
 *          -   The type of INI file, such as 'session' or 'rc'. It is
 *              essentially a file extension without the period.
 *          -   The home directory, which might be prepended to a bare
 *              file-name.
 *          -   The name of the home configuration file.
 *          -   A description that goes near the top of the INI file when
 *              it is written to.
 *          -   A list of inisection::specification objects. This object
 *              contains a section name and description plus a container
 *              of named options::spec structures.
 *
 * \param ininame
 *      Information about the file-specification of the INI file. It can
 *      be a full path like:
 *
 *          "~/.config/seq66v2/special-session/seq66v2.session".
 *
 *      The parts of this file name replace whatever the spec specifies.
 *      It can also be just a directory, base-name, and extension, to override
 *      just some of the spec values.
 *
 *      This parameter is optional, since the relevant items can be found
 *      in the spec parameter.
 */

inisections::inisections
(
    inisections::specification & spec,
    const std::string & ininame
) :
    m_app_version   ("Cfg66-based application configuration file"),
    m_directory     (spec.file_directory),
    m_base_name     (spec.file_basename),
    m_extension     (spec.file_extension),
    m_config_type   (spec.file_extension),
    m_description   (spec.file_description),
    m_section_list  ()
{
    extract_file_values(ininame);                   /* works if non-empty   */
    for (auto sec : spec.file_sections)             /* see banner notes     */
    {
        /*
         * Create a new inisection. Using the stored std::ref() object and
         * the file-extension. Don't override the section name in
         * that constructor, use the name in the inisection::specification.
         */

        inisection ini(sec.get(), m_extension);     /* 3rd argument empty   */
        add(ini);

        /*
         * TODO. Provide a way to add all named options to the global
         * inimanager
         */
    }
}

/**
 *  This function can be used to override the following
 *  inisections::specification values:
 *
 *      -   file_directory
 *      -   file_basename
 *      -   file_extension
 *
 *  The configuration type member, m_config_type, can also be set.
 *
 * \param fname
 *      The name of the file, used only if not empty. It can contain
 *      the following parts:
 *
 *              ~/.config/application/application-162.session
 *              Path----------------= Base-name-----=Extension
 *
 *      Only the components that are present will change these members:
 *
 *          -   m_directory
 *          -   m_base_name
 *          -   m_extension
 *          -   m_config_type
 *
 *      If all components are present, the 3 spec values and configuration
 *      type are modified.
 *
 *      If empty, only some fix-ups are made, such as making sure the
 *      file-extension has a period.
 *
 *  This function is kind of an inverse of file_specification().
 */

void
inisections::extract_file_values (const std::string & fname)
{
    if (fname.empty())
    {
        fix_extension(m_extension);
    }
    else
    {
        std::string path, filebase, ext;
        bool has_path = util::filename_split_ext(fname, path, filebase, ext);
        if (has_path)
            m_directory = path;

        if (! filebase.empty())
            m_base_name = filebase;

        if (ext.empty())
            fix_extension(m_extension);
        else
            fix_extension(ext);
    }
}

/**
 *  This function makes sure that m_extension and m_config_type are
 *  appropriate.
 */

void
inisections::fix_extension (const std::string & ext)
{
    std::string extension = ext;
    if (extension[0] != '.')
        extension = "." + extension;

    m_extension = extension;
    m_config_type = m_extension.substr(1);
}

/**
 *  Reconstructs the name of the INI file holding all of the sections
 *  from existing values. It assumes nothing about the values at this
 *  time.
 *
 *  This function is kind of an inverse of extract_file_values().
 *
 *  Also see its usage in the inifile constructor.
 *
 * \param basename
 *      If not empty (the default is empty), this parameter causes some
 *      changes:
 *      -   If the name has a path, it is used as is. This allows
 *          full control of the file-name.
 *      -   If the name does not have a path, the current INI file-name is
 *          reconstructed with a different base-name.  This supports the
 *          use-case where we read in one file from a given directory, and
 *          want to write to another file in that directory.
 */

std::string
inisections::file_specification (const std::string & basename) const
{
    if (util::name_has_path(basename))
    {
        return basename;
    }
    else
    {
        const std::string & base = basename.empty() ? m_base_name : basename ;
        return util::filename_concatenate(m_directory, base, m_extension);
    }
}

/**
 *  In the actual file writing, we will let the enclosing file class (a
 *  variant on cfg::configfile) write the date first.
 */

std::string
inisections::settings_text () const
{
    std::string filespec = util::filename_concatenate(m_directory, m_base_name);
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

std::string
inisections::debug_text () const
{
    std::string result;
    for (auto & sec : section_list())
        result += sec.debug_text();

    return result;
}

/*------------------------------------------------------------------------
 * Finding an inisection object
 *------------------------------------------------------------------------*/

/**
 *  Makes sure the section name is bracketed.
 *
 *  Is this wise? Should we force the programmer to ensure this?
 */

std::string
inisections::fix_section_name (const std::string & s) const
{
    std::string result;
    if (! s.empty())
    {
        if (s.front() != '[' && s.back() != ']')
            result = "[" + s + "]";
        else
            result = s;
    }
    return result;
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
    static inisection s_inactive_inisection{! options::stock};
    std::string name = fix_section_name(sectionname);
    for (const auto & section : section_list())
    {
        if (section.name() == name)
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

}           // namespace cfg

/*
 * inisections.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

