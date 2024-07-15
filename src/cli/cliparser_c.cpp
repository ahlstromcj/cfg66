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
 * \file          cliparser_c.cpp
 *
 *    This module provides C-compatible access to the cliparser class for
 *    parsing command line without using getopt.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2024-07-15
 * \license       See above.
 *
 */

#include <cstdio>

#include "cli/parser.hpp"               /* cfg::options, cli::parser        */
#include "cli/cliparser_c.h"            /* cliparser_c functions            */

/**
 *  Gets the singleton client parser for this application.
 */

static cli::parser &
parser (void)
{
    static cli::parser s_parser_object;
    return s_parser_object;
}

void
reset (void)
{
    parser().reset();
}

void
initialize (void)
{
    parser().initialize();
}

/**
 *  This "transformer" function takes an array of options_spec C structures
 *  with character pointers and converts them to cfg::options::spec structures,
 *  then adds the name to make a cfg::options::option pair, then fills the
 *  static cli::parser object with them.
 *
 *  Note that this function forces the option_value to be empty.
 */

bool
create_option_list (const options_spec * opts, int optcount)
{
    bool result = not_nullptr(opts);
    if (result)
    {
        parser().reset();     /* clears, adds help & version opts */
        for (int i = 0; i < optcount; ++i)
        {
            const options_spec * opts_ptr = &opts[i];
            cfg::options::kind okind =
                static_cast<cfg::options::kind>(opts_ptr->option_kind);

#if defined USER_CONSTRUCTOR_FOR_OPTIONS_SPEC
            cfg::options::spec os
            (
                opts_ptr->option_name, opts_ptr->option_code,
                okind, opts_ptr->option_cli_enabled,
                opts_ptr->option_default, opts_ptr->option_value,
                opts_ptr->option_modified, opts_ptr->option_desc
            );
#else
            /*
             *  modified and read_from_cli always false at first.
             */

            cfg::options::spec os;
            std::string name = opts_ptr->option_name;
            os.option_code = opts_ptr->option_code;
            os.option_kind = okind;
            os.option_cli_enabled = opts_ptr->option_cli_enabled;
            os.option_default = opts_ptr->option_default;
            os.option_value = opts_ptr->option_value;
            os.option_modified = opts_ptr->option_modified;
            os.option_desc = opts_ptr->option_desc;

            /*
             * This fails to compile!
             *
             *  std::make_pair<std::string, cfg::options::spec>(name, os);
             */

            cfg::options::option op = make_option(name, os);

#endif // defined USER_CONSTRUCTOR_FOR_OPTIONS_SPEC

            parser().add(op);
        }
    }
    return result;
}

/**
 *  Parses a command-line.  Assumes that create_option_list() has
 *  been called already.
 */

bool
parse_option_list (int argc, char * argv [])
{
    return parser().parse(argc, argv);
}

bool
change_value (const char * name, const char * value, bool fromcli)
{
    bool result = not_nullptr_2(name, value);
    if (result)
        result = parser().change_value(name, value, fromcli);

    return result;
}

/**
 *  A somewhat simplistic implementation.
 */

bool
value (const char * name, char * value, size_t szvalue)
{
    bool result = false;
    bool ok = not_nullptr_2(name, value) && szvalue >= 1;
    if (ok)
    {
        std::string v = parser().value(name);
        int count = std::snprintf(value, szvalue, "%s", v.c_str());
        result = size_t(count) <= szvalue;
    }
    return result;
}

/**
 *  This can be simple, since the help text will not change during run
 *  time.
 */

const char *
help_text (void)
{
    static std::string s_help_text = parser().help_text();
    return s_help_text.c_str();
}

/**
 *  This result can change during run-time, but is generally called
 *  once in a while by a single thread.
 */

const char *
debug_text (void)
{
    static std::string s_debug_text = parser().debug_text();
    if (parser().debug_text() != s_debug_text)
        s_debug_text = parser().debug_text();

    return s_debug_text.c_str();
}

bool
help_request (void)
{
    return parser().help_request();
}

bool
version_request (void)
{
    return parser().version_request();
}

/*
 * cliparser_c.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

