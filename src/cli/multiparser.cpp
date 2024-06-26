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
 * \updates       2024-06-26
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

namespace cli
{

/**
 *  Empty options set.
 */

static cfg::options s_dummy_options;

/**
 *  Constructors. The first creates an empty options container.
 */

multiparser::multiparser () :
    parser          (s_dummy_options),
    m_code_mappings (),
    m_cli_mappings  ()
{
    // no code
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
            std::string sectionname = isectspec.sec_name;

            /*
             * const std::pair<std::string, options::spec>
             */

            for (const auto & opt : isectspec.sec_optionlist)
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
                        if (! r.second)
                        {
#if defined PLATFORM_DEBUG
                            printf("Could not insert code '%c'\n", optcode);
#endif
                        }
                    }

                    duo d{configtype, sectionname};
                    auto p = std::make_pair(optname, d);
                    auto r = cli_mappings().insert(p);
                    if (! r.second)
                    {
#if defined PLATFORM_DEBUG
                        printf
                        (
                            "Could not insert option '%s'\n", optname.c_str()
                        );
#endif
                    }
                }
            }
        }
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
    std::string & cfgtype,
    std::string & cfgsection
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
            cfgtype = d.config_type;
            cfgsection = d.config_section;
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

