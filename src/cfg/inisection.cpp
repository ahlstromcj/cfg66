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
 * \updates       2024-07-28
 * \license       See above.
 *
 *  See the inisections class and modules for details.
 */

#include "cfg/appinfo.hpp"              /* cfg::appinfo data structure      */
#include "cfg/inisection.hpp"           /* cfg::inisection classes          */
#include "util/strfunctions.hpp"        /* util::word_wrap()                */

namespace cfg
{

/*--------------------------------------------------------------------------
 * inisection
 *--------------------------------------------------------------------------*/

/**
 *  This constructor creates an inisection for the optional global options.
 *
 * \param loadglobal
 *      If true (the default is options::stock == true), then the default
 *      global stock options are loaded.
 */

inisection::inisection (bool loadglobal) :
    m_config_type           (),
    m_name                  (),
    m_section_description   ("Default global options."),
    m_option_names          (),
    m_option_set            (loadglobal)
{
    if (loadglobal)
    {
        options::container & opspecs = option_set().option_pairs();
        for (const auto & opt : opspecs)
            add_name(opt.first);
    }
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
    const std::string & extension
) :
    m_config_type           (),
    m_name                  (spec.sec_name),
    m_section_description   (spec.sec_description),
    m_option_names          (),
    m_option_set            (spec.sec_optionlist, extension, m_name)
{
    if (extension[0] == '.')
        m_config_type = extension.substr(1);
    else
        m_config_type = extension;

#if defined PLATFORM_DEBUG
    if (m_name.empty())
        printf("inisection(): no section name\n");

    if (m_section_description.empty())
        printf("inisection(): no section description\n");
#endif

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
    result += description_commented();          /* description_wrapped() */
//  result += "\n";
    result += name();
//  result += "\n\n";
    for (const auto & s : option_names())
    {
        result += option_set().setting_line(s);
//      result += "\n";
    }
    return result;
}

/**
 *  If this is not the main configuration section, "[Cfg66]" or whatever the
 *  application changed it to at startup, the section configuration type,
 *  section name, the section description, then the help text for all
 *  options in the set.
 *
 *  rc:[audio]
 */

std::string
inisection::cli_help_text () const
{
    std::string result;
    if (get_main_cfg_section_name() != name())
    {
        bool havenames = false;
        std::string enabledoptshelp = option_set().cli_help_text();
        if (! enabledoptshelp.empty())
        {
#if defined USE_COLOR_CLI_HELP_TEXT
            bool showcolor = is_a_tty();
            if (showcolor)
                result += level_color(3);           /* see appinfo module   */
#endif
            if (! config_type().empty())
            {
                result += config_type();
                result += ":";
                havenames = true;
            }
            if (! name().empty())
            {
                result += name();
#if defined SHOW_WHOLE_DESCRIPTION
                result += "\n";
#endif
                havenames = true;
            }
#if defined USE_COLOR_CLI_HELP_TEXT
            if (showcolor)
                result += level_color(0);
#endif
            if (! section_description().empty())
            {
#if defined SHOW_WHOLE_DESCRIPTION          // way too much for --help
                std::string formatted = util::hanging_word_wrap
                (
                    section_description(), 0, options::terminal_width
                );
                result += formatted;
                result += "\n\n";
#else
                std::string line = util::first_sentence(section_description());
                if (havenames)
                    result += " ";

                result += line;
                result += "\n\n";
#endif
            }
            result += enabledoptshelp;      // option_set().cli_help_text()
        }
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

std::string
inisection::debug_text () const
{
    std::string result = config_type();
    result += ":";
    result += name();
    result += "\n";
    result += option_set().debug_text();
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
inisection::description_wrapped () const
{
    return util::word_wrap
    (
        section_description(), options::terminal_width, '#'
    );
}

/**
 *  Similar to description_wrapped(), but newlines are respected and used.
 *  This feature is useful when the description contains lists, for
 *  example.
 *
 *  The program is responsible for text width and for formatting.
 */

std::string
inisection::description_commented () const
{
    return util::line_comments(section_description(), '#');
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
 *  This function wraps the "[Cfg66]" inisection::specification in a
 *  function so that we can guarantee it is initialized when it is
 *  first used.
 */

const inisection::specification &
stock_cfg66_data ()
{
    static inisection::specification s_cfg66_data
    {
        "[Cfg66]",

        /*
         * This description is formatted exactly as it should appear,
         * with all lines ending with a newline.
         */

"This file holds the main configuration data for Cfg66-compliant\n"
"applications. It follows a format similar to the old INI files of MS-DOS.\n"
"\n"
"'config-type' can be used to make sure the right kind of file is in use.\n"
"'version' helps the application to detect older configuration files. See\n"
"the 'session' specification for the common 'quiet' and 'verbose' options.\n"
        ,
        {
            {
                "config-type",
                {
                    options::code_null, options::kind::string, options::disabled,
                    "session", "", false, false,
                    "The type of configuration file.", false
                }
            },
            {
                "version",
                {
                    options::code_null, options::kind::integer, options::disabled,
                    "0", "", false, false,
                    "Configuration file version.", false
                }
            }
        }
    };
    return s_cfg66_data;
}

/**
 *  This function guarantees that the stcok comment data is not just
 *  zero-initialized, but fully initialized, before usage.
 */

const inisection::specification &
stock_comment_data ()
{
    static inisection::specification s_comment_data
    {
        "[comments]",

"The [comments] section holds user documentation for this file. The first\n"
"empty, hash-commented, or tag (section) line ends the comment.\n"
" \n"
"Use a space for line breaks as done in the line above.\n"  // note the newline
        ,
        {
            {
                "comment",
                {
                    options::code_null, options::kind::section, options::disabled,
"Add your comment block here. Comments are a 'section' option. For 'section'\n"
"options, there are no 'variable = value' lines, just lines that are read\n"
"as is.\n"
"  \n"
"This is the next paragraph, separated from the above with a line blank\n"
"except for a single space.\n"
                    ,
                    "",
                    false, false, "Configuration file user comments.", false
                }
            }
        }
    };
    return s_comment_data;
}

}           // namespace cfg

/*
 * inisections.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

