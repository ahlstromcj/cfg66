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
 * \file          multiparser.cpp
 *
 *      Provides a way to associate command-line options with INI entries.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-24
 * \updates       2024-07-11
 * \license       See above.
 *
 *      The limitations of command-line options as implemented in cli::parser
 *      are space and applicability to a single configuration file and single
 *      option section. One way around this limitation would be to make long
 *      options such as "rc-jack-transport-transport-type=conditional". Ugh.
 *
 *      Another option is to map the single-character code and long name of
 *      the option to the "config-type / sections-name / option-name" trio,
 *      and use those string to navigate to the desired option. In this case,
 *      we have to use a brute-force search to find the option code or name.
 */

#include "c_macros.h"                   /* not_nullptr()                    */
#include "cli/multiparser.hpp"          /* cli::multiparser class           */
#include "cfg/inimanager.hpp"           /* cfg::inimanager & inisections    */
#include "util/msgfunctions.hpp"        /* util::set_verbose() etc.         */

namespace cli
{

/**
 *  Empty options set.
 */

static cfg::options::container s_dummy_options;

/**
 *  Constructors. The first creates an empty options container, but hooks
 *  in the cfg::inimanager that ultimately rules all options.
 */

multiparser::multiparser (cfg::inimanager & mgr) :
    parser              (s_dummy_options),
    m_ini_manager       (mgr),
    m_current_options   (nullptr),
    m_code_mappings     (),
    m_cli_mappings      ()
{
    // no code
}

multiparser::~multiparser ()
{
    // no code
}

/**
 *  This override forwards the request to the inimanager (parent).
 */

std::string
multiparser::cli_help_text () const
{
    return ini_manager().cli_help_text();
}

/**
 *  This override forwards the request to the inimanager (parent).
 */

std::string
multiparser::help_text () const
{
    return ini_manager().help_text();
}

/**
 *  cfg::inisections::specification contains a "file_extension" field that
 *  is essentially the "configuration type". The other field of interest
 *  is "file_sections"; it contains is a vector of references to
 *  inisection::specification structure.
 */

bool
multiparser::cli_mappings_add (cfg::inisections::specification & spec)
{
    std::string configtype = spec.file_extension;
    bool result = ! configtype.empty();
    if (result)
    {
        /*
         *  const class std::reference_wrapper<cfg::inisection::specification>
         *
         *  Requires the get() function to access the reference.
         */

        for (const auto & sec : spec.file_sections)
        {
            const cfg::inisection::specification & isectspec = sec.get();
            std::string configsection = isectspec.sec_name;

            /*
             * sec_optionlist is a cfg::options::container, i.e. a
             * map of option::spec structures keyed by their name:
             *
             *      const std::pair<std::string, options::spec>
             */

            result = cli_mappings_add
            (
                isectspec.sec_optionlist, configtype, configsection
            );
        }
    }
    return result;
}

/**
 *  Adds a container of options to the desired type:[section] pair.
 *
 * \param opts
 *      Provides the list of options.
 *
 * \param configtype
 *      Provides the type of configuration file, e.g. "rc". If empty,
 *      there's no file involved, which is the case with stock options
 *      and any additions to them. The default is an empty string.
 *
 * \param configsection
 *      Provides the name of the INI "section" in force, such as "[audio]".
 *      If empty and \a configtype is empty, then this is the stock options
 *      section. (A better name might be "global" options.) The default is
 *      an empty string.
 *
 *  In ini_set_test, this is the one called.
 */

bool
multiparser::cli_mappings_add
(
    const cfg::options::container & opts,
    const std::string & configtype,
    const std::string & configsection
)
{
    bool result = opts.size() > 0;
    if (result)
    {
        for (const auto & opt : opts)
        {
            bool allowcli = opt.second.option_cli_enabled;
            if (allowcli)
            {
                char optcode = opt.second.option_code;
                std::string optname = opt.first;
                if (optcode > ' ')
                {
                    auto p = std::make_pair(optcode, optname);
                    auto r = code_mappings().insert(p);
                    if (r.second)
                    {
#if defined PLATFORM_DEBUG_TMI
                        printf
                        (
                            "Inserted pair <'%c','%s'>\n",
                            optcode, optname.c_str()
                        );
#endif
                    }
                    else
                    {
                        char tmp[64];
                        snprintf
                        (
                            tmp, sizeof tmp,
                            "Couldn't insert pair <'%c','%s'>\n",
                            optcode, optname.c_str()
                        );
                        util::warn_message(tmp);
                    }
                }

                duo d{configtype, configsection};
                auto p = std::make_pair(optname, d);
                auto r = cli_mappings().insert(p);
                if (r.second)
                {
#if defined PLATFORM_DEBUG_TMI
                    printf
                    (
                        "Inserted option <'%s',('%s',%s)>\n",
                        optname.c_str(), configtype.c_str(),
                        configsection.c_str()
                    );
#endif
                }
                else
                {
                    char tmp[64];
                    snprintf
                    (
                        tmp, sizeof tmp,
                        "Couldn't insert <%s,(%s,%s)>",
                        optname.c_str(), configtype.c_str(),
                        configsection.c_str()
                    );
                    util::warn_message(tmp, "Change option to a unique name");
                }
            }
        }
    }
    return result;
}

/**
 *
 */

const cfg::options &
multiparser::option_set () const
{
    if (not_nullptr(m_current_options))
        return *m_current_options;
    else
        return parser::option_set();
}

cfg::options &
multiparser::option_set ()
{
    return const_cast<cfg::options &>
    (
        static_cast<const multiparser &>(*this).option_set()
    );
}

/**
 *  Override the base-class functions to use the inimanager parent to
 *  find the right option set to use and to access it. If both parameters
 *  are empty, this is the stock/global options.
 */

const cfg::options &
multiparser::find_option_set
(
    const std::string & configtype,
    const std::string & configsection
) const
{
    static cfg::options s_empty_options;
    const cfg::inisection & ini = m_ini_manager.find_inisection
    (
        configtype, configsection
    );
    if (ini.active())
    {
        const cfg::options & opts = ini.option_set();
        return opts;
    }
    return s_empty_options;
}

cfg::options &
multiparser::find_option_set
(
    const std::string & configtype,
    const std::string & configsection
)
{
    return const_cast<cfg::options &>
    (
        static_cast<const multiparser &>(*this).find_option_set
        (
            configtype, configsection
        )
    );
}

/**
 *  Provides an override to look up the desired option set.
 *
 *  -#  If the token is a code (e.g. "-i"), then the corresponding long
 *      option name is looked up.
 *
 *  After that, essentially reimplements parser::parse(); will refine in the
 *  near future.
 */

bool
multiparser::parse (int argc, char * argv [])
{
    bool result = not_nullptr(argv) && ! has_error();
    if (result && argc > 1)
    {
        for (int i = 1; i < argc; ++i)      /* token 0 might be app name    */
        {
            std::string token = argv[i];
            if (token == "--")              /* GNU end-of-options marker    */
                break;

            if (token == "-")               /* ill-formed token, bug out    */
                break;

            if (token[0] != '-')            /* probably a value, so skip    */
                continue;

            /*
             *  1.  Look up the long name if needed.
             */

            std::string longname;
            if (token.length() == 2)
            {
                char code = token[1];
                auto it = code_mappings().find(code);
                if (it != code_mappings().end())
                    longname = it->second;
            }
            else if (token.length() > 2)
            {
                longname = token.substr(2);
            }
            if (token.empty())
            {
                util::error_message("option lookup failed", token);
                break;
            }

            /*
             *  2.  Look up the long name's configuration type and section.
             */

            std::string configtype;
            std::string configsection;
            auto dit = cli_mappings().find(longname);
            if (dit != cli_mappings().end())
            {
                configtype = dit->second.config_type;
                configsection = dit->second.config_section;

                /*
                 *  3.  Get the option-set from the INI section.
                 */

                m_current_options = & find_option_set(configtype, configsection);
                if (m_current_options->active())
                    result = parse_value(argc, argv, i, token);
            }
        }
    }
    if (result)
    {
        description_request(option_set().boolean_value("description"));
        help_request(option_set().boolean_value("help"));
        version_request(option_set().boolean_value("version"));
        inspect_request(option_set().boolean_value("inspect"));
        verbose_request(option_set().boolean_value("verbose"));
        util::set_verbose(verbose_request());           /* see msgfunctions */
        investigate_request(option_set().boolean_value("investigate"));
        util::set_investigate(investigate_request());   /* see msgfunctions */
        log_file(option_set().value("log"));
        use_log_file(! log_file().empty());
    }
    return result;
}

/**
 *  This function looks up the option name and delivers the configuration type
 *  and the configuration section to the caller.
 *
 * \param clioptname
 *      Provides the command-line option name to look up. It should be either
 *      a single character or more, and should not start with "-" or "--".
 *      That will cause this function to fail, since the name will not be
 *      found.
 *
 * \param [out] cfgtype
 *      Returns the configuration type, if found.
 *
 * \param [out] cfgsection
 *      Returns the configuration section (e.g. "[manual]") if found.
 *
 * \return
 *      Returns true if the output strings can be used.
 */

bool
multiparser::lookup_names
(
    const std::string & clioptname,         /* one or more characters   */
    std::string & configtype,
    std::string & configsection
)
{
    bool result = ! clioptname.empty() && clioptname[0] != '-';
    if (result)
    {
        std::string truename = clioptname;
        if (truename.length() == 1)
        {
            const auto nip = code_mappings().find(truename[0]);
            if (nip != code_mappings().end())
                truename = nip->second;
        }
        const auto nip = cli_mappings().find(truename);
        result = nip != cli_mappings().end();
        if (result)
        {
            auto d = nip->second;
            configtype = d.config_type;
            configsection = d.config_section;
        }
    }
    return result;
}

}           // namespace cli

/*
 * multiparser.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

