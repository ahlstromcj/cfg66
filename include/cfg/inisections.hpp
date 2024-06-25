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
 * \date          2024-06-19
 * \updates       2024-06-25
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
 *      -   inisections
 */

#include <functional>                   /* std::reference_wrapper<>         */
#include <string>                       /* std::string class                */
#include <vector>                       /* std::vector container            */

#include "cfg/inisection.hpp"           /* cfg::inisection class            */
#include "cfg/options.hpp"              /* cfg::options class               */

#undef CFG66_USE_INIFILES

namespace cfg
{

/**
 *  Provides a list of INI section specifications for a given INI file.
 *  That is, each INI file will contain an inisections object. Thus, they
 *  are closely related.
 */

class inisections
{

    friend class inifile;

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
     *  or just a file extension such as "rc". For now, prefer the latter,
     *  and treat it as a "configuration type".
     */

    std::string m_name;

    /**
     *  The stock file-extension to use for this INI file. Examples would
     *  be '.session', '.rc', and '.usr'.
     */

    std::string m_extension;

    /**
     *  The type of configuration file. Usually similar to the file extension.
     */

    std::string m_config_type;

    /**
     *  Holds a description of the INI file.
     */

    std::string m_description;

    /**
     *  Holds a vector of inisection objects in the order they appear in the
     *  initializers.
     */

    sectionlist m_section_list;

public:

    inisections () = default;
    inisections (const std::string & ininame);
    inisections (const std::string & ininame, inisections::specification & spec);
    inisections (const inisections & ini) = default;
    inisections & operator = (const inisections & ini) = default;
    ~inisections () = default;

    std::string settings_text () const;

    bool active () const
    {
        return m_section_list.size() > 0;
    }

    /**
     *  Do we want to push the object or a reference wrapper?
     */

    bool add (inisection & section)
    {
        m_section_list.push_back(section);
        return true;
    }

    void clear ()
    {
        m_section_list.clear();
    }

    const std::string & config_type () const
    {
        return m_config_type;
    }

    const sectionlist & section_list () const
    {
        return m_section_list;
    }

    /*
     *  These will return a dummy (empty) references if not found.
     */

    const inisection & find_inisection (const std::string & sectionname) const;
    const options & find_options (const std::string & sectionname) const;

private:

    sectionlist & section_list ()
    {
        return m_section_list;
    }

    inisection & find_inisection (const std::string & sectionname);
    options & find_options (const std::string & sectionname);

};          // class inisections

#if defined CFG66_USE_INIFILES

/*------------------------------------------------------------------------
 * inifiles
 *------------------------------------------------------------------------*/

/**
 *  Holds a list of inisections objects. Represents all of the files, and all
 *  of the options that are held by the files.  The options are in a single
 *  list, and the INI items look them up by name.
 *
 *  NOT SURE THIS CONCEPT IS USEFUL.
 *  See the inifile class/module instead.
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

#endif      // defined CFG66_USE_INIFILES

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

