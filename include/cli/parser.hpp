#if ! defined CFG66_CLI_CLIPARSER_HPP
#define CFG66_CLI_CLIPARSER_HPP

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
 * \file          parser.hpp
 *
 *  Basic command-line and configuration-file option handling.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2023-08-02
 * \license       See above.
 *
 *  Provides for the handling of options specifications.  This module is
 *  appropriate for simple option sets and the command-line.  For a more
 *  advanced parser, see the cfg::parser class.
 *
 * Direct support:
 *
 *  -h, --help
 *  -v, --version
 *  -V, --verbose
 *  -o, --option log=file.log
 *
 * Appinfo support:
 *
 *  We still need to figure out the best way to concatenate option lists.
 *
 *  -H, --home dir                      Calls set_home_cfg_directory(dir)
 *  -c, --config file                   Calls set_home_cfg_file(file)
 *  -l, --client-name ???
 *
 *  The --config option is, by default, the session file in the default
 *  --home directory.
 *
 *
 */

#include <string>                       /* std::string class                */
#include <vector>                       /* std::vector container            */

#include "cfg/options.hpp"              /* cfg::options class               */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cli
{

/**
 *  Accessor function class. This class manages two sets of options: the main
 *  options set (which contains the default values and empty or false actual
 *  values), and the actual options, which contains only the modified values
 *  (though there is a way to provide the whole set, modified or not.
 */

class parser
{

private:

    /**
     *  The complete set of non-file or single-file configurations options
     *  supported by this class.
     */

    cfg::options m_option_set;

    /**
     *  Useful to quickly determine that the app was run only to see the help
     *  or the version. And also shows/set some very common/universal options.
     *  The verbosity and log-file options could be included in a particiular
     *  configuration file, but the other options do not need to be saved.
     */

    bool m_help_request;
    bool m_version_request;
    bool m_verbose_request;
    bool m_description_request;
    bool m_use_log_file;
    std::string m_log_file;

public:

    parser ();
    parser
    (
        const cfg::options & optset,
        const std::string & filename    = "",
        const std::string & sectionname = ""
    );
    parser (const parser & other) = default;
    parser & operator = (const parser & other) = default;
    ~parser () = default;

    bool parse (int argc, char * argv []);
    bool check_option
    (
        int argc, char * argv [],
        const std::string & token,
        bool must_exist = true
    ) const;

    void reset ()
    {
        m_option_set.reset();
    }

    void initialize ()
    {
        m_option_set.initialize();
    }

    void clear ()
    {
        m_option_set.clear();
    }

    cfg::options & option_set ()
    {
        return m_option_set;
    }

    const cfg::options & option_set () const
    {
        return m_option_set;
    }

    bool add (const cfg::options & opt)
    {
        return option_set().add(opt);
    }

    bool add (const cfg::options::option & s)
    {
        return option_set().add(s);
    }

    bool verify () const
    {
        return option_set().verify();
    }

    bool change_value
    (
        const std::string & name,
        const std::string & value,
        bool fromcli = false
    )
    {
        return option_set().change_value(name, value, fromcli);
    }

    bool is_boolean (const std::string & name) const
    {
        return option_set().option_is_boolean(name);
    }

    bool modified () const
    {
        return option_set().modified();
    }

    void unmodify (const std::string & name)
    {
        option_set().unmodify(name);
    }

    void unmodify_all ()
    {
        option_set().unmodify_all();
    }

    std::string description (const std::string & name) const
    {
        return option_set().description(name);
    }

    std::string help_line (const std::string & name) const
    {
        return option_set().help_line(name);
    }

    std::string value (const std::string & name) const
    {
        return option_set().value(name);
    }

    std::string default_value (const std::string & name) const
    {
        return option_set().default_value(name);
    }

    std::string help_text () const
    {
        return option_set().help_text();
    }

    std::string debug_text () const
    {
        return option_set().debug_text();
    }

    std::string description_text () const
    {
        return option_set().description();          /* shows all options    */
    }

    bool help_request () const
    {
        return m_help_request;
    }

    bool version_request () const
    {
        return m_version_request;
    }

    bool verbose_request () const
    {
        return m_verbose_request;
    }

    bool description_request () const
    {
        return m_description_request;
    }

    bool use_log_file () const
    {
        return m_use_log_file;
    }

    const std::string log_file () const
    {
        return m_log_file;
    }

private:

    bool extract_value (std::string & token, std::string & value);
    bool parse_o_option
    (
        const std::string & name,  const std::string & value
    );
    bool parse_value
    (
        int argc, char * argv [], int index,
        const std::string & token
    );

};          // class parser

}           // namespace cli

#endif      // CFG66_CLI_CLIPARSER_HPP

/*
 * parser.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

