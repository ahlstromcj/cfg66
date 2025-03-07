#if ! defined CFG66_CLI_PARSER_HPP
#define CFG66_CLI_PARSER_HPP

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
 *  Basic command-line option handling.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2024-09-30
 * \license       See above.
 *
 *  Provides for the handling of options specifications.  This module is
 *  appropriate for simple option sets and the command-line.  For a more
 *  advanced parser, see the cli::multiparser class.
 *
 * Direct support:
 *
 *      -h, --help
 *      -v, --version
 *      -Q, --quiet
 *      -V, --verbose
 *      -o, --option log=file.log
 *
 * Appinfo support:
 *
 *  We still need to figure out the best way to concatenate option lists.
 *
 *      -H, --home dir                  Calls set_home_cfg_directory(dir)
 *      -c, --config file               Calls set_home_cfg_file(file)
 *      -l, --client-name ???
 *
 *  The --config option is, by default, the session file in the default
 *  --home directory.
 *
 *
 */

#include <string>                       /* std::string class                */
#include <vector>                       /* std::vector container            */

#include "cfg/options.hpp"              /* cfg::options class               */

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
     *  supported by this class. This class can handle only one "section" of
     *  options.
     */

    cfg::options m_option_set;

    /**
     *  Error messaging.
     */

    bool m_has_error;
    std::string m_error_msg;

    /**
     *  Like the "-a/--alternative" option in getopt(1), this allows long
     *  options to start with a single '-' characters. The default is false.
     */

    bool m_alternative;

    /**
     *  Useful to quickly determine that the app was run only to see the help
     *  or the version. And also shows/set some very common/universal options.
     *  The verbosity and log-file options could be included in a particiular
     *  configuration file, but the other options do not need to be saved.
     */

    bool m_help_request;

    /**
     *  This also manages the verbose setting in msgfunctions. See
     *  util::set_verbose() and util::verbose.
     */

    bool m_version_request;
    bool m_quiet_request;
    bool m_verbose_request;
    bool m_inspect_request;
    bool m_investigate_request;
    bool m_description_request;

    /**
     *  The settings for a log file can also be made via
     *  session::configuration. But the parser should take precedence.
     *  Note that it is the responsiblity of the application to
     *  actually implement the log file. See the xpc::reroute_stdio()
     *  function in the Xpc66 library project.
     */

    bool m_use_log_file;
    std::string m_log_file;

public:

    parser ();
    parser
    (
        const cfg::options::container & optset,
        const std::string & filename        = "",
        const std::string & sectionname     = "",
        bool use_alternative_long_option    = false
    );
    parser (const parser &) = default;
    parser (parser &) = default;
    parser & operator = (const parser &) = default;
    parser & operator = (parser &) = default;
    virtual ~parser () = default;

    virtual bool parse (int argc, char * argv []);

    virtual const cfg::options & option_set () const
    {
        return m_option_set;
    }

    virtual cfg::options & option_set ()
    {
        return m_option_set;
    }

    virtual std::string cli_help_text () const
    {
        return option_set().cli_help_text();
    }

    virtual std::string help_text () const
    {
        return option_set().help_text();
    }

    bool has_error () const
    {
        return m_has_error;
    }

    const std::string & error_msg () const
    {
        return m_error_msg;
    }

    const std::string & code_list () const
    {
        return option_set().code_list();
    }

    void reset ()
    {
        option_set().reset();
    }

    bool set_value
    (
        const std::string & name,
        const std::string & value
    )
    {
        return option_set().set_value(name, value);
    }

    bool set_value
    (
        cfg::options & optset,
        const std::string & name,
        const std::string & value
    )
    {
        return optset.set_value(name, value);
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

    bool change_value
    (
        cfg::options & optset,
        const std::string & name,
        const std::string & value,
        bool fromcli = false
    )
    {
        return optset.change_value(name, value, fromcli);
    }

    bool check_option
    (
        int argc, char * argv [],
        const std::string & token,
        bool must_exist = true
    ) const;
    bool show_information_only () const;

    void initialize ()
    {
        option_set().initialize();
    }

    void clear ()
    {
        option_set().clear();
    }

    /*
     *  Warning. adding a cfg::options reference directly causes a
     *  temporary to be created, and it leads to recursion.
     */

    bool add (const cfg::options & opt)
    {
        return option_set().add(opt.option_pairs());
    }

    /*
     *  Remember that an "option" is a std::pair<std::string, spec>.
     */

    bool add (const cfg::options::option & s)
    {
        return option_set().add(s);
    }

    bool verify () const
    {
        return option_set().verify();
    }

    bool is_boolean (const std::string & name) const
    {
        return option_set().option_is_boolean(name);
    }

#if defined USE_NAME_OPTION_ACCESSORS

    /*
     * These are not supported in cfg::options.
     */

    bool is_int (const std::string & name) const
    {
        return option_set().option_is_int(name);
    }

    bool is_int_pair (const std::string & name) const
    {
        return option_set().option_is_int_pair(name);
    }

    bool is_float (const std::string & name) const
    {
        return option_set().option_is_float(name);
    }

    bool is_float_pair (const std::string & name) const
    {
        return option_set().option_is_float_pair(name);
    }

    bool is_number (const std::string & name) const
    {
        return option_set().option_is_number(name);
    }

    bool is_list (const std::string & name) const
    {
        return option_set().option_is_list(name);
    }

    bool is_string (const std::string & name) const
    {
        return option_set().option_is_string(name);
    }

    bool is_recents (const std::string & name) const
    {
        return option_set().option_is_recents(name);
    }

    bool is_section (const std::string & name) const
    {
        return option_set().option_is_section(name);
    }

    bool is_dummy (const std::string & name) const
    {
        return option_set().option_is_dummy(name);
    }

#endif  // defined USE_NAME_OPTION_ACCESSORS

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

    std::string debug_text (bool showbuiltins = false) const
    {
        return option_set().debug_text(showbuiltins);
    }

    /**
     *  Somewhat redundant re the --help option.
     */

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

    bool quiet () const
    {
        return m_quiet_request;
    }

    bool quiet_request () const
    {
        return quiet();
    }

    bool verbose () const
    {
        return m_verbose_request;
    }

    bool verbose_request () const
    {
        return verbose();
    }

    bool inspect_request () const
    {
        return m_inspect_request;
    }

    bool investigate_request () const
    {
        return m_investigate_request;
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

protected:

    void help_request (bool flag)
    {
        m_help_request = flag;
    }

    void version_request (bool flag)
    {
        m_version_request = flag;
    }

    void quiet_request (bool flag)
    {
        m_quiet_request = flag;
    }

    void verbose_request (bool flag)
    {
        m_verbose_request = flag;
    }

    void inspect_request (bool flag)
    {
        m_inspect_request = flag;
    }

    void investigate_request (bool flag)
    {
        m_investigate_request = flag;
    }

    void description_request (bool flag)
    {
        m_description_request = flag;
    }

    void use_log_file (bool flag)
    {
        m_use_log_file = flag;
    }

    void log_file (const std::string & fname)
    {
        m_log_file = fname;
    }

    bool token_match
    (
        const std::string & token,
        const std::string & opt,
        char code = 0
    );
    bool extract_value (std::string & token, std::string & value);
    bool parse_o_option
    (
        const std::string & name,
        const std::string & value
    );
    bool parse_value
    (
        int argc, char * argv [], int index,
        const std::string & token
    );

};          // class parser

}           // namespace cli

#endif      // CFG66_CLI_PARSER_HPP

/*
 * parser.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

