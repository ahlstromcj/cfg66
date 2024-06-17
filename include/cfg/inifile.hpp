#if ! defined SEQ66_CFG_INIFILE_HPP
#define SEQ66_CFG_INIFILE_HPP

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
 * \file          inifile.hpp
 *
 *  options files.
 *  This module declares an INI-handling class using configfile functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2018-11-23
 * \updates       2024-06-16
 * \license       GNU GPLv2 or above
 *
 *  This is actually an elegant little parser, and works well as long as one
 *  respects its limitations.
 */

#include "cfg/configfile.hpp"           /* cfg::configfile class            */
#include "cfg/inisections.hpp"          /* cfg::inisections class           */

namespace cfg
{

/**
 *    A class for handling INI-style files.
 */

class inifile : public configfile
{

private:

#if defined CFG66_RCSETTINGS

    /**
     *  Hold a reference to the "rc" settings object. For cfg66, this
     *  will likely be a more expansive object based on inisettings.
     */

    rcsettings & m_rc;

#endif

    /**
     *  The set of "[xyz]" sections, with values, in the file.
     *  This object is usually a global object, meant to apply
     *  to the whole application. Therefore this value should not
     *  be a copy, but a reference, otherwise changes would be
     *  hidden.
     */

    inisections & m_ini_sections;

public:

    inifile
    (
        inisections & sections,
        const std::string & name,
        const std::string & cfgtype
    );

    inifile () = delete;
    inifile (inifile &&) = delete;
    inifile (const inifile &) = delete;
    inifile & operator = (const inifile &) = delete;
    inifile & operator = (inifile &&) = delete;
    virtual ~inifile() = default;

    virtual bool parse () override;
    virtual bool write () override;

protected:

    /**
     *  Sometimes we need to know if there are new data lines at the end of an
     *  existing section.  One clue that there is not is that we're at the
     *  next section marker.  This function tests for that condition.
     *
     * \return
     *      Returns true if m_line[0] is the left-bracket character.

    bool at_section_start () const
    {
        return m_line[0] == '[';
    }
     */

    void parse_section
    (
        std::ifstream & file,
        inisection & section
    );
    void write_section
    (
        std::ofstream & file,
        const inisection & section
    );

};          // class inifile

}           // namespace cfg

#endif      // SEQ66_CFG_INIFILE_HPP

/*
 * inifile.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

