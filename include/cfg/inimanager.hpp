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
 * \updates       2024-09-05
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
     *  An inimanager::sections holds actual inisections objects. They are
     *  created on the fly from static inisections::specifications
     *  definitions, so we need to store copies.
     *
     *      using sectionsref = std::reference_wrapper<inisections>;
     *
     *  Remember that an inisections object represents all of the sections
     *  in a single INI file. The key value is the configuration type,
     *  which is a variation of the file extension of an INI file. A good
     *  example is "rc", which represents a "*.rc" INI file.
     */

    using sections = std::map<std::string, inisections>;

    /**
     *  A convenience structure of pointers for adding inisections data in one
     *  call.
     */

    using sections_specs = std::vector<inisections::specification *>;

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
    inimanager (const options::container & additional);
    inimanager (const inimanager &) = delete;
    inimanager (inimanager &&) = delete;
    inimanager & operator = (const inimanager &) = delete;
    inimanager & operator = (inimanager &&) = delete;
    virtual ~inimanager () = default;

    std::string cli_help_text () const;
    std::string help_text () const;
    std::string debug_text () const;

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

    /*
     * Adds cfg::inisections (a list of std::ref items) for a
     * set of data supporting a single INI file.
     */

    bool add_inisections (inisections::specification & spec);
    bool add_inisections (inimanager::sections_specs & ops);

    /*
     *  These will return a dummy (empty inisections) reference if not found.
     */

    const inisections & find_inisections (const std::string & cfgtype) const;
    inisections & find_inisections (const std::string & cfgtype);
    bool read_sections
    (
        const std::string & fname,
        const std::string & cfgtype
    );
    bool write_sections
    (
        const std::string & fname,
        const std::string & cfgtype
    );
    const inisection & find_inisection
    (
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    ) const;
    const options & find_options
    (
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    ) const;
    const options::spec & find_options_spec
    (
        const std::string & optionname,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    ) const;

    std::string value
    (
        const std::string & name,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    ) const;
    void value
    (
        const std::string & name,
        const std::string & value,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    );
    bool boolean_value
    (
        const std::string & name,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    ) const;
    void boolean_value
    (
        const std::string & name,
        bool value,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    );
    int integer_value
    (
        const std::string & name,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    ) const;
    void integer_value
    (
        const std::string & name,
        int value,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    );
    float floating_value
    (
        const std::string & name,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    ) const;
    void floating_value
    (
        const std::string & name,
        float value,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    );

private:

    sections & sections_map ()
    {
        return m_sections_map;
    }

    inisection & find_inisection
    (
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    );
    options & find_options
    (
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    );
    options::spec & find_options_spec
    (
        const std::string & optionname,
        const std::string & cfgtype     =   global,
        const std::string & sectionname =   global
    );

};          // class inimanager

}           // namespace cfg

#endif      // CFG66_CFG_INIMAP_HPP

/*
* inimanager.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

