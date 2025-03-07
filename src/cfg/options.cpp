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
 * \file          options.cpp
 *
 *    Provides a useful replacement for GNU getopt.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2024-09-30
 * \license       See above.
 *
 *  The cli::options class provides a way to hold the state of command-line
 *  options. Also see the cfg::options class, which adds a file name and a
 *  section name and will hopefully add memento support for undo/redo.
 *
 * Option kinds:
 *
 *  Note that the string version of option_kind is somewhat free-form.
 *  It should be similar to the name of a C/C++ data type:
 *
 *      -   "boolean".
 *          "valuename = [ true | false ]"
 *          This one is necessary if the option is to be boolean.
 *          It also enables support for the prefix "no-" as part of the
 *          name to reverse the value.  The only useful option_value
 *          strings are "true" and "false".
 *      -   "integer".
 *          "valuename = [ integer ]"
 *          Use this one to indicate that an integer value
 *          (positive, negative, unsigned, short, etc.) is needed.
 *      -   "floating".
 *          "valuename = [ floating-point ]"
 *          Use this to indicate that a float or double value
 *          is needed.
 *      -   "string".
 *          "valuename = [ one-word-string | "string" ]"
 *          Use this to indicate that the option string is
 *          indeed just a string.
 *      -   "overflow".
 *          "valuename = [ one-word-string | "string" ]"
 *          This string indicates the built-in support
 *          for "o options". These are options created when running
 *          out of single-character options, and are included for
 *          backward compatibility with Seq66. The format is -o or
 *          --option name[=value]. Supports log=filename by default.
 *          Otherwise the same as the "string" option time.
 *
 * Special cases:
 *
 *      "filename": The string parameter is a file-name string plus an
 *      active flag..
 *
 *          Configuration file:
 *
 *              [xyz-file]
 *              active = [true | false ]
 *              file = "filename.zyx"         # 'xyz-file = "filename"'
 *              base-directory = "path" | ""  # optional 'xyz-dir = "directory"'
 *
 *          CLI:
 *
 *              --xyz-file="filename", which can be a full path if desired.
 *
 *              If specified, "active = true", "file = filename".
 *              "base-directory" not supported on the CLI.
 *
 *      "list": A list of items with names of the form "xyz-N".
 *
 *          Configuration file:
 *
 *              [xyz-list]
 *              count = N
 *              xyz-0 = "item specification"
 *              xyz-1 = ...
 *
 *          Alternate:
 *
 *              2                       # number of ports
 *              0  1   "[0] 0:1 system:ALSA Announce"
 *              1  0   "[1] 14:0 Midi Through Port-0"
 *
 *          CLI:
 *
 *              --xyz-N="itemspec", where N is 0 on up.
 *
 *          Specific "xyz" values to be supported:
 *
 *              [in-port-list]          --port-0="enabled" or "disabled"
 *              count = N
 *              in-port-0 = "port specification" (enabled status and name)
 *
 *              [in-portmap-list]
 *              count = N
 *              in-portmap-0 = "portmap specification" (nickname)
 *
 *      "recents": A simplie list of filenames and some other flags
 *
 *          Configuration file:
 *
 *              full-paths = [ true | false ]
 *              load-most-recent = [ true | false ]
 *              count = [ integer ]
 *              "full file specification"
 *
 *          CLI: Never implemented, ever.
 *
 * Special value formats?:
 *
 *      -   "control specification"
 *          loop number, key name, 3x5 stanza
 *      -   "control out specification"
 *          loop number, 4x3 stanzas
 *      -   "mute out specification"
 *          mute number, 3x3 stanzas
 *      -   "automatic out specification"
 *          enabled, 3x3 stanzas
 *      -   "mutes"
 *          group number, 4x8 stanza or HEX
 *      -   "playlist"
 *      -   "palette"
 *      -   "metro"
 *      -   "keymap"
 *      -   "ui"
 *
 *  The following option kinds are generally not used for command-line
 *  options (though allowed), but for arrays of values.
 *  The old bare count values used for arrays are replaced by the option
 *  "count = integer".  If encountered on the command line, ignored?
 *
 *      -   Anything else can be added to support odd concepts such
 *          as a coordinate "(x,y)", a size "wxh", etc. App-specific.
 */

#include <algorithm>                    /* std::sort()                      */
#include <cmath>                        /* std::fabs(), std::fabsf()        */
#include <iomanip>                      /* std::setw()                      */
#include <limits>                       /* std::numeric_limits<>            */
#include <sstream>                      /* std::ostringstream               */

#include "c_macros.h"                   /* not_nullptr()                    */
#include "cfg/appinfo.hpp"              /* cfg::level_color()               */
#include "cfg/options.hpp"              /* cfg::options class               */
#include "util/strfunctions.hpp"        /* util::string_to_int() etc.       */

#if defined USE_COLOR_CLI_HELP_TEXT
#include "cfg/appinfo.hpp"              /* cfg::level_color()               */
#endif

namespace cfg
{

#if defined USER_CONSTRUCTOR_FOR_OPTIONS_SPEC

/*--------------------------------------------------------------------------
 * options::spec constructors
 *--------------------------------------------------------------------------*/

options::spec::spec () :
    option_code         (0),
    option_kind         (options::kind::dummy),
    option_cli_enabled  (false),
    option_default      (),
    option_value        (),
    option_modified     (false),
    option_desc         (),
    option_global       (false)
{
    // No other code
}

options::spec::spec
(
    char code, options::kind k,
    bool enabled, const char * defalt, const char * value,
    bool modified, const char * desc
) :
    option_code         (code),
    option_kind         (k),
    option_cli_enabled  (enabled),
    option_default      (defalt),
    option_value        (value),
    option_modified     (modified),
    option_desc         (desc),
    option_global       (false)
{
    // No other code
}

#endif // defined USER_CONSTRUCTOR_FOR_OPTIONS_SPEC

/*--------------------------------------------------------------------------
 * options
 *--------------------------------------------------------------------------*/

options::options (bool loadglobal) :
    m_code_list         (),
    m_has_error         (false),
    m_error_msg         (),
    m_source_file       (),
    m_source_section    (),
    m_option_pairs      ()
{
    if (loadglobal)
    {
        if (add(global_options()))      /* add the default/global options   */
            initialize();
    }
}

options::options
(
    const container & specs,
    const std::string & file,
    const std::string & section
) :
    m_code_list         (),
    m_has_error         (false),
    m_error_msg         (),
    m_source_file       (file),
    m_source_section    (section),
    m_option_pairs      (specs)
{
    if (file.empty() && section.empty())
    {
        if (add(global_options()))     /* add the default/stock options    */
            initialize();
    }
    else
        initialize();
}

/**
 *  Empties the options container completely. It then (optionally) adds
 *  in stock help and version information. This function must be called
 *  if one wants to support the default options.
 *
 * \param add_stock
 *      If true, add the default options.
 */

void
options::reset ()
{
    m_has_error = false;
    m_error_msg.clear();
    initialize();
}

/**
 *  Keeps the current set of options, but clears the dirtiness and resets
 *  the default values of the options.
 */

void
options::initialize ()
{
    init_container(option_pairs());
}

/**
 *  Initializes the specified container. It does the following for each
 *  options::spec in the container:
 *
 *      -   Sets the value of the option to its default value.
 *      -   Unsets the "read from CLI" flag.
 *      -   Unsets the "modified" flag.
 *
 *  This is a static member function.
 */

void
options::init_container (container & pairs)
{
    for (auto & specs : pairs)
    {
        spec & sp = specs.second;
        sp.option_value = sp.option_default;
        sp.option_read_from_cli = sp.option_modified = false;
    }
}

/**
 *  Duplicate option names will be rejected (i.e. not be inserted).
 *  Remember that an "option" is a std::pair<std::string, spec>.
 */

bool
options::add (const option & op)
{
    auto r = option_pairs().insert(op);
    return r.second;
}

/**
 *  Adds another options container to this one.  Watch out for options with
 *  the same name!
 *
 * \return
 *      Returns true if the element was inserted.  If not, an attempt
 *      was made to insert an alread-existing key in one of the entries.
 *      It is probably safe to ignore this code, but at least show a warning.
 */

bool
options::add (const container & optlist)
{
    bool result = ! optlist.empty();
    if (result)
    {
        for (const auto & sp : optlist)
        {
            bool ok = add(sp);
            if (! ok)
            {
                result = false;
                break;
            }
        }
        if (result)
            result = verify();
    }
    return result;
}

/**
 *  Check the list for duplicates.  Currently checks only for duplicate
 *  single-letter option codes, not for full option names.
 *
 *  In C++17, we could use "for (const auto & [ key, value ] : option_specs())".
 */

bool
options::verify () const
{
    bool result = true;
    m_code_list.clear();
    for (const auto & op : option_pairs())
    {
        char c = char(op.second.option_code);
        if (c > 0)
        {
            std::string::size_type pos = m_code_list.find_first_of(c);
            if (pos == std::string::npos)
            {
                m_code_list += c;
            }
            else
            {
                m_has_error = true;
                m_error_msg = "Option code '";
                m_error_msg += c;
                m_error_msg += "' already added";
                result = false;

                /*
                 * Let's just keep going.
                 *
                 * break;
                 */
            }
        }
    }
    if (result)
        std::sort(m_code_list.begin(), m_code_list.end());

    return result;
}

/**
 *  Checks an integer or float against a range, and sets an error messag e
 *  if necessary.
 */

bool
options::check_range
(
    const std::string & name,
    float value, float minimum, float maximum
) const
{
    bool result = value >= minimum && value <= maximum;
    if (! result)
    {
        bool adding = m_has_error;
        std::string msg = "Option '";
        msg += name;
        msg += "=";
        msg += util::double_to_string(value);
        msg += "' outside of range ";
        msg += util::double_to_string(minimum);
        msg += " to ";
        msg += util::double_to_string(maximum);
        m_has_error = true;
        if (adding)
        {
            m_error_msg += "; ";
            m_error_msg += msg;
        }
        else
            m_error_msg = msg;
    }
    return result;
}

/**
 *  Sets the option's value.
 *
 * \param name
 *      The long name of the option.  We will always externally look for the
 *      long name, for the sake of the human reader/programmer. Note that
 *      each inisection has its own options object, so that searches look
 *      only in that object; thus, options in different inisections can safely
 *      have the same name. Note that the name must not start with "--"; strip
 *      it off.
 *
 * \param value
 *      The value to be assigned.  This can be empty.  For boolean options,
 *      "true" sets the boolean, and any other value falsifies it. Since this
 *      is a string, when applied to numbers the modification test can fail
 *      even if the numbers are the same (e.g. "0" vs "0.0" for float value).
 *
 * \return
 *      Returns true if the option was found and it's value actually changed.
 *      However, the option's modify flag is NOT set. This allows for reading
 *      option values from the command line or from a configuration file
 *      without causing options to be saved at application exit.
 */

bool
options::set_value
(
    const std::string & name,
    const std::string & value
)
{
    bool result = ! name.empty();
    if (result)
    {
        auto opt = find_match(name);
        result = option_exists(opt);
        if (result)
        {
            spec & ncop = const_cast<spec &>(opt->second);
            result = value != ncop.option_value;
            if (result)
            {
                if (option_is_boolean(ncop))
                {
                    std::string newvalue = "true";
                    if (value != "true")
                        newvalue = "false";

                    ncop.option_value = newvalue;
                }
                else if (option_is_int(ncop))
                {
                    int minimum;
                    int maximum;
                    int defalt = integer_value_range(name, minimum, maximum);
                    if (value.empty())
                    {
                        ncop.option_value = std::to_string(defalt);
                    }
                    else
                    {
                        int iv = util::string_to_int(value);
                        result = check_range
                        (
                            name, float(iv), float(minimum), float(maximum)
                        );
                        if (result)
                            ncop.option_value = value;
                    }
                }
                else if (option_is_float(ncop))
                {
                    float minimum;
                    float maximum;
                    float defalt = floating_value_range(name, minimum, maximum);
                    if (value.empty())
                    {
                        ncop.option_value = std::to_string(defalt);
                    }
                    else
                    {
                        float iv = float(util::string_to_double(value));
                        result = check_range(name, iv, minimum, maximum);
                        if (result)
                            ncop.option_value = value;
                    }
                }
                else
                    ncop.option_value = value;
            }
        }
    }
    return result;
}

/**
 *  Changes the option's value, if found.
 *
 * \param name
 *      The long name of the option.  We will always externally look for the
 *      long name, for the sake of the human reader/programmer. Note that
 *      the name must not start with "--"; strip it off.
 *
 * \param value
 *      The value to be assigned.  This can be empty.  For boolean options,
 *      "true" sets the boolean, and any other value falsifies it.
 *
 * \param fromcli
 *      If true, the change was made via the command-line parser.
 *
 * \return
 *      Returns true if the name was not empty and was found.
 *      Should we also check for actual modification?
 */

bool
options::change_value
(
    const std::string & name,
    const std::string & value,
    bool fromcli
)
{
    bool result = set_value(name, value);
    if (result)
    {
        auto opt = find_match(name);                /* always succeeds here */
        spec & ncop = const_cast<spec &>(opt->second);
        ncop.option_modified = true;
        if (fromcli)
            ncop.option_read_from_cli = true;
    }
    return result;
}

bool
options::modified () const
{
    bool result = false;
    for (const auto & op : option_pairs())
    {
        if (op.second.option_modified)
        {
            result = true;
            break;
        }
    }
    return result;
}

/**
 *  If this function returns true, then the option was already obtained from the
 *  command-line, and should not be overwritten (except from an edit within
 *  the application).
 */

bool
options::was_read_from_cli (const std::string & name) const
{
    auto opt = find_match(name);
    bool result = option_exists(opt);
    if (result)
        result = opt->second.option_read_from_cli;

    return result;
}

/**
 *  This function is useful when getting an option from the command-line.
 *  If that was done, then do not read the option from the file.
 */

void
options::set_read_from_cli (const std::string & name, bool flag)
{
    auto opt = find_match(name);
    if (option_exists(opt))
    {
        spec & ncop = const_cast<spec &>(opt->second);
        ncop.option_read_from_cli = flag;
        ncop.option_modified = false;
    }
}

/**
 *  This function is useful when saving changes.
 */

void
options::unmodify (const std::string & name)
{
    auto opt = find_match(name);
    if (option_exists(opt))
    {
        spec & ncop = const_cast<spec &>(opt->second);
        ncop.option_modified = false;
    }
}

void
options::unmodify_all ()
{
    for (auto & op : option_pairs())
        op.second.option_modified = false;
}

/**
 *  Provides a way to get the long name from a character option.
 *  This is slower than using the key value for lookup.
 *
 *  \private
 */

std::string
options::long_name (char code) const
{
    std::string result;
    for (const auto & op : option_pairs())
    {
        if (code == op.second.option_code)
        {
            result = op.first;
            break;
        }
    }
    return result;
}

/**
 *  Given a valid single-character code, this function looks up the long name
 *  of the option for that character code.
 *
 * \param code
 *      This value should be a single-character string. For flexibility, a
 *      long name is permitted.
 *
 * \return
 *      If a single character, and the lookup fails, then an empty string is
 *      returned. Otherwise the code parameter is returned unaltered.
 */

std::string
options::long_name (const std::string & code) const
{
    std::string result;
    if (code.length() == 1)
    {
        char uc = code[0];
        result = long_name(uc);
    }
    else
        result = code;                  /* will be empty if code.empty()    */

    return result;
}

/**
 *  Finders for callers that want the whole specification of the option,
 *  and don't want to deal with iterators.
 *
 *  Note that the "opt" value found is an std::pair<>.
 */

const options::spec &
options::find_spec (const std::string & name) const
{
    static spec s_inactive_spec;            /* do not load global options   */
    if (! name.empty())
    {
        const auto opt = find_match(name);
        if (option_exists(opt))
            return opt->second;
    }
    return s_inactive_spec;
}

/**
 *  Note: if we move this project to C++17 the following line could be used:
 *
 *      const_cast<spec &>(std::as_const(*this).find_spec(name));
 */

options::spec &
options::find_spec (const std::string & name)
{
    return const_cast<spec &>(static_cast<const options &>(*this).find_spec(name));
}

/**
 *  Provides a way to get an options spec from the long name.  If the
 *  name is a single character long, it is treated as a code name and
 *  is used to look up the long name.
 *
 *  Unfortunately, this can cause two linear lookups.  Would that be
 *  noticeable with a list of 200 options?  More likely an application
 *  won't have more than a couple dozen.
 *
 *  \private
 *
 * \param name
 *      The long name or the single-character code name to look up.
 *
 * \return
 *      Returns the matched reference if found, otherwise a dummy reference
 *      is returned.
 */

options::container::const_iterator
options::find_match (const std::string & name) const
{
    std::string longname = long_name(name);

#if defined PLATFORM_DEBUG
    if (option_pairs().empty())
    {
        printf
        (
            "No options in the option container (context: '%s')\n",
            name.c_str()
        );
    }
#endif

    return longname.empty() ?
        option_pairs().end() : option_pairs().find(longname) ;
}

/**
 *  Simply determines if an option (long name or code name) exists in the
 *  top set.
 */

bool
options::option_exists (const std::string & name) const
{
    std::string longname = long_name(name);
    bool result = ! longname.empty();
    if (result)
    {
        container::const_iterator valueptr = option_pairs().find(longname);
        result = valueptr != option_pairs().end();
    }
    return result;
}

bool
options::option_is_boolean (const std::string & name) const
{
    bool result = false;
    auto opt = find_match(name);
    if (option_exists(opt))
        result = opt->second.option_kind == kind::boolean;

    return result;
}

/**
 *  Similar to description(), but adds the default value as well.
 *  Does not include the end-of-line character.  There are two overloads, one
 *  for option::spec and one that searches for the option::spec by name.
 */

std::string
options::help_line (const option & opt) const
{
    std::string result;
    if (opt.second.option_cli_enabled)
    {
        const spec & op = opt.second;
        std::ostringstream ost;
        char code = op.option_code;
        std::string name = opt.first;
        int count = 18;
        if (code == 0)
        {
            code = ' ';
            count = 22;
        }
        else
            ost << " -" << code << ",";

        if (op.option_kind != kind::boolean)    /* i.e. not a flag option   */
        {
            if (op.option_kind == kind::overflow)
                name += " x[=]v";
            else
                name += "=v";
        }
        ost << " --" << std::setw(count) << std::left << name;

        std::string desc = op.option_desc;
        if (code != 'h' && code != 'v')
        {
            if (op.option_value != op.option_default)
                desc += " [" + op.option_value + "]";
            else
                desc += " [" + op.option_default + "]";
        }
        desc = util::hanging_word_wrap
        (
            desc, options::hanging_width, options::terminal_width
        );
        ost << std::left << desc;
        result = ost.str();
    }
    return result;
}

/**
 *  The same as help_line(), but adds color to the options themselves.
 *  The result should look similar to a man page.
 *
 *  level_color(0) as defined in appinfo is the normal foreground/text terminal
 *  color.
 *
 *  level_color(2) as defined in appinfo is "yellow".
 *
 *  level_color(4) as defined in appinfo is "blue".
 */

std::string
options::color_help_line (const option & opt) const
{
    std::string result;
    if (opt.second.option_cli_enabled)
    {
        const spec & op = opt.second;
        std::ostringstream ost;
        char code = op.option_code;
        std::string name = opt.first;
        int count = 18;
        if (code == 0)
        {
            code = ' ';
            count = 22;
        }
        else
        {
            ost
                << " " << level_color(4) << "-" << code
                << level_color(0) << ","
                ;
        }

        if (op.option_kind != kind::boolean)    /* i.e. not a flag option   */
        {
            if (op.option_kind == kind::overflow)
                name += " x[=]v";
            else
                name += "=v";
        }
        ost
            << " " << level_color(4) << "--"
            << std::setw(count) << std::left << name
            << level_color(0)
            ;

        std::string desc = op.option_desc;
        if (code != 'h' && code != 'v')
        {
            desc += " [";
            desc += level_color(2);
            desc += op.option_default;
            desc += level_color(0);
            desc += "]";
        }
        desc = util::hanging_word_wrap
        (
            desc, options::hanging_width, options::terminal_width
        );
        ost << std::left << desc;
        result = ost.str();
    }
    return result;
}

std::string
options::help_line (const std::string & name) const
{
    std::string result;
    auto opt = find_match(name);
    if (option_exists(opt))
        result = help_line(*opt);

    return result;
}

/**
 *  We need a help_text() function that emits only cli-enabled options.
 *  Here it is.
 */

std::string
options::cli_help_text () const
{
    std::string result;
    if (! option_pairs().empty())
    {
        bool finish = false;                    /* at least 1 cli-enabled?  */
        for (const auto & op : option_pairs())
        {
            if (op.second.option_cli_enabled)
            {
                bool showcolor = is_a_tty();
                std::string h = showcolor ?
                    color_help_line(op) : help_line(op) ;

                if (! h.empty())
                {
                    result += h;
                    result += "\n";
                    finish = true;
                }
            }
        }
        if (finish)
            result += "\n";
    }
    return result;
}

/**
 *  Returns all of the help lines.
 */

std::string
options::help_text () const
{
    std::string result;
    if (option_pairs().size() > 0)
    {
        for (const auto & op : option_pairs())
        {
            std::string h = help_line(op);
            if (! h.empty())
            {
                result += h;
                result += "\n";
            }
        }
        result += "\n";
    }
    return result;
}

/**
 *  Assembles a line of one of the forms:
 *
 *      value-name = value      # description
 *      value-name = "value"    # description (if there is room < 80 columns)
 *      wraparound/multiline string
 *
 * \param op
 *      The option specification to use to construct the line.
 */

std::string
options::setting_line (const std::string & name) const
{
    std::string result;
    auto opt = find_match(name);
    if (option_exists(opt))
        result = setting_line(*opt);

    return result;
}

/**
 *  This function creates a line describing the current status of the
 *  setting.
 *
 *  We have the following kinds of setting values:
 *
 *      -   Simple. Single-line (i.e. short) values, which generally do not
 *          have a newline, but we want to handle that properly.
 *          -   value-name = value      # description
 *          -   value-name = "value"    # description (if < 80 columns)
 *      -   Long. Values long enough to require multiple lines, with or
 *          without newlines (it's the programmer's decision). We don't
 *          yet have a strategy for these.
 *      -   List value. These include a count and then a list of values
 *          without comments.
 *      -   Section value. These are generated already formatted.
 *
 *  Empty or lines that would be too long to hold the description will not
 *  provide the description.
 */

std::string
options::setting_line (const option & opt) const
{
    std::string result;
    const spec & op = opt.second;
    if (option_exists(op))
    {
        if (option_is_section(op))
        {
            result = op.option_value;       /* just dump the whole string   */
        }
        else
        {
            size_t width = options::field_width;        /* 40 characters    */
            std::string value = opt.first;  /* the key (the option's name   */
            value += " = ";
            if (option_is_quotable(op))
                value += "\"";

            value += op.option_value;
            if (option_is_quotable(op))
                value += "\"";

            std::string desc;
            bool show_description = ! op.option_desc.empty();
            if (show_description)
            {
                size_t vlen = value.length();
                size_t dlen = op.option_desc.length();
                show_description = vlen <= width && dlen <= width;
                if (show_description)
                {
                    desc = "# " + op.option_desc;
                    int newlinecount = util::count_character(desc);
                    if (newlinecount == 0)
                        desc += "\n";
                }
            }
            if (! show_description)
                value += "\n";

            std::ostringstream ost;
            if (show_description)
            {
                ost << std::setw(width) << std::left << value;
                ost << desc;
            }
            else
                ost << value;

            result = ost.str();
        }
    }
    return result;
}

/**
 *  For debugging or reference only.  Shows only the long name,
 *  the default value, the current value, and the "modified" flag.
 */

std::string
options::debug_line (const option & opt) const
{
    std::string result;
    const spec & op = opt.second;
    if (option_exists(op))
    {
        std::string value;
        std::ostringstream ost;
        if (op.option_value.length() > 18)
        {
            std::string sub = op.option_value.substr(0, 14);
            value = "\"" + sub;
            value += "...\"";
        }
        else
            value = "\"" + op.option_value + "\"";

        ost
            << "   "
            << std::setw(20) << std::left << opt.first << " = "
            << std::setw(20) << std::left << value
            ;

        if (op.option_code != 'h' && op.option_code != 'v')
        {
            if (op.option_default.length() > 18)
            {
                value = "[" + op.option_default.substr(0, 14);
                value += "...]";
            }
            else
                value = "[" + op.option_default + "]";

            ost << std::setw(20) << std::left << value;
            if (op.option_cli_enabled)
                ost << " CLI";

            if (op.option_modified)
                ost << " *";
        }
        result = ost.str();
    }
    return result;
}

/**
 *  Returns all of the status lines. Useful in verbose mode.
 */

std::string
options::debug_text (bool show_builtins) const
{
    std::string result;
    if (! option_pairs().empty())
    {
        for (const auto & opt : option_pairs())
        {
            if (show_builtins || ! opt.second.option_global)
            {
                std::string s = debug_line(opt);
                if (! s.empty())
                {
                    result += s;
                    result += "\n";
                }
            }
        }
    }
    else
        result = "Empty\n";

    return result;
}

/**
 *  Looks up the long name in this options object.  If found, then the
 *  corresponding option description is returned.  Otherwise, an empty string
 *  is returned.
 *
 * \param name
 *      Provides the long name for the option.  However, the code name for the
 *      option can also be used, at the expense of an extra lookup.
 *
 * \return
 *      Returns an options::spec::option_desc if the lookup succeeds.
 */

std::string
options::description (const std::string & name) const
{
    std::string result;
    auto opt = find_match(name);
    if (option_exists(opt))
    {
        result = opt->first;
        result += ": ";
        result = opt->second.option_desc;
    }
    return result;
}

/**
 *  Returns all of the help lines, in essence. Compare to the description()
 *  overload above.
 */

std::string
options::description () const
{
    std::string result;
    for (const auto & op : option_pairs())
    {
        std::string h = op.second.option_desc;
        if (h.empty())
            h = "No description!";

        result += op.first;
        result += ": ";
        result += h;
        result += "\n";
    }
    result += "\n";
    return result;
}

/**
 *  Shows everything about an option.
 */

std::string
options::long_description (const option & opt) const
{
    std::string result;
    const spec & op = opt.second;
    std::string value = "\"" + op.option_value + "\"";
    std::string kindstr = kind_to_string(op.option_kind);
    std::ostringstream ost;
    ost
        << std::setw(16) << std::left << opt.first
        << "(" << op.option_code << ")"
        << " = "
        << std::setw(20) << std::left << value
        << "[" << std::setw(10) << std::left << kindstr << "]"
        ;

    if (op.option_code != 'h' && op.option_code != 'v')
    {
        if (op.option_modified)
        {
            ost << " modified";
            if (op.option_read_from_cli)
                ost << " (on CLI)";
        }
    }
    result = ost.str();
    if (! op.option_cli_enabled)
        result += " non-CLI";

    return result;
}

/**
 *  Similar to value(), but returns the default value if the long name is
 *  found.
 */

std::string
options::default_value (const std::string & name) const
{
    std::string result;
    auto opt = find_match(name);
    if (option_exists(opt))
        result = opt->second.option_default;

    return result;
}

/**
 *  Looks up the long name in this options object.  If found, then
 *  the corresponding option value is returned.  Otherwise, an empty
 *  string is returned.
 *
 * \param name
 *      Provides the long name for the option.  However, the code name for the
 *      option can also be used, at the expense of an extra lookup.
 *
 * \return
 *      Returns an options::spec::option_value if the lookup succeeds.
 */

std::string
options::value (const std::string & name) const
{
    std::string result;
    auto opt = find_match(name);
    if (option_exists(opt))
        result = opt->second.option_value;

    return result;
}

/**
 *  Note that this function can change any kind of value, since the
 *  values of all type are stored as strings.
 *
 *  This function is not used when setting values from the command line.
 *  It is for the app itself to use.
 */

void
options::value (const std::string & name, const std::string & value)
{
    bool ok = change_value(name, value);
    if (! ok)
    {
#if defined PLATFORM_DEBUG
        printf("Could not change option '%s'\n", name.c_str());
#endif
    }
}

/**
 *  Looks up the value string, assuming this option is boolean, and
 *  tests its option-value against the English string "true".
 */

bool
options::boolean_value (const std::string & name) const
{
    return value(name) == "true";
}

void
options::boolean_value (const std::string & name, bool value)
{
    std::string bvalue = value ? "true" : "false" ;
    bool ok = change_value(name, bvalue);
    if (! ok)
    {
#if defined PLATFORM_DEBUG
        printf("Could not change option '%s'\n", name.c_str());
#endif
    }
}

/**
 *  Looks up the value string, assuming this option is integer, and
 *  return its option-value string as converted to an integer.
 */

int
options::integer_value (const std::string & name) const
{
    return util::string_to_int(value(name));
}

void
options::integer_value (const std::string & name, int value)
{
    std::string ivalue = util::int_to_string(value);
    bool ok = change_value(name, ivalue);
    if (! ok)
    {
#if defined PLATFORM_DEBUG
        printf("Could not change option '%s'\n", name.c_str());
#endif
    }
}

/**
 *  Looks up the value string, assuming this option is floating, and
 *  return its option-value string as converted to a float.
 */

float
options::floating_value (const std::string & name) const
{
    return util::string_to_float(value(name));
}

void
options::floating_value (const std::string & name, float value)
{
    std::string dvalue = util::double_to_string(value);
    bool ok = change_value(name, dvalue);
    if (! ok)
    {
#if defined PLATFORM_DEBUG
        printf("Could not change option '%s'\n", name.c_str());
#endif
    }
}

/**
 *  Converts a string to one or three tokens. A string with just one
 *  number is returned as one token.  Otherwise, if a "<" is found,
 *  then it is used to create as many tokens (hopefully 3 at maximum)
 *  as found. If an "=" is found, it will indicate a closed endpoint.
 *  Examples"
 *
 *      -   "99"            Returns one token, "99".
 *      -   "-100<0<100     Returns "-100", "0", and "100". The range
 *                          intended is -99 to 99 i.e. "(-100,100)".
 *      -   "-100<=0<=100"  Returns "-100", "=0", and "=100". The range
 *                          intended is -100 to 100 i.e. "[-100,100]".
 *
 *  Spaces are stripped before processing.
 */

static lib66::tokenization
range_tokens (const std::string & rangestring)
{
    lib66::tokenization result;
    if (! rangestring.empty())
    {
        std::string cleaned;
        for (auto c : rangestring)
        {
            if (! std::isspace(c))
                cleaned += c;
        }
        if (! cleaned.empty())
            result = util::tokenize(rangestring, "<");
    }
    return result;
}

/**
 *  Looks up the potential range string from option_default, which will have
 *  the following values:
 *
 *      -   Some non-numeric string. Will return 0.
 *      -   A single number. Will return that number as an integer.
 *      -   A min-default-max string of the form "min<def<max" (e.g.
 *          "-5<0<5". This format must be strictly followed. This notation
 *          corresponds to the numerical interval notation "[-5,5)", but
 *          is easy to tokenize. The actual range for this example would be
 *          from -5 to +4.
 *
 * \param name
 *      The name of the option to check. Assumed to describe an integer.
 *      This is on the caller.
 *
 * \param [out] minimum
 *      The minimum value specified in the spec.option_default string. If
 *      there is no such value (e.g. the value is a single number), then
 *      std::numeric_limits<int>::min() is used.
 *
 * \param [out] maximum
 *      The maximum value specified in the spec.option_default string. If
 *      there is no such value (e.g. the value is a single number), then
 *      std::numeric_limits<int>::max() is used.
 *
 * \return
 *      Returns the default value. This is the center value in "x<d<y" or
 *      the single value in "d".
 */

int
options::integer_value_range
(
    const std::string & name,
    int & minimum,
    int & maximum
) const
{
    std::string defstring = default_value(name);
    lib66::tokenization range = range_tokens(defstring);
    int result = -99999;
    if (range.size() == 3)
    {
        bool equals_min = range[1][0] == '=';
        bool equals_max = range[2][0] == '=';
        minimum = util::string_to_int(range[0]);
        if (equals_min)
        {
            result = util::string_to_int(range[1].substr(1));   /* ignore = */
        }
        else
        {
            ++minimum;
            result = util::string_to_int(range[1]);
        }
        if (equals_max)
            maximum = util::string_to_int(range[2].substr(1));
        else
            maximum = util::string_to_int(range[2]) - 1;
    }
    else if (range.size() == 1)
    {
        minimum = std::numeric_limits<int>::min();
        result = util::string_to_int(defstring);
        maximum = std::numeric_limits<int>::max();
    }
    else
    {
        minimum = std::numeric_limits<int>::min();
        maximum = std::numeric_limits<int>::max();
    }
    return result;
}

/**
 *  Similar to integer_value_range, but for floating-point values.
 *  This one is a little tricky. The string "-5.0<0<5.0" would
 *  cover of range of about +4.9999 to +4.9999.
 */

float
options::floating_value_range
(
    const std::string & name,
    float & minimum,
    float & maximum
) const
{
    std::string defstring = default_value(name);
    lib66::tokenization range = range_tokens(defstring);
    float result = -99999.0;
    if (range.size() == 3)
    {
        bool equals_min = range[1][0] == '=';
        bool equals_max = range[2][0] == '=';
        float e = std::numeric_limits<float>::epsilon();
        minimum = util::string_to_float(range[0]);
        if (equals_min)
        {
            result = util::string_to_float(range[1].substr(1));
        }
        else
        {
            minimum += e;
            result = util::string_to_float(range[1]);
        }
        if (equals_max)
            maximum = util::string_to_float(range[2].substr(1));
        else
            maximum = util::string_to_float(range[2]) - e;
    }
    else if (range.size() == 1)
    {
        minimum = std::numeric_limits<float>::min();
        result = util::string_to_float(defstring);
        maximum = std::numeric_limits<float>::max();
    }
    else
    {
        minimum = std::numeric_limits<float>::min();
        maximum = std::numeric_limits<float>::max();
    }
    return result;
}

/*--------------------------------------------------------------------------
 * static options functions
 *--------------------------------------------------------------------------*/

std::string
options::kind_to_string (kind k)
{
    std::string result;
    switch (k)
    {
    case kind::boolean:   result = "boolean";     break;
    case kind::filename:  result = "filename";    break;
    case kind::integer:   result = "integer";     break;
    case kind::intpair:   result = "intpair";     break;
    case kind::floating:  result = "floating";    break;
    case kind::floatpair: result = "floatpair";   break;
    case kind::list:      result = "list";        break;
    case kind::recents:   result = "recents";     break;
    case kind::overflow:  result = "overflow";    break;
    case kind::section:   result = "section";     break;
    case kind::string:    result = "string";      break;
    case kind::dummy:     result = "dummy";       break;
    }
    return result;
}

options::kind
options::string_to_kind (const std::string & s)
{
    options::kind result = kind::boolean;
    if (s == "filename")
        result = kind::filename;
    else if (s == "integer")
        result = kind::integer;
    else if (s == "intpair")
        result = kind::intpair;
    else if (s == "floating")
        result = kind::floating;
    else if (s == "floatpair")
        result = kind::floatpair;
    else if (s == "list")
        result = kind::list;
    else if (s == "recents")
        result = kind::recents;
    else if (s == "overflow")
        result = kind::overflow;
    else if (s == "section")
        result = kind::section;
    else if (s == "string")
        result = kind::string;
    else if (s == "dummy")              /* used for non-existent options    */
        result = kind::dummy;

    return result;
}

/*--------------------------------------------------------------------------
 * Free functions
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * Default list of options
 *--------------------------------------------------------------------------*/

/*
 *  Internal and commonly useful options.  The following option codes are
 *  unavailable to other application code if the user allows the addition
 *  of these items.
 *
\verbatim
        0123456789#@AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz
                                   o                o         oo
\endverbatim
 *
 *  Note that we do not use common codes like 'i' (input) and 'o' (output).
 *  However, 'h' (help), 'V' (verbose), and 'v' (version) are commonly
 *  used for those options.
 *
 *  Only two options here could be considered "permanent" (i.e. saved to
 *  a file):
 *
 *      -   "log"
 *      -   "option"
 *
 * Fields:
 *
 *   Name, Code, Kind, Enabled, Default, Value, FromCli, Dirty,
 *   Description
 */

options::container &
global_options ()
{
    static options::container s_default_options =
    {
        {
            "description",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Flags application to show extra descriptive information.",
                true
            }
        },
        {
            "help",
            {
                'h', options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Show this help text.", true
            }
        },
        {
            "inspect",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "", false, false,
                "This is a trouble-shooting option.", true
            }
        },
        {
            "investigate",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "", false, false,
                "This is another trouble-shooting option.", true
            }
        },
        {
            "log",
            {
                options::code_null, options::kind::filename, options::enabled,
                "", "", false, false,
                "Specifies use of a log file." /* --option log[=file] */, true
            }
        },
        {
            "option",
            {
                options::code_null, options::kind::overflow, options::enabled,
                "false", "", false, false,
                "Handles 'overflow' options (no character code).", true
            }
        },
        {
            "quiet",
            {
                'Q', options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Hide startup warnings.", true
            }
        },
        {
            "verbose",
            {
                'V', options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Show extra information.", true
            }
        },
        {
            "version",
            {
                'v', options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Show version information.", true
            }
        }
    };
    return s_default_options;
}

/**
 *  Solves the problem of comparing floating-point values.  For example,
 *  using the string "3.14159" to set a value yields "3.1415901184083021",
 *  while using the same string as a double argument yields "3.14158999...".
 *  The precision of a float is 7 digits, and the precision of a double is
 *  15 digits. So we choose the lower limit, good enough for handling most
 *  floating-point option values. The method is described here; we use
 *  T = float:
 *
 *      https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
 *
 *  The machine epsilon has to be scaled to the magnitude of the values used
 *  and multiplied by the desired precision in ULPs (units in the last place)
 *  unless the result is subnormal.
 *
 * \param ftarget
 *      Provides the first number.
 *
 * \param fsource
 *      Provides the second number.
 *
 * \param ulp
 *      Provides the desired precision in units in the last place.
 *      It defaults to 7, values less than 7 can be used.
 *
 * \return
 *      Returns true if the arguments, as floats, are equal enough.
 */

bool
almost_equal (float ftarget, float fsource, int ulp)
{
#if defined STD_FABSF_AVAILABLE             /* not in g++ v. 9, bug? */
    float diff = std::fabsf(fsource - ftarget);
    float total = std::fabsf(fsource + ftarget);
#else
    float diff = std::fabs(double(fsource - ftarget));
    float total = std::fabs(double(fsource + ftarget));
#endif
    float max = std::numeric_limits<float>::epsilon() * total * ulp;
    return diff <= max || diff < std::numeric_limits<float>::min();
}

/**
 *  The almost_equal() function doesn't work for comparing values close
 *  to 0.1; the max is oddly small.
 *
 *  Very simplistic.
 *
 * \param ftarget
 *      Provides the first number.
 *
 * \param fsource
 *      Provides the second number.
 *
 * \param precision
 *      Defines the maximum allowable difference. The caller must
 *      have some knowledge of the magnitude of the source and target
 *      numbers.  For example, decimal fractions on the order of 0.1
 *      should differ by, say 0.001. If set to 0.0, then the precision
 *      is set to 0.001 times the magnitude of the target, unless the target
 *      is less than 1..
 *
 * \return
 *      Returns true if the arguments, as floats, are equal enough.
 */

bool
approximates (float ftarget, float fsource, float precision)
{
#if defined STD_FABSF_AVAILABLE             /* not in g++ v. 9, bug? */
    float diff = std::fabsf(fsource - ftarget);
    if (precision == 0.0)
        precision = 0.001 * std::fmaxf(1.0, std::fabsf(ftarget));
#else
    float diff = std::fabs(double(fsource - ftarget));
    if (precision == 0.0)
        precision = 0.001 * std::fmaxf(1.0, std::fabs(ftarget));
#endif

    return diff < precision;
}

/**
 *  Creates an option.
 */

options::option
make_option
(
    const std::string & name,
    const options::spec & s
)
{
    options::option result;
    result.first = name;
    result.second = s;
    return result;
}

}           // namespace cfg

/*
 * options.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

