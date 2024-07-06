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
 * \file          inimanager.hpp
 *
 *      Provides a way to hold all options from multiple INI-style files and
 *      multiple INI file sections.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-19
 * \updates       2024-07-06
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
#include "cli/multiparser.hpp"          /* cli::multiparser class           */

namespace cfg
{

/*------------------------------------------------------------------------
 * inimanager
 *------------------------------------------------------------------------*/

/**
 *  Collects all of the option objects into one object for straightforward
 *  lookup by section-name and option-name.
 *
 *  Meant for unique-option names, spanning many inifiles and inisections.
 */

class inimanager
{
    friend class cli::multiparser;

public:

    /**
     *  An inimanager::sections holds actual inisections objects. They are created
     *  on the fly from static inisections::specifications definitions, so
     *  we need to store copies.
     *
     *      using sectionsref = std::reference_wrapper<inisections>;
     *
     *  Remember that an inisections object represents all of the sections
     *  in a single INI file.
     */

    using sections = std::map<std::string, inisections>;

private:

    /**
     *  Allows the collection of command-line options that span multiple
     *  configuration files.
     */

    cli::multiparser m_multi_parser;

    /**
     *  A map of inisections by configuration type (e.g. "rc").
     */

    sections m_sections_map;

public:

    inimanager ();

    std::string help_text () const;

    int count () const
    {
        return int(m_sections_map.size());
    }

    bool active () const
    {
        return count() > 0;
    }

    /*
     *  When called by an app, this non-const version is chosen as the
     *  closest match to the call. For now we prepend a wart to the
     *  name of the internal version.
     */

    cli::multiparser & multi_parser ()
    {
        return m_multi_parser;
    }

    const cli::multiparser & multi_parser () const
    {
        return m_multi_parser;
    }

    bool parse_cli (int argc, char * argv [])
    {
        return multi_parser().parse(argc, argv);
    }

    const sections & sections_map () const
    {
        return m_sections_map;
    }

    bool add_inisections
    (
        const std::string & cfgtype,
        inisections::specification & op
    );

    /*
     *  These will return a dummy (empty inisections) reference if not found.
     */

    const inisections & find_inisections (const std::string & cfgtype) const;
    inisections & find_inisections (const std::string & cfgtype);

    const inisection & find_inisection
    (
        const std::string & cfgtype,
        const std::string & sectionname
    ) const;
    const options & find_options
    (
        const std::string & cfgtype,
        const std::string & sectionname
    ) const;
    const options::spec & find_options_spec
    (
        const std::string & cfgtype,
        const std::string & sectionname,
        const std::string & optionname
    ) const;

private:

    sections & sections_map ()
    {
        return m_sections_map;
    }

    inisection & find_inisection
    (
        const std::string & cfgtype,
        const std::string & sectionname
    );
    options & find_options
    (
        const std::string & cfgtype,
        const std::string & sectionname
    );
    options::spec & find_options_spec
    (
        const std::string & cfgtype,
        const std::string & sectionname,
        const std::string & optionname
    );

};          // class inimanager

}           // namespace cfg

#endif      // CFG66_CFG_INIMAP_HPP

/*
* inimanager.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

