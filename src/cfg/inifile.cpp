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
 * \updates       2024-06-15
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
 * \param fileext
 *      The file-extension for this type of configuration file.
 */

inifile::inifile
(
    inisections & sections,
    const std::string & name,
    const std::string & fileext
) :
    configfile      (name, fileext),
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
    std::ifstream file(name(), std::ios::in | std::ios::ate);
    bool result = ! set_up_ifstream(file);
    if (result)
    {
        // TODO
    }
    return result;
}

/**
 *  Writes out all of the sections using configfile member functions.
 */

bool
inifile::write ()
{
    std::ofstream file(name(), std::ios::out | std::ios::trunc);
    bool result = file.is_open();
    if (result)
    {
        /*
         * Stock INI file header
         */

        util::file_message("Write", name());
        write_date(file);

        /*
         * Stock [Cfg66] and [comments] sections
         */

        const inisection & cfg66sec = get_inifile_cfg66_section();
        write_section(file, cfg66sec);

        const inisection & commsec = get_inifile_comment_section();
        write_section(file, commsec);

        /*
         * The rest of the sections
         */

        const inisections::sectionlist & sections = m_ini_sections.section_list();
        for (const auto & section : sections)
        {
            write_section(file, section);
        }

        /*
         * Stock INI file footer
         */

    }
    else
    {
        util::file_error("Write open fail", name());
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
    file
        << section.name() << "\n\n"
        << section.section_description() << "\n\n"
        ;

    for (const auto & s : section.option_names())
    {
        file << section.option_set().setting_line(s) << "\n";
    }

#if 0
    const options & opset = section.option_set();
    options::container opspecs = opset.option_pairs();
    for (const auto & opt : opspecs)
    {
        const std::string & name = opt.first;
        file << opt.setting_line(name) << "\n";     /* ugh, too indirect!   */
    }
#endif
}

}           // namespace cfg

/*
 * inifile.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

