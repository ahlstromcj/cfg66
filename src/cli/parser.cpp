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
 * \file          parser.cpp
 *
 *      Provides a useful replacement for GNU getopt.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2024-07-04
 * \license       See above.
 *
 *      While this parser follows the basics of GNU getopt fairly well,
 *      it has some significant differences:
 *
 *      -   The setup of the command-line options adds more features to
 *          the options.
 *      -   The actual values of the options are saved in an std::map
 *          of options.  The user of the options does not need to handle
 *          each command-line option... it merely needs to access the
 *          map of options and their current values.
 *      -   Some common options (help, version, log-files) can be handled
 *          without adding code to the caller.
 *      -   Support for "overflow" options has been added.
 *      -   Callers can keep adding options to the set of options with
 *          no other additional code.
 */

#include <iostream>                     /* std::cout                        */
#include <sstream>                      /* std::ostringstream               */

#include "c_macros.h"                   /* not_nullptr()                    */
#include "cfg/appinfo.hpp"              /* cfg::appinfo structure           */
#include "cli/parser.hpp"               /* cli::parser class                */
#include "util/msgfunctions.hpp"        /* util::set_verbose() etc.         */
#include "util/strfunctions.hpp"        /* util::tokenize()                 */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cli
{

/**
 *  Constructors. The first creates an empty options container.
 */

parser::parser () :
    m_option_set            (),
    m_has_error             (false),
    m_error_msg             (),
    m_alternative           (false),
    m_help_request          (false),
    m_version_request       (false),
    m_verbose_request       (false),
    m_inspect_request       (false),
    m_investigate_request   (false),
    m_description_request   (false),
    m_use_log_file          (false),
    m_log_file              ()
{
    // no code needed
}

/**
 *  Constructs the parser from an options object.
 *
 *  Note that this class and its constructor supports only one configuration
 *  file/section. (For now).
 *
 *  Also note that this class's construction is compatible with the C
 *  interface in the cliplarser_c module.
 *
 * TODO:
 *
 *      Implement an std::initializer_list for options directly.
 *      This would be done in the more advanced cfg/parser module
 *      (NOT YET WRITTEN).
 *
 *      Also, we need to check for duplicate option codes and warn the
 *      user!
 */

parser::parser
(
    const cfg::options::container & specs,
    const std::string & filename,
    const std::string & sectionname,
    bool use_alternative_long_option
) :
    m_option_set            (specs, filename, sectionname),
    m_has_error             (false),
    m_error_msg             (),
    m_alternative           (use_alternative_long_option),
    m_help_request          (false),
    m_version_request       (false),
    m_verbose_request       (false),
    m_inspect_request       (false),
    m_investigate_request   (false),
    m_description_request   (false),
    m_use_log_file          (false),
    m_log_file              ()
{
    // no code needed
}

parser::~parser ()
{
    // no code
}

/**
 *  A straightforward command-line parser, for the most part.
 *
 *  One issue is the use of token_match() to set some flags. We really
 *  need for parse() to, at the end, to call options::boolean_value(),
 *  etc.
 *
 *  Note that this function will fail if something in the parser constructor
 *  failed.
 */

bool
parser::parse (int argc, char * argv [])
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

            if (token_match(token, "option"))
            {
                if (argv[i + 1][0] != '-')  /* needs a non-option argument  */
                {
                    std::string name;
                    std::string value;
                    (void) extract_value(name, value);
                    bool good = parse_o_option(name, value);
                    if (good)
                    {
                        if (name == "log")
                        {
                            use_log_file(true);
                            if (! name.empty())
                                log_file(name);
                        }
                        continue;
                    }
                    else
                    {
                        result = false;
                        break;
                    }
                }
                continue;
            }
            if (token[0] != '-')            /* probably a value, so skip    */
                continue;

            result = parse_value(argc, argv, i, token);
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
 *  Determine if an option name is present in the command-line.
 *
 * \param argc
 *      Command-line argument count from main().
 *
 * \param argv
 *      Command-line argument array from main().
 *
 * \param token
 *      The name of the current option. For convenience, if it doesn't
 *      have the option markers ("--" or "-"), one of them will be
 *      prepended. If the option "must exist", then that lookup is done
 *      without the hyphens.
 *
 * \param must_exist
 *      If true (the default), the option must exist in the option list.
 *
 * \return
 *      Returns true if the option was found on the command-line, and,
 *      if necessary, is also present in the options list.
 */

bool
parser::check_option
(
    int argc, char * argv [],
    const std::string & token,
    bool must_exist
) const
{
    bool result = false;
    bool ok = not_nullptr(argv) && ! token.empty();
    if (ok && argc > 1)
    {
        std::string stripped = token;
        std::string cltarget = token;
        if (token[0] != '-')
        {
            std::string pre = cltarget.size() > 1 ? "--" : "-" ;
            cltarget = pre + cltarget;
        }
        else
        {
            if (token.size() == 2)
                stripped = token.substr(1, 1);
            else
                stripped = token.substr(2, token.length() - 2);
        }
        for (int i = 1; i < argc; ++i)      /* token 0 might be app name    */
        {
            std::string arg = argv[i];
            if (arg == "--")                /* GNU end-of-options marker    */
                break;

            if (arg == "-")                 /* ill-formed token, bug out    */
                break;

            if (arg == cltarget)
            {
                if (must_exist)
                    result = option_set().option_exists(stripped);
                else
                    result = true;

                break;
            }
        }
    }
    return result;
}

/**
 *  This function assumes that the token is an option that was found
 *  on the command line. That is, it starts with a hyphen; it is not
 *  the value after the option name.
 *
 * \param argc
 *      Command-line argument count from main().
 *
 * \param argv
 *      Command-line argument array from main().
 *
 * \param index
 *      The command-line index of the current option; the next string
 *      after that one might be a value for the option.
 *
 * \param token
 *      The name of the current option. If it is an option (as opposed
 *      to a value) it will start with "--" or "-".
 *
 * \return
 *      Returns true if a value was obtained for the current option.
 *      The value for the option in the list is modified.
 */

bool
parser::parse_value
(
    int argc, char * argv [], int index,
    const std::string & token
)
{
    bool result = false;                    /* a pessimistic start          */
    std::string name;                       /* holds the option name/chars  */
    std::string value;                      /* value for a compound option  */
    bool boolvalue = true;                  /* used for boolean options     */
    size_t offset = 1;                      /* count the first hyphen       */
    std::string tk = token;
    std::string no = tk.substr(0, 5);
    if (no == "--no-")                      /* it's a falsified boolean     */
    {
        std::string partial = tk.substr(5); /* after "--no-"                */
        boolvalue = false;
        tk = "--";                          /* pretend there's no "--no-"   */
        tk += partial;                      /* get the parts after "--no-"  */
        offset = 2;                         /* count the two hyphens        */
    }
    else if (tk[1] == '-')
        ++offset;                           /* count the second hyphen      */

    name = tk.substr(offset, tk.length() - offset);
    if (offset == 1 && name.length() > 1)   /* an argument like "-xyz"      */
    {
        for (size_t i = 0; i < name.length(); ++i)
        {
            /*
             *  We set the flag for being set from the command-line.
             */

            std::string code(1, name[i]);               /* a bit tricky     */
            result = change_value(code, "true", true);  /* set the "bit"    */
            if (! result)
                break;
        }
    }
    else
    {
        /*
         *  Pass 'true' to indicate the value was set from the command-line.
         *  Note that boolean options are not compound options.
         */

        bool compound = extract_value(name, value);     /* side effects     */
        if (compound)
        {
            result = change_value(name, value, true);
        }
        else
        {
            if (is_boolean(name))
            {
                value = boolvalue ? "true" : "false" ;
            }
            else if (((index + 1) < argc))              /* more?            */
            {
                if (argv[index + 1][0] != '-')          /* can't be option  */
                    value = argv[index + 1];            /* too tricky       */

            }
            result = change_value(name, value, true);
        }
        if (! result)
        {
            m_has_error = true;
            if (option_set().has_error())   /* use the options' error msg   */
            {
                m_error_msg = option_set().error_msg();
            }
            else                            /* avoid replacing existing msg */
            {
                m_error_msg = "Option '";
                m_error_msg += name;
                if (! value.empty())
                {
                    m_error_msg += "=";
                    m_error_msg += value;
                }
                m_error_msg += "' not found";
            }
        }
    }
    return result;
}

/**
 *  Handles the "overflow" options, or "o options". These are additional
 *  options to be created when running out of non-white-space option
 *  characters.
 *
 *  The command line arguments for each "o option" are of the following forms:
 *
 *      -   --option name
 *      -   --option name=value
 *
 *  The first one is a boolean value, and the second one is a compound value.
 *
 *  These options are specified in the same was a regular options, except
 *  that the character code is null. Of course, all "o option" long names must
 *  be unique over the whole set of long option names in order for lookup to
 *  work.
 *
 *  Pass 'true' to indicate the value was set from the
 *  command-line.
 */

bool
parser::parse_o_option
(
    const std::string & name,  const std::string & value
)
{
    return change_value(name, value, true);
}

/**
 *  Checks if the token has the form "name separator value". The only
 *  separators supported are ":" or "=".
 *
 *  No need to handle quoted values, since the command shell will do that for
 *  us.
 *
 * \param [inout] token
 *      The argv[] token to be analyzed. If it is a compound token, such as
 *      "username=Pep" or "u:Pep", then it will be modified to drop the
 *      separator and the value.
 *
 * \param [out] value
 *      The destination for the value, if found.
 *
 * \return
 *      Returns true if a value was able to be extracted. If so, then
 *      this is a "compound" option, and not a boolean option, though
 *      booleans can also be compound.
 */

bool
parser::extract_value (std::string & token, std::string & value)
{
    std::size_t seppos = token.find_first_of(":=");         /* holy Algol!  */
    bool result = seppos != std::string::npos;
    if (result)
    {
        lib66::tokenization tokens = util::tokenize(token, ":=");
        result = tokens.size() == 2;        /* separator is not a token     */
        if (result)
        {
            token = tokens[0];              /* return the "name" portion    */
            value = tokens[1];              /* return the "value" portion   */
        }
    }
    return result;
}

/**
 *  Check for a match of the token to "-x", "--xyz", or, if the single-hyphen
 *  alternative is allowed, "-xyz". It assumes that the token has already been
 *  checked for being "--" or "-".
 *
 * \param token
 *      The option token. A valid token starts with at least one hyphen.
 *      Otherwise, it is likely to be a value. In the case, we cannot
 *      flag a match.
 *
 * \param opt
 *      This is the option long-name that the token should match.
 *
 * \param code
 *      This is the single-character code of the option that should
 *      match. The default is the null character (0), which means there
 *      is no option code.
 *
 * \return
 *      Returns true if the token is an option (starts with "-" or "--")
 *      and matchs the desired \a opt.
 */

bool
parser::token_match
(
    const std::string & token,
    const std::string & opt,
    char code
)
{
    bool result = ! token.empty() && token[0] == '-';
    if (result)
    {
        if (token.length() == 2)            /* check for a short option -x  */
        {
            if (code > ' ')                 /* theoretically allows "--"    */
                result = token[1] == code;
            else
                result = false;
        }
        else
        {
            bool singledash = token[1] != '-';
            if (singledash)
            {
                if (m_alternative)
                {
                    std::string tokpart = token.substr(1);
                    result = tokpart == opt;
                }
            }
            else
            {
                std::string tokpart = token.substr(2);
                result = tokpart == opt;
            }
        }
    }
    return result;
}

/**
 *  Provides default handling for informational options.
 *
 * \return
 *     Returns true if information-only was requested.
 */

bool
parser::show_information_only () const
{
    bool result = false;
    if (help_request())
    {
        /*
         * cli_help_text() is help_text() without items that make no sense
         * to change via the command-line.
         */

        std::cout << cli_help_text();
        result = true;
    }
    if (description_request())
    {
        std::cout << description_text();
        result = true;
    }
    if (version_request())
    {
        const std::string & ver = cfg::get_app_version();
        if (! ver.empty())
            std::cout << "Version " << cfg::get_app_version() << std::endl;
        else
            std::cout << cfg::get_app_version_text() << std::endl;

        result = true;
    }
    return result;
}

}           // namespace cli

/*
 * parser.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

