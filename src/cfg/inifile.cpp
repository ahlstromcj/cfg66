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
 * \file          inifile.cpp
 *
 *  This module defines an INI-handling class using configfile functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2018-11-23
 * \updates       2024-07-16
 * \license       GNU GPLv2 or above
 *
 */

#include "cfg/inifile.hpp"              /* cfg::inifile class               */
#include "util/msgfunctions.hpp"        /* util::msgfunctions module        */

namespace cfg
{

/**
 *  Provides the string constructor for a configuration file.
 *
 * \param name
 *      The name of the configuration file.
 *
 * \param cfgtype
 *      The file-extension for this type of configuration file.
 */

inifile::inifile
(
    const inisections & sections,
    const std::string & filename,
    const std::string & cfgtype
) :
    configfile      (filename, cfgtype),
    m_ini_sections  (sections)
{
    // no code needed
}

/**
 *  Reads in all of the sections using configfile member functions.
 */

bool
inifile::parse ()
{
    std::ifstream file(file_name(), std::ios::in | std::ios::ate);
    bool result = set_up_ifstream(file);
    if (result)
    {
        util::file_message("Read", file_name());
        std::string s = parse_version(file);
        if (s.empty() || file_version_old(file))
        {
            /*
             * To do:
             *
             *      1.  Flag for saving in the new format.
             *      2.  Verify the config-type valuie (e.g. "rc").
             *
             *          rc().auto_save(true);
             *          file_type() == get_variable
             *          (
             *              file, "[Cfg66]", "config-type", 0 // position
             *          )
             */
        }
        if (result)
        {
            /*
             *  To do: modify the stored comment.

            std::string comments = parse_comments(file);
            if (! comments.empty())
            {
#if defined PLATFORM_DEBUG_TMI
                printf("[comments] %s\n", comments.c_str());
#endif
            }
             */

            inisections::sectionlist & sections =
                const_cast<inisections::sectionlist &>
                (
                    m_ini_sections.section_list()
                );

            for (auto & section : sections)
                parse_section(file, section);
        }
    }
    return result;
}

void
inifile::parse_section
(
    std::ifstream & file,
    inisection & section
)
{
    options & opset = section.option_set();
    options::container opspecs = opset.option_pairs();
#if defined PLATFORM_DEBUG_TMI
    printf("SECTION %s\n", section.name().c_str());
#endif
    for (auto & opt : opspecs)
    {
        const std::string & name = opt.first;
        if (opset.option_is_section(opt.second))
        {
            std::string value = parse_section_option(file, section.name());
            (void) opset.set_value(name, value);
        }
        else
        {
            std::string value = get_variable(file, section.name(), name);
            (void) opset.set_value(name, value);
        }
    }
}

/**
 *  Writes out all of the sections using configfile member functions.
 */

bool
inifile::write ()
{
    std::ofstream file(file_name(), std::ios::out | std::ios::trunc);
    bool result = file.is_open();
    if (result)
    {
        /*
         * Stock INI file header.
         */

        util::file_message("Write", file_name());
        write_date(file);

        /*
         * Stock [Cfg66] and [comments] sections.
         *
         * It's too problematic to have these built in.
         *
         *  const inisection & cfg66sec = get_inifile_cfg66_section();
         *  write_section(file, cfg66sec);
         *  const inisection & commsec = get_inifile_comment_section();
         *  write_section(file, commsec);
         *
         */

        /*
         * Write the rest of the sections.
         */

        const inisections::sectionlist & sections = m_ini_sections.section_list();
        for (const auto & section : sections)
            write_section(file, section);

        /*
         * Write the stock INI file footer.
         */

        write_cfg66_footer(file);
    }
    else
    {
        util::file_error("Write open fail", file_name());
        result = false;
    }
    return result;
}

/**
 *  Writes out the INI section's name and description. Then gets the
 *  options object, which provides the option pairs, a map of option names
 *  and option specs. From that we get the option name, and then use that
 *  to look up the setting line.  This process is a bit stilted!
 */

void
inifile::write_section
(
    std::ofstream & file,
    const inisection & section
)
{
    if (! section.name().empty())
    {
        file << "\n" << section.name() << "\n\n";
    }
    if (! section.section_description().empty())
    {
        file << section.description_commented() << "\n";
    }

    const options & opset = section.option_set();
    options::container opspecs = opset.option_pairs();
    for (const auto & opt : opspecs)
    {
        const std::string & name = opt.first;
        file << opset.setting_line(name) << "\n";   /* ugh, too indirect!   */
    }
}

}           // namespace cfg

/*
 * inifile.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

