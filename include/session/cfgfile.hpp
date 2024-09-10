#if ! defined CFG66_SESSION_CFGFILE_HPP
#define CFG66_SESSION_CFGFILE_HPP

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
 * \file          cfgfile.hpp
 *
 *  This module declares an INI-handling class using configfile functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2024-09-09
 * \updates       2024-09-09
 * \license       GNU GPLv2 or above
 *
 *  An cfgfile is a configfile that overrides the parse() and write()
 *  functions.
 */

#include "cfg/configfile.hpp"           /* cfg::configfile class            */

namespace cfg
{

/**
 *    A class for handling the INI-style file for the session configuration.
 *    It has a number of items that need to be handled differently from
 *    the "generic" options setup..
 */

class cfgfile : public configfile
{

private:

    // TODO

public:

    cfgfile
    (
        const std::string & filename    = "",
        const std::string & cfgtype     = ""
    );

    cfgfile () = delete;
    cfgfile (cfgfile &&) = delete;
    cfgfile (const cfgfile &) = delete;
    cfgfile & operator = (const cfgfile &) = delete;
    cfgfile & operator = (cfgfile &&) = delete;
    virtual ~cfgfile() = default;

    virtual bool parse () override;
    virtual bool write () override;

protected:

};          // class cfgfile

}           // namespace cfg

#endif      // CFG66_SESSION_CFGFILE_HPP

/*
 * cfgfile.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

