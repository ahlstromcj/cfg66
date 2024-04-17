#if ! defined CFG66_CFG_OPTIONS_HPP
#define CFG66_CFG_OPTIONS_HPP

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
 * \file          options.hpp
 *
 *      Provides a way to hold command-line and file-provided configuration
 *      options.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2023-08-11
 * \license       See above.
 *
 *  Supports variables of the following types:
 *
 *      -   "boolean". Each value is either true or false.
 *      -   "filename". A file-name component, such as a path, a base
 *          file-name, or a full file-specification (path + base).
 *      -   "floating". The value is either a float or a double.
 *      -   "floatpair". Two floating values in a format like "3.0x4.0".
 *      -   "integer". The value is an integer, whether signed or unsigned.
 *      -   "intpair". Two integer values in a format like "3x4".
 *      -   "list". The value is a multi-line list of items, such as
 *          a list of busses or a list of recent files. The first value
 *          is a count of the items. The whole section is just this list.
 *      -   "overflow". The option is supported by the "--option" option.
 *      -   "section". The INI-section has no variables, but contains
 *          some section-specific values.  Example: the "[comments]"
 *          section, containing paragraphs.
 *      -   "string".  The value is a human readable string. It can also
 *          be used to hold string versions of enumeration values.
 */

#include <string>                       /* std::string class                */
#include <map>                          /* std::map container               */

#include "cpp_types.hpp"                /* enum class opt                   */

/**
 *  Versions less than C++20 cannot use initializer lists with structures that
 *  have a user-defined constructor.
 *
 *  As per https://en.cppreference.com/w/cpp/language/aggregate_initialization
 *  a class type (typically, struct or union), must have the following features.
 *
 *  -   no user-declared constructors (until C++11)
 *  -   no user-provided, inherited, or explicit constructors
 *      (since C++11, until C++20)
 *  -   no user-declared or inherited constructors (since C++20)
 *  -   no private or protected direct (since C++17) non-static data members
 *  -   no base classes (until C++17)
 *  -   no virtual base classes
 *  -   no private or protected direct base classes (since C++17)
 *  -   no virtual member functions
 *  -   no default member initializers (since C++11, until C++14)
 */

#undef USER_CONSTRUCTOR_FOR_OPTIONS_SPEC

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cli
{
    class parser;
}

namespace cfg
{

/**
 *  Accessor function class.
 */

class options
{
    friend class cli::parser;

public:

    /**
     *  Provides a clearer specification than bare true/false.
     *  Compare to the C enumeration opt_flags in the c_macros.h module.
     *  Note that these values currently apply to whether an option
     *  is enabled from the command-line. If not, it is likely an option
     *  supported only in a configuration file.
     */

    static const bool disabled{false};
    static const bool enabled{true};

    /**
     *  More searchable versions of specific booleans and a null option
     *  code character.
     */

    static const char code_null{0};

    /**
     *  Control of the output for a terminal. We don't try to determine
     *  the width of the terminal, we assume it is at least 80 characters
     *  wide. The description will not be output if it makes the line
     *  longer than about 78.
     */

    static const size_t hanging_width{25};                      /* tricky   */
    static const size_t field_width{40};
    static const size_t terminal_width{78};

    /**
     *  The kinds of options supported, mostly representing various
     *  data-types.
     */

    enum class kind
    {
        boolean,                        /* values are "true" or "false"     */
        filename,                       /* a quoted file specification      */
        floating,                       /* a float or double value          */
        floatpair,                      /* two float or double values       */
        integer,                        /* an integer value                 */
        intpair,                        /* two integer values               */
        list,                           /* integer value counting elements  */
        overflow,                       /* an --option or -o option         */
        section,                        /* only an un-named string value    */
        string,                         /* a quoted string                  */
        dummy                           /* used when option can't be found  */
    };

    /**
     *  A structure holding information about a command-line option.
     *  See the string option_kind values in the options.cpp module.
     *  Also compare to the cliparser_c.h header file's options_spec
     *  structure.
     *
     *  Rules (partially enforced):
     *
     *      -#  The long name must be non-empty. The code name (single
     *          letter) can be empty, as we have a limited number of
     *          characters (0-9, A-Z, and a-z) to employ.
     *      -#  The code name must be 1 character long.
     *      -#  The long name must be longer than 1 characters.
     *      -#  All options where option_kind is "bool" represent a
     *          boolean value (strings "true" or "false" exactly).
     *          The presence of the option means "true".
     */

    class spec
    {
        friend class options;

#if defined USER_CONSTRUCTOR_FOR_OPTIONS_SPEC
    private:
#else
    public:
#endif

        char option_code;           /**< Optional single-character name.    */
        std::string option_kind;    /**< Is it boolean, integer, string...? */
        bool option_cli_enabled;    /**< Normally true; false disables.     */
        std::string option_default; /**< Either a value or "true"/"false"   */
        std::string option_value;   /**< The actual value as parsed.        */
        bool option_read_from_cli;  /**< Option already set from CLI.       */
        bool option_modified;       /**< Option changed since read/save.    */
        std::string option_desc;    /**< A one-line description of option.  */
        bool option_built_in;       /**< This option present in all apps.   */

#if defined USER_CONSTRUCTOR_FOR_OPTIONS_SPEC   /* see top of this module   */

    public:

        spec () = default;

        spec
        (
            const char * name, char code, const char * kind,
            bool enabled, const char * defalt, const char * value,
            /* bool option_read_from_cli, */ bool modified, const char * desc
        );
        spec (const spec &) = default;
        spec & operator = (const spec &) = default;

#endif

    };

public:

    /**
     *  An option is an options::spec keyed by a name.
     */

    using option = std::pair<std::string, spec>;

    /**
     *  A list of options supported by a particular application.  This can be
     *  used as a lookup list and as an output list for the options actually
     *  found. The key is the name of the option.
     */

    using container = std::map<std::string, spec>;      /* std::map<option> */

private:

    /**
     *  Provides the source configuration file from which these options were
     *  read. This name is merely the extension of the file, such as "rc",
     *  "usr", etc.  If empty, there is no source file associated with this
     *  option.
     */

    std::string m_source_file;

    /**
     *  Provides the section of the configuration file from which these
     *  were read.  (For command-line settings, we have to be able to
     *  search for the option name to find out what section holds it.
     *  If empty, there is no section associated with this option.
     */

    std::string m_source_section;

    /**
     *  The container (map) of options::spec objects.
     */

    container m_option_pairs;

public:

    options (const std::string & file = "", const std::string & section = "");
    options
    (
        const container & specs,
        const std::string & file = "",
        const std::string & section = ""
    );
    options (const options & other) = default;
    options & operator = (const options & other) = default;
    ~options () = default;

    static std::string kind_to_string (kind k);
    static kind string_to_kind (const std::string & s);

    void reset ();
    void initialize ();

    const std::string & source_file () const
    {
        return m_source_file;
    }

    const std::string & source_section () const
    {
        return m_source_section;
    }

    container & option_pairs ()
    {
        return m_option_pairs;
    }

    const container & option_pairs () const
    {
        return m_option_pairs;
    }

    void clear ()
    {
        option_pairs().clear();
    }

    size_t size () const
    {
        return option_pairs().size();
    }

    bool empty () const
    {
        return option_pairs().empty();
    }

    bool add (const options::option & op);
    bool add (const options & os);
    bool verify () const;
    bool change_value
    (
        const std::string & name,
        const std::string & value,
        bool fromcli = false
    );
    bool modified () const;
    bool was_read_from_cli (const std::string & name) const;
    void set_read_from_cli (const std::string & name, bool flag = true);
    void unmodify (const std::string & name);
    void unmodify_all ();
    std::string help_line (const std::string & name) const;
    std::string help_line (const option & opt) const;
    std::string help_text () const;
    std::string setting_line (const std::string & name) const;
    std::string setting_line (const option & op) const;
    std::string debug_line (const option & op) const;
    std::string debug_text (bool show_builtins = false) const;
    std::string description (const std::string & name) const;
    std::string description () const;
    std::string long_description (const option & op) const;
    std::string value (const std::string & name) const;
    std::string default_value (const std::string & name) const;
    bool boolean_value (const std::string & name) const;
    int integer_value (const std::string & name) const;
    float floating_value (const std::string & name) const;
    int integer_value_range
    (
        const std::string & name,
        int & minimum, int & maximum
    ) const;
    float floating_value_range
    (
        const std::string & name,
        float & minimum, float & maximum
    ) const;

protected:

    void source_file (const std::string & f)
    {
        m_source_file = f;
    }

    void source_section (const std::string & s)
    {
        m_source_section = s;
    }

    bool option_is_boolean (const std::string & name) const;

    bool option_is_boolean (const spec & s) const
    {
        return s.option_kind == "boolean";
    }

    /**
     *  Tricky code. A list starts with an integer count value, so it is
     *  included here.  (The caller must produce the list somehow.)
     */

    bool option_is_int (const spec & s) const
    {
        return s.option_kind == "integer" || s.option_kind == "list";
    }

    bool option_is_int_pair (const spec & s) const
    {
        return s.option_kind == "intpair";
    }

    bool option_is_float (const spec & s) const
    {
        return s.option_kind == "floating";
    }

    bool option_is_float_pair (const spec & s) const
    {
        return s.option_kind == "floatpair";
    }

    bool option_is_number (const spec & s) const
    {
        return s.option_kind == "integer" || s.option_kind == "floating";
    }

    bool option_is_string (const spec & s) const
    {
        return s.option_kind == "string";
    }

    bool option_is_quotable (const spec & s) const
    {
        return s.option_kind == "string" || s.option_kind == "filename";
    }

    bool option_is_overflow (const spec & s) const
    {
        return s.option_kind == "overflow";
    }

    bool option_is_filename (const spec & s) const
    {
        return s.option_kind == "filename";
    }

    bool option_is_list (const spec & s) const
    {
        return s.option_kind == "list";
    }

    /**
     *  A "section" option has no variables, just one item of data.
     *  Clumsy?
     */

    bool option_is_section (const spec & s) const
    {
        return s.option_kind == "section";
    }

    std::string help_line (const spec & op) const;
    bool option_exists (const std::string & name) const;

    bool option_exists (const spec & s) const
    {
        return s.option_kind != "dummy";
    }

    bool option_exists (container::const_iterator oit) const
    {
        return oit != m_option_pairs.end();
    }

private:

    container::const_iterator find_match (const std::string & name) const;
    std::string long_name (char code) const;
    std::string long_name (const std::string & code) const;

};          // class options

/*------------------------------------------------------------------------
 * Free functions
 *------------------------------------------------------------------------*/

extern bool almost_equal (float ftarget, float fsource, int ulp = 7);
extern bool approximates (float ftarget, float fsource, float precision = 0.0);
extern options::option make_option
(
    const std::string & name,
    const options::spec & s
);
extern options create_options
(
    const options::container & optset,
    const std::string & filename,
    const std::string & sectionname
);

}           // namespace cfg

#endif      // CFG66_CFG_OPTIONS_HPP

/*
 * options.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

