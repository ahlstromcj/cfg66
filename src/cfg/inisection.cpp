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
 * \file          inisection.cpp
 *
 *    Provides a way to more comprehensively automate the reading and
 *    writing of INI-style files.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-19
 * \updates       2024-07-06
 * \license       See above.
 *
 *  See the inisections class and modules for details.
 */

#include "cfg/inisection.hpp"           /* cfg::inisection classes          */
#include "util/strfunctions.hpp"        /* util::word_wrap()                */

namespace cfg
{

/*------------------------------------------------------------------------
 * inisection
 *------------------------------------------------------------------------*/

/**
 *  This constructor creates an inisection for the optional stock options.
 */

inisection::inisection () :
    m_config_type           (),
    m_name                  (),
    m_section_description   ("Internal default stock options."),
    m_option_names          (),
    m_option_set            ()
{
    /*
     * Iffy
     *
     * m_option_set.reset(options::stock);
     *
     */

    options::container & opspecs = option_set().option_pairs();
    for (const auto & opt : opspecs)
        add_name(opt.first);
}

/**
 *  This constructor grabs the names of each option from the options object
 *  that is created, and adds them to the list.
 *
 *  The quandary we have is that we have a name as key value, and a name as
 *  the name for the option.  They must match.  But if the programmer
 *  makes them different, then the option won't be found. To guarantee the
 *  option is found, we have to use the map's key value, not the option's name.
 *
 *  Note that we might be able to use the key value to change the order of
 *  the options.
 *
 *  Also note that the options are initialized by this constructor. This means
 *  that the option value is set to the default (saves the programmer having
 *  to add the value to the specification) and turns off the modified and
 *  read-from-cli flags.
 */

inisection::inisection
(
    inisection::specification & spec,
    const std::string & extension,
    const std::string & sectname
) :
    m_config_type           (),
    m_name                  (sectname.empty() ? spec.sec_name : sectname),
    m_section_description   (spec.sec_description),
    m_option_names          (),
    m_option_set            (spec.sec_optionlist, extension, sectname)
{
    if (extension[0] == '.')
        m_config_type = extension.substr(1);
    else
        m_config_type = extension;

#if defined WE_NEED_THIS_CODE
    options::container & opspecs = spec.sec_optionlist;
    options::init_container(opspecs);
    for (const auto & opt : opspecs)
    {
        options::option p = std::make_pair(opt.first, opt.second);
        if (option_set().add(p))
            add_name(opt.first);
    }
#endif
}

/**
 *  Could also create an options::settings_text() and use that here.
 */

std::string
inisection::settings_text () const
{
    std::string result = "\n";
    result += description_commented();
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

std::string
inisection::help_text () const
{
    std::string result = name();
    result += "\n";
    result += section_description();
    result += "\n";
    result += option_set().help_text();
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

/**
 *  This function gets the desired option's spec structure by name.
 */

const options::spec &
inisection::find_option_spec (const std::string & name) const
{
    return option_set().find_spec(name);
}

/**
 *  Note: if we move this project to C++17 the following line could be used:
 *
 *  const_cast<options::spec &>(std::as_const(*this).find_option_spec(name)
 */

options::spec &
inisection::find_option_spec (const std::string & name)
{
    return const_cast<options::spec &>
    (
        static_cast<const inisection &>(*this).find_option_spec(name)
    );
}

/*------------------------------------------------------------------------
 * Reusable objects for "[Cfg66]" and "[comments]"
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
"The main configuration data for Cfg66-compliant applications, similar\n"
"to an MS-DOS INI file.\n"
"'config-type' indicates the kind of file and file extension.\n"
"'version' allows detection of older configuration files.\n"
    },
    {
        {
            "config-type",
            {
                options::code_null, "string", options::disabled,
                "session", "", false, false,
                "The type of configuration file.", false
            }
        },
        {
            "version",
            {
                options::code_null, "integer", options::disabled,
                "0", "", false, false,
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
                "Add your comment block here.", "",
                false, false, "Configuration file user comments.", false
            }
        }
    }
};

}           // namespace cfg

/*
 * inisections.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

