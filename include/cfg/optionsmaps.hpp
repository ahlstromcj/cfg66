#if ! defined CFG66_CFG_OPTIONSMAPS_HPP
#define CFG66_CFG_OPTIONSMAPS_HPP

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
 * \file          optionsmaps.hpp
 *
 *      Provides a way to hold all options from multiple INI-style files and
 *      multiple INI file sections.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-01-08
 * \updates       2024-06-17
 * \license       See above.
 *
 */

#include <memory>                       /* std::shared_ptr<> template       */
#include <vector>                       /* std::vector container            */
#include <string>                       /* std::string class                */

#include "cfg/options.hpp"              /* cfg::options class               */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cfg
{

/**
 *  How to access options in a way that supports the following:
 *
 *      -#  Options in various configuration files.
 *      -#  Options in relatively small sections suitable to serve as
 *          mementos.
 *      -#  Easily looked up for setting and getting configuration items.
 *
 *  For the files, it seems reasonable to create xxx_settings and xxx_files
 *  modules as done in Seq66, and let each file set/get items by calling a
 *  function like option_set(section_name, item_name, value_string).
 *
 *  We would like to be able to save all files in one swoop or save each
 *  file individually. For all, use the inifiles class.  For one, use the
 *  singular inifile class.
 *
 *  For the sections, note that the options class supports a list of option
 *  values keyed by the names of the options. The names *must* be unique.
 *  We have an inisection class that provides a small list of options
 *  that has a section-name. Each section can be used as a memento.
 *
 *  We would like to be able to save all sections in one swoop. For that,
 *  the infile class can be used.
 *
 *  How to create all of the options for an application? If we keep all
 *  option names unique, we could use a long list, but it seems better
 *  to partition them by file, and in each file, partition them by sections.
 *
 */

class optionmap
{

private:

public:

    optionsmaps (const std::string & file = "", const std::string & section = "");
    optionsmaps (const optionsmaps & other) = default;
    optionsmaps & operator = (const optionsmaps & other) = default;
    virtual ~optionsmaps () = default;

    void reset (bool addverhelp = false);
    void initialize ();

    void clear ()
    {
        // option_specs().clear();
    }

    size_t size () const
    {
        // return option_specs().size();
    }

    bool empty () const
    {
        // return option_specs().empty();
    }

    bool add (const spec & s);
    bool add (const optionsmaps & os);
    bool change_value
    (
        const std::string & name,
        const std::string & value,
        bool boolvalue = false
    );
    bool modified () const;
    void unmodify (const std::string & name);
    void unmodify_all ();

    protected:

    bool option_cli_enabled (const std::string & s)
    {
        // return s.option_cli_enabled; // == opt::enabled;
    }

    bool option_is_boolean (const std::string & name) const;

    bool option_is_boolean (const std::string & s) const
    {
        return s.option_kind == "bool";
    }

    bool option_is_int (const std::string & s) const
    {
        return s.option_kind == "integer";
    }

    bool option_is_float (const std::string & s) const
    {
        return s.option_kind == "float" || s.option_kind == "double";
    }

    bool option_is_string (const std::string & s) const
    {
        return s.option_kind == "string";
    }

    bool option_is_overflow (const spec & s) const
    {
        return s.option_kind == "overflow";
    }

    const spec & find_match (const std::string & name) const;

};          // class optionsmaps

}           // namespace cfg

#endif      // CFG66_CFG_OPTIONSMAPS_HPP

/*
* optionsmaps.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

