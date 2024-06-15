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
 * \updates       2024-06-15
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
 *
 *
 *
 * Classes and structures:

inisections ::
    specification ::
        sectionrefs : vector<reference_wrapper<inisection::specficiations>>
    sectionrefs

inisection ::
    specification ::
        options::container
    names (hmmmmm)
    options


 */

#include "cfg/inisections.hpp"          /* cfg::inisections classes         */
#include "util/filefunctions.hpp"       /* util::filename_split() etc.      */
#include "util/strfunctions.hpp"        /* util::word_wrap()                */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

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
 *  This function looks at an inisections::specification. Each of those contains
 *  a list of inisection::specifications, and each of those contains
 *  a set of options in a cfg::options::container.
 *
 *  We loop through the inisection::specifications in the
 *  inisections::specification, and add each of them to the map.
 *
 *  If this function fails, it means a section name is not unique. All
 *  section names must be unique.
 *
 *  What about [comments] and [Cfg66] though? They are basically read-only
 *  and will never be modified, except via access from each INI file
 *  module. They don't need to be in this map.  We should skip them.
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

/*------------------------------------------------------------------------
 * inisection
 *------------------------------------------------------------------------*/

/**
 *  This constructor grabs the names of each option from the options object
 *  that is created, and adds them to the list.
 *
 *  The quandary we have is that we have a name as key value, and a name as
 *  the name for the option.  They must match.  But if the programmer
 *  makes them different, then the option won't be found. To guarantee the option
 *  is found, we have to use the map's key value, not the option's name.
 *
 *  Note that we might be able to use the key value to change the order of
 *  the options.
 */

inisection::inisection
(
    inisection::specification & spec,
    const std::string & extension,
    const std::string & sectname
) :
    m_extension             (extension),
    m_name                  (sectname.empty() ? spec.sec_name : sectname),
    m_section_description   (spec.sec_description),
    m_option_names          (),
    m_options               (extension, sectname)   /* pairs filled below   */
{
    options opset = option_set();
    options::container opspecs = opset.option_pairs();
    for (const auto & opt : opspecs)
    {
        options::option p = std::make_pair(opt.first, opt.second);
        if (m_options.add(p))
            add_name(opt.first);
    }
}

std::string
inisection::settings_text () const
{
    std::string result = description_commented();
    result += "\n";
    result += name();
    result += "\n\n";
    for (const auto & s : option_names())
    {
        result += option_set().setting_line(s);
        result += "\n";
    }
    return result;
}

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

/**
 *  Adds the "#" comment marker to the beginning of each description
 *  line. This function assumes the description is already in lines of
 *  less than 78 characters and that each has a newline already appended.
 *
 *  Later we might support automatic wraparound based on the static
 *  width value defined in the cfg::options class.
 */

std::string
inisection::description_commented () const
{
#if defined USE_SIMPLISTIC_CODE
    std::string result;
    if (! section_description().empty())
    {
        size_t count = section_description().length();
        result += "# ";
        for (auto c : section_description())
        {
            --count;
            result += c;
            if (c == '\n' && count > 0)
                result += "# ";
        }
    }
    return result;
#else
    return util::word_wrap
    (
        section_description(), options::terminal_width, '#'
    );
#endif
}

/*------------------------------------------------------------------------
 * Reusable objects for "[Cfg66]" and "[comments]" ???
 *------------------------------------------------------------------------*/

/**
 *  This object cannot be const because it is included in a vector of
 *  non-const items. Luckily, one has to go out of one's way to modify
 *  this section specification.
 */

inisection::specification inifile_cfg66_data
{
    "[Cfg66]",
    {
"This file holds the main configuration data for Cfg66-compliant applications.\n"
"It follows a format similar to the old INI files of MS-DOS.\n"
"'config-type' can be used to make sure the right kind of file is in use.\n"
"'version' helps the application to detect older configuration files.\n"
"See the 'session' specification for the common 'quiet' and 'verbose' options.\n"
    },
    {
        {
            "config-type",
            {
                options::code_null, "string", options::disabled,
                "session", "session", false, false,
                "The type of configuration file.", false
            }
        },
        {
            "version",
            {
                options::code_null, "int", options::disabled,
                "0", "0", false, false,
                "Configuration file version.", false
            }
        }
    }
};

inisection::specification inifile_comment_data
{
    "[comments]",
    {
"[comments] holds user documentation for this file. The first empty, hash-\n"
"commented, or tag line ends the comment. Use a space for line breaks.\n"
    },
    {
        {
            "comment",
            {
                options::code_null, "section", options::disabled,
                "Add your comment block here.", "Add your comment block here.",
                false, false, "Configuration file user comments.", false
            }
        }
    }
};

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
    m_app_version   ("Cfg66-based application configuration file"),
    m_directory     (),
    m_name          (),
    m_extension     (),
    m_description   ("This is a generic configuration file."),
    m_section_list  ()
{
    if (! ininame.empty())
    {
        (void) util::filename_split(ininame, m_directory, m_name);
        m_extension = util::file_extension(m_name);
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
    m_directory     (),
    m_name          (),
    m_extension     (),
    m_description   (spec.file_description),
    m_section_list  ()
{
    if (! ininame.empty())
    {
        /* WHAT ABOUT the dir/base.ext info in the spec parameter!??? */

        (void) util::filename_split(ininame, m_directory, m_name);
        m_extension = util::file_extension(m_name);
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

/**
 *  In the actual file writing, we will let the enclosing file class (a
 *  variant on cfg::configfile) write the date first.
 *
 *  IS THIS USEFUL???
 */

std::string
inisections::settings_text () const
{
    std::string result = m_app_version + "\n";
    std::string filespec = util::filename_concatenate(m_directory, m_name);
    result += s_stock_file_intro + "\n";
    result += filespec + "\n";
    result += m_description + "\n";

    for (auto sec : m_section_list)
    {
        result += sec.settings_text();
    }

#if 0
    std::string result = description_commented();
    result += "\n";
    result += name();
    result += "\n\n";
    for (const auto & s : option_names())
    {
        result += option_set().setting_line(s);
        result += "\n";
    }
#endif
    return result;
}

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

