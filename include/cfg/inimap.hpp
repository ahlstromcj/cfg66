#if ! defined CFG66_CFG_INIMAP_HPP
#define CFG66_CFG_INIMAP_HPP

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
 * \file          inimap.hpp
 *
 *      Provides a way to hold all options from multiple INI-style files and
 *      multiple INI file sections.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-19
 * \updates       2024-06-19
 * \license       See above.
 *  section.
 *
 *      See the cpp file for details.
 */

#include <functional>                   /* std::reference_wrapper<>         */
#include <string>                       /* std::string class                */
#include <vector>                       /* std::vector container            */

#include "cfg/inisections.hpp"          /* cfg::inisections class and kids  */
#include "cfg/options.hpp"              /* cfg::options class               */

namespace cfg
{

/*------------------------------------------------------------------------
 * inimap
 *------------------------------------------------------------------------*/

/**
 *  Collects all of the option objects into one object for straightforward
 *  lookup by section-name and option-name.
 *
 *  Meant for unique-option names, spanning many inifiles and inisections.
 */

class inimap
{

public:

    /**
     *  Holds reference_wrappers for each option. Also see std::ref and
     *  std::cref.
     */

    using sectionsref = std::reference_wrapper<inisection>;
    using sectionsmap = std::map<std::string, sectionsref>;

private:

    sectionsmap m_sections_map;

public:

    inimap ();

#if defined CFG66_USE_INIMAP
    static bool add_sectionss_to_map (inimap & mapp);
    static bool add_sectionss_to_map (inimap & mapp);
#endif

    int count () const
    {
        return int(m_sections_map.size());
    }

    sectionsmap & sections_map ()
    {
        return m_sections_map;
    }

    const sectionsmap & sections_map () const
    {
        return m_sections_map;
    }

public:

    bool add_sections (const std::string & sections_name, options::spec & op);

};          // class inimap

}           // namespace cfg

#endif      // CFG66_CFG_INIMAP_HPP

/*
* inimap.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

