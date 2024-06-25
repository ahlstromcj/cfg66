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
 * \updates       2024-06-24
 * \license       See above.
 *
 */

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
     *  This structure represents a single command-line option, but
     *  includes only the name and the code.
     */

    using pair = struct
    {
        std::string option_name;
        char option_code;
    };

    /**
     *  This structure represents a single option. The strings can be used
     *  to navigate through the inisections (files) and inisection objects
     *  to find the exact option.
     */

    using trio = struct
    {
        std::string config_type;
        std::string config_section;
        std::string config_option;
    };

    /**
     *
     */

    using map = std::map<pair, trio>;

private:

    /**
     *  The complete set of configurations options.
     */

    map m_cli_mappings;

public:

    multiparser ();
    multiparser (const multiparser & other) = default;
    multiparser & operator = (const multiparser & other) = default;
    virtual ~multiparser () = default;

    virtual cfg::options & option_set () override;
    virtual const cfg::options & option_set () const override;

    bool cli_mappings_add (cfg::inisections::specification & spec);

};          // class multiparser

}           // namespace cli

#endif      // CFG66_CLI_MULTIPARSER_HPP

/*
 * multiparser.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

