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
 * \updates       2024-06-26
 * \license       See above.
 *
 *  This class provides a way to look up command-line options specified by
 *  multiple INI files and INI sections.
 */

#include <map>                          /* std::map<> template class        */
#include "cfg/inisections.hpp"          /* cfg::inisections class and kids  */
#include "cli/parser.hpp"               /* cfg::parser class                */

namespace cli
{

/**
 *  Extends cli::parser to handle options from multiple option sections and
 *  configuration files.
 */

class multiparser : public parser
{

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
     *  The set of supported command-line option character codes.
     */

    codes m_code_mappings;

    /**
     *  The complete set of configurations options.
     */

    names m_cli_mappings;

public:

    multiparser ();
    multiparser (const multiparser & other) = default;
    multiparser & operator = (const multiparser & other) = default;
    virtual ~multiparser () = default;

    bool cli_mappings_add (cfg::inisections::specification & spec);
    bool cli_mappings_add (const cfg::options::container & opts);
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

};          // class multiparser

}           // namespace cli

#endif      // CFG66_CLI_MULTIPARSER_HPP

/*
 * multiparser.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

