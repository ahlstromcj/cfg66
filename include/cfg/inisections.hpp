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
    friend class inimanager;

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
        std::string file_extension;         /**< Also used as config type.  */
        std::string file_directory;         /**< Often the home directory.  */
        std::string file_basename;          /**< Name without path or .ext. */
        std::string file_description;       /**< Shown at top of INI file.  */
        specrefs file_sections;             /**< Contains "[sections]".     */
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
     *  The name of the INI file as a group.  This should be the base of
     *  a file-name (i.e. "filename.rc" without the ".rc"), and is treated
     *  as such.
     */

    std::string m_base_name;

    /**
     *  The file-extension to use for this INI file. Examples would
     *  be '.session', '.rc', and '.usr', which are some of the "stock"
     *  file extensions supported by Cfg66.
     */

    std::string m_extension;

    /**
     *  The type of configuration file. Usually the same as the file
     *  extension, but without the period.
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

    inisections ();
    inisections (const std::string & ininame);
    inisections
    (
        inisections::specification & spec,
        const std::string & ininame = ""
    );
    inisections (const inisections &) = default;
    inisections (inisections &&) = default;
    inisections & operator = (const inisections &) = default;
    inisections & operator = (inisections &&) = default;
    ~inisections () = default;

    std::string settings_text () const;
    std::string cli_help_text () const;
    std::string help_text () const;
    std::string debug_text () const;
    std::string file_specification
    (
        const std::string & basename = "",
        const std::string & cfgtype = ""
    ) const;

    bool active () const
    {
        return section_list().size() > 0;
    }

    /**
     *  Do we want to push the object or a reference wrapper?
     */

    bool add (inisection & section)
    {
        section_list().push_back(section);
        return true;
    }

    void clear ()
    {
        section_list().clear();
    }

    const std::string & directory () const
    {
        return m_directory;
    }

    const std::string & base_name () const
    {
        return m_base_name;
    }

    const std::string & extension () const
    {
        return m_extension;
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

    const inisection & find_inisection
    (
        const std::string & sectionname = global /* default = stock options */
    ) const;
    const options & find_options
    (
        const std::string & sectionname = global
    ) const;

    const options::spec & find_option_spec (const std::string & name) const;
    bool add_options
    (
        const options::container & specs,
        const std::string & sectionname = global
    );

private:

    sectionlist & section_list ()
    {
        return m_section_list;
    }

    void extract_file_values (const std::string & fname);
    void fix_extension (const std::string & ext);
    std::string fix_section_name (const std::string & s) const;
    inisection & find_inisection (const std::string & sectionname = global);
    options & find_options (const std::string & sectionname = global);
    options::spec & find_option_spec (const std::string & name);

};          // class inisections

}           // namespace cfg

#endif      // CFG66_CFG_INISECTIONS_HPP

/*
* inisections.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

