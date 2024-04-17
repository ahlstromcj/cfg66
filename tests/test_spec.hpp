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
 * \file          test_spec.hpp
 *
 *      A test-file for the rudimentary CLI parser class and C API.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-08-02
 * \updates       2023-08-03
 * \license       See above.
 *
 */

/**
 *  Test options.  Note that, in the create_option_list() call in the main()
 *  function, we specify that help and version options are automatically
 *  added. Also note that this list is a map of cli::options::container
 *  name/spec pairs.  Note that the following options are built-in and always
 *  accessible:
 *
 *      -   --help, -h
 *      -   --version, -v
 *      -   --verbose, -V
 *      -   --description,
 *      -   --option log[=name.log], -o ...
 *
 *  Note that option "loop-count" has no character code, as a test. Also note
 *  that this list is a vector of option::spec objects, and that std::vector
 *  has built-in support for std::initializer.
 */

static cfg::options::container s_test_options
{
    /*
     *   Name, Code,  Kind, Enabled,
     *   Default, Value, FromCli, Dirty,
     *   Description, Built-in
     */
    {
        {
            "alertable",
            {
                'a', "boolean", cfg::options::enabled,
                "false", "false", false, false,
                "If specified, the application is alertable.",
                false
            }
        },
        {
            "canned-code",
            {
                'c', "boolean", cfg::options::enabled,
                "true", "true", false, false,
                "If specified, the application employs canned code.",
                false
            }
        },
        {
            "dead-code",
            {
                'd', "boolean", cfg::options::enabled,
                "false", "false", false, false,
                "If specified, detect dead code.",
                false
            }
        },
        {
            "ethernet",
            {
                'e', "boolean", cfg::options::enabled,
                "false", "false", false, false,
                "If specified, use the ethernet adapter.",
                false
            }
        },
        {
            "fast-code",
            {
                'f', "boolean", cfg::options::enabled,
                "false", "false", false, false,
                "If specified, detect fast- code.",
                false
            }
        },
        {
            "find-me",
            {
                'F', "boolean", cfg::options::enabled,
                "false", "false", false, false,
                "If specified, detect 'find-me' on the CLI.",
                false
            }
        },
        {
            "disabled",
            {
                'D', "boolean", cfg::options::disabled,
                "false", "false", false, false,
                "If specified, disable the code.",
                false
            }
        },
        {
            "username",
            {
                'u', "string", cfg::options::enabled,
                "Joe Cool", "", false, false,
                "Specifies the user name (for permissions).",
                false
            }
        },
        {
            "loop-count",
            {
                cfg::options::code_null,  "integer", cfg::options::enabled,
                "0<=0<=99", "30", false, false,
                "Specifies the number of loops to make.", false
            }
        },
        {
            "flux",
            {
                'f', "floating", cfg::options::enabled,
                "0.0<=0.1<=50.0", "22.3", false, false,
                "Specifies the magnetic flux.",
                false
            }
        }
    }
};

/*
 * test_spec.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

