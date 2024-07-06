#if ! defined CFG66_CLI_MULTIPARSER_HPP
#define CFG66_CLI_MULTIPARSER_HPP

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
 * \file          multiparser.hpp
 *
 *  Advanced command-line and configuration-file option handling.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-23
 * \updates       2024-07-06
 * \license       See above.
 *
 *  This class provides a way to look up command-line options specified by
 *  multiple INI files and INI sections.
 */

#include <map>                          /* std::map<> template class        */

#include "cfg/inisections.hpp"          /* cfg::inisections and options     */
#include "cli/parser.hpp"               /* cfg::parser class                */

namespace cfg
{
    class inimanager;
    class inisections;
}

namespace cli
{

/**
 *  Extends cli::parser to handle options from multiple option sections and
 *  configuration files.
 */

class multiparser : public parser
{

    friend class inimanager;

public:

    /**
     *  This map allows quick lookup of the long option name from a character
     *  code.
     */

    using codes = std::map<char, std::string>;

    /**
     *  This structure represents a single option. The strings can be used
     *  to navigate through the inisections (files) and inisection objects
     *  to find the exact option. The name of the option is not part of
     *  this structure because it is present in the key.
     */

    using duo = struct
    {
        std::string config_type;
        std::string config_section;
    };

    /**
     *  Allows for lookup of the duo structure from the long option name.
     */

    using names = std::map<std::string, duo>;

private:

    /**
     *  The manager and care-taker of all options, including those
     *  supported by the command line. This reference definition
     *  implicitly deletes the assignment operators.
     */

    cfg::inimanager & m_ini_manager;

    /**
     *  The currently active options. If null. then a dummy empty one is
     *  used so that access functions fail. This set is accessed by the
     *  virtual function option_set().
     */

    mutable cfg::options * m_current_options;

    /**
     *  The set of supported command-line option character codes.
     */

    codes m_code_mappings;

    /**
     *  The complete set of configurations options.
     */

    names m_cli_mappings;

public:

    multiparser () = delete;
    multiparser (cfg::inimanager & mgr);
    multiparser (const multiparser & other) = default;
    multiparser (multiparser && other) = default;
    multiparser & operator = (const multiparser & other) = delete;
    multiparser & operator = (multiparser && other) = delete;
    virtual ~multiparser ();

    virtual const cfg::options & option_set () const override;
    virtual cfg::options & option_set () override;
    virtual bool parse (int argc, char * argv []) override;
    virtual std::string help_text () const override;

    const cfg::options & find_option_set
    (
        const std::string & configtype,
        const std::string & sectionname
    ) const;
    cfg::options & find_option_set
    (
        const std::string & configtype,
        const std::string & sectionname
    );

    bool cli_mappings_add (cfg::inisections::specification & spec);
    bool cli_mappings_add
    (
        const cfg::options::container & opts,
        const std::string & configtype,
        const std::string & configsection
    );
    bool lookup_names
    (
        const std::string & clioptname,         /* one or more characters   */
        std::string & cfgtype,
        std::string & cfgsection
    );

    const names & cli_mappings () const
    {
        return m_cli_mappings;
    }

    const cfg::inimanager & ini_manager () const
    {
        return m_ini_manager;
    }

private:

    const codes & code_mappings () const
    {
        return m_code_mappings;
    }

    codes & code_mappings ()
    {
        return m_code_mappings;
    }

    names & cli_mappings ()
    {
        return m_cli_mappings;
    }

    cfg::inimanager & ini_manager ()
    {
        return m_ini_manager;
    }

};          // class multiparser

}           // namespace cli

#endif      // CFG66_CLI_MULTIPARSER_HPP

/*
 * multiparser.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

