#if ! defined CFG66_CFG_INISECTIONS_HPP
#define CFG66_CFG_INISECTIONS_HPP

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
 * \file          inisections.hpp
 *
 *      Provides a way to hold all options from multiple INI-style files and
 *      multiple INI file sections.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-02-25
 * \updates       2024-06-15
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
 *
 *  Classes supported:
 *
 *      -   inispecification
 *      -   inisection
 *      -   inisections
 *      -   inifiles (very tentative at this time)
 */

#include <functional>                   /* std::reference_wrapper<>         */
#include <string>                       /* std::string class                */
#include <vector>                       /* std::vector container            */

#include "cfg/options.hpp"              /* cfg::options class               */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cfg
{

/*------------------------------------------------------------------------
 * inimap
 *------------------------------------------------------------------------*/

/**
 *  Collects all of the option objects into one object for straightforward
 *  lookup by section-name and option-name.
 *
 *  Meant for unique-option names, spanning many inifiles and inisections.
 */

class inimap
{

public:

    /**
     *  Holds reference_wrappers for each option.
     */

    using optionref = std::reference_wrapper<options::spec>;
    using optionmap = std::map<std::string, optionref>;

private:

    optionmap m_option_map;

public:

    inimap ();

    int count () const
    {
        return int(m_option_map.size());
    }

    optionmap & option_map ()
    {
        return m_option_map;
    }

    const optionmap & option_map () const
    {
        return m_option_map;
    }

public:

    bool add_option (const std::string & option_name, options::spec & op);

};

/*------------------------------------------------------------------------
 * inisection
 *------------------------------------------------------------------------*/

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
     *  options were read. This name is merely the extension of the file, such
     *  as "rc", "usr", etc.  If empty, there is no source file associated
     *  with this option, which should be uncommon.
     */

    std::string m_extension;

    /**
     *  The name of the INI section. It must include the "[]" wrapping,
     *  as in "[Cfg66]" or "[directories]".
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
     *  different order. TODO: write a function to do it???
     */

    names m_option_names;

    /**
     *  Access to the options themselves.
     */

    options m_options;

public:

    inisection () = default;
    inisection
    (
        inisection::specification & spec,   /* name, description, opt list  */
        const std::string & extension,      /* mandatory file-kind ('rc')   */
        const std::string & sectname = ""   /* can override the name        */
    );
    inisection (const inisection & inif) = default;
    inisection & operator = (const inisection & inif) = default;
    ~inisection () = default;

    std::string settings_text () const;
    std::string description_commented () const;

    bool add_option (const options::option & op)    /* add one option       */
    {
        return m_options.add(op);
    }

    bool add_options (const options & opts)         /* add many options     */
    {
        return m_options.add(opts);
    }

    bool add_options_to_map (inimap & mapp);

    bool add_name (const std::string & optionname)
    {
        bool result = ! optionname.empty();
        if (result)
            m_option_names.push_back(optionname);

        return result;
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

    options option_set ()
    {
        return m_options;
    }

    const options option_set () const
    {
        return m_options;
    }

};          // inisection

/*------------------------------------------------------------------------
 * inisections
 *------------------------------------------------------------------------*/

/**
 *  Provides a list of INI section names for a given INI file.
 */

class inisections
{

public:

    /**
     *  We have a minor issue that populating all our sections and options
     *  from static data defined in header files (see libraries/cfg66/tests
     *  and the *_spec.hpp files) can lead to duplication of data and much
     *  more memory being used.  Therefore, instead of storing the
     *  inisection::specifications in a vector, we store references to them,
     *  using std::reference_wrapper.  This also has the benefit of
     *  guaranteeing that the specifications are not owned.
     *
     *  Instances of std::reference_wrapper are objects (they can be copied
     *  or stored in containers), but they are implicitly convertible to T &,
     *  and so can be used as arguments of functions taking the underlying
     *  type by reference.
     */

    using specref = std::reference_wrapper<inisection::specification>;
    using specrefs = std::vector<specref>;

    /**
     *  While specref can be used in initializer lists, we
     *  also need to "convert" them to inisections.
     *
     *  using sectionref = std::reference_wrapper<inisection>;
     *  using sectionrefs = std::vector<sectionref>;
     */

    using sectionlist = std::vector<inisection>;

    struct specification
    {
        std::string file_extension;
        std::string file_directory;
        std::string file_basename;
        std::string file_description;
        specrefs file_sections;
    };

private:

    /**
     *  Describes the application. Will work this one out later.
     */

    std::string m_app_version;

    /**
     *  The location of the INI file.
     */

    std::string m_directory;

    /**
     *  The name of the INI file as a group.  This could be a file-name,
     *  or just a file extension such as "rc".
     */

    std::string m_name;

    /**
     *  The stock file-extension to use for this INI file. Examples would
     *  be 'session', 'rc', and 'usr'.
     */

    std::string m_extension;

    /**
     *  Holds a description of the INI file.
     */

    std::string m_description;

    /**
     *  Holds a vector of inisection objects in the order
     *  they appear in the initializers.
     */

    sectionlist m_section_list;

public:

    inisections () = default;
    inisections (const std::string & ininame);
    inisections (const std::string & ininame, inisections::specification & spec);
    inisections (const inisections & inif) = default;
    inisections & operator = (const inisections & inif) = default;
    ~inisections () = default;

    std::string settings_text () const;

    bool add (inisection & section)
    {
        m_section_list.push_back(section);
        return true;
    }

    void clear ()
    {
        m_section_list.clear();
    }

    sectionlist & section_list ()
    {
        return m_section_list;
    }

    const sectionlist & section_list () const
    {
        return m_section_list;
    }

    bool add_options_to_map (inimap & mapp);

};          // class inisections

/*------------------------------------------------------------------------
 * inifiles
 *------------------------------------------------------------------------*/

/**
 *  Holds a list of inisections objects. Represents all of the files, and all of
 *  the options that are held by the files.  The options are in a single list,
 *  and the INI items look them up by name.
 *
 *  NOT SURE THIS CONCEPT IS USEFUL.
 */

class inifiles
{

public:

    using ref = std::reference_wrapper<inisections>;
    using refs = std::vector<inisections>;

private:

    /**
     *  The name of the INI sections as a group.  This could be a file-name,
     *  or just a file extension such as "rc".
     */

    std::string m_name;

    /**
     *  Holds a vector of references to inisections objects.
     */

    refs m_file_list;

    /**
     *  Holds std::reference_wrappers to all options, whether CLI-enabled or
     *  not.
     */

    inimap m_all_options;

public:

    inifiles () = default;
    inifiles (const std::string & name);
    inifiles (const inifiles & inif) = default;
    inifiles & operator = (const inifiles & inif) = default;
    ~inifiles () = default;

    bool add (const inisections & file);
    bool add_options_to_map (inimap & mapp);

    void clear ()
    {
        m_file_list.clear();
    }

    refs & file_list ()
    {
        return m_file_list;
    }

    const refs & file_list () const
    {
        return m_file_list;
    }

    inimap all_options ()
    {
        return m_all_options;
    }

    const inimap all_options () const
    {
        return m_all_options;
    }

};          // class inifiles

/*------------------------------------------------------------------------
 * Reusable objects for "[Cfg66]" and "[comments]"
 *------------------------------------------------------------------------*/

extern inisection::specification inifile_cfg66_data;
extern inisection::specification inifile_comment_data;

/*------------------------------------------------------------------------
* Free functions
*------------------------------------------------------------------------*/

extern const inisection & get_inifile_cfg66_section ();
extern const inisection & get_inifile_comment_section ();

}           // namespace cfg

#endif      // CFG66_CFG_INISECTIONS_HPP

/*
* inisections.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

