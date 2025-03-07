#if ! defined CFG66_CFG_INISECTION_HPP
#define CFG66_CFG_INISECTION_HPP

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
 * \file          inisection.hpp
 *
 *      Provides a way to hold all options from a single INI-style file
 *      section.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-06-19
 * \updates       2024-09-05
 * \license       See above.
 *
 *  We want to provide a list of { filename, sectionname } pairs, and
 *  for each pair, a options object to contain all the options for the
 *  section.
 *
 *  Then we want a parser that, unlike cli::parser, can contain multiple
 *  cfg::options objects.
 *
 *  First, the long option list is created. Then, we iterated ....
 */

#include <functional>                   /* std::reference_wrapper<>         */
#include <string>                       /* std::string class                */
#include <vector>                       /* std::vector container            */

#include "cfg/options.hpp"              /* cfg::options class               */

namespace cfg
{

/**
 *  Provides the entries in a section of an INI file.  A section is denoted
 *  by a string of the form "[section-name]".
 *
 *  For our purposes, each section holds only a list of option names. The
 *  option name is used to look up the option in a long list of all options
 *  supported in the application.
 */

class inisection
{

    friend class inimanager;
    friend class inisections;

public:

    /**
     *  This data structure is used to provide setup information for Cfg66
     *  INI-style files, their sections, and their variables. Each instance
     *  of this structure sets up one INI section (class inisection).
     */

    struct specification
    {
        std::string sec_name;
        std::string sec_description;
        cfg::options::container sec_optionlist;
    };

    /**
     *  Provide a list of options supported by this INI section. This list
     *  is normally unsorted, so that the caller can control the order
     *  of options. Also supports iterating through the option names.
     *
     *  Not sure how useful this is; it's a minor formatting issue.
     */

    using names = std::vector<std::string>;

private:

    /**
     *  Provides the kind of source configuration file from which these
     *  options were read. This name is usually the extension of the file,
     *  such as ".rc", ".usr", etc., minus the period.
     *
     *  If empty, there is no source file associated with this option, which
     *  should be uncommon. If equal to "" (empty, value cfg::global), this
     *  inisection contains only the internal stock options, which is a list
     *  defined in the cfg::options module.
     */

    std::string m_config_type;

    /**
     *  The name of the INI section. It must include the "[]" wrapping,
     *  as in "[Cfg66]" or "[directories]". Don't forget about the appinfo
     *  function set_main_cfg_section_name() to change that value.
     *
     *  If equal to "" (empty, value cfg::global), this inisection contains
     *  only the internal stock options, which is a list defined in the
     *  cfg::options module.
     */

    std::string m_name;

    /**
     *  The commentary/descriptive text that appears before the
     *  section in the INI-style file.  Each line will be preceded by
     *  the comment marker, "#", but only for output. Each line should
     *  be < 78 characters and end with a newline.
     */

    std::string m_section_description;

    /**
     *  Provide a list of option names supported by this INI section.
     *  It must match the names of the actual options, though can be in a
     *  different order.
     */

    names m_option_names;

    /**
     *  Access to the options themselves.
     */

    options m_option_set;

public:

    inisection (bool loadglobal = options::stock);  /* "global" options     */
    inisection
    (
        inisection::specification & spec,   /* name, description, opt list  */
        const std::string & extension       /* mandatory file-kind ('rc')   */
    );
    inisection (const inisection &) = default;
    inisection (inisection &&) = default;
    inisection & operator = (const inisection &) = default;
    inisection & operator = (inisection &&) = default;
    ~inisection () = default;

    std::string settings_text () const;
    std::string description_wrapped () const;
    std::string description_commented () const;
    std::string cli_help_text () const;
    std::string help_text () const;
    std::string debug_text () const;

    bool inactive () const
    {
        return m_option_set.empty();
    }

    bool active () const
    {
        return ! inactive();
    }

    bool add_options (const options::container & specs)
    {
        return m_option_set.add(specs);
    }

    /*
     *  Remember that an "option" is a std::pair<std::string, spec>.
     */

    bool add_option (const options::option & op)    /* add one option       */
    {
        return m_option_set.add(op);
    }

    bool add_name (const std::string & optionname)
    {
        bool result = ! optionname.empty();
        if (result)
            m_option_names.push_back(optionname);

        return result;
    }

    const std::string & config_type () const
    {
        return m_config_type;
    }

    const std::string & name () const
    {
        return m_name;
    }

    const std::string & section_description () const
    {
        return m_section_description;
    };

    names & option_names ()
    {
        return m_option_names;
    }

    const names & option_names () const
    {
        return m_option_names;
    }

    options & option_set ()
    {
        return m_option_set;
    }

    const options & option_set () const
    {
        return m_option_set;
    }

protected:

    /*
     *  These will return a dummy (empty inisections) reference if not found.
     */

    const options::spec & find_option_spec (const std::string & name) const;

private:

    options::spec & find_option_spec (const std::string & name);

};          // inisection

/*------------------------------------------------------------------------
 * Free functions
 *------------------------------------------------------------------------*/

extern const inisection::specification & stock_cfg66_data ();
extern const inisection::specification & stock_comment_data ();

}           // namespace cfg

#endif      // CFG66_CFG_INISECTION_HPP

/*
* inisection.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

