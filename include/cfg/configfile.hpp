#if ! defined CFG66_CFG_CONFIGFILE_HPP
#define CFG66_CFG_CONFIGFILE_HPP

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
 * \file          configfile.hpp
 *
 *  This module declares the abstract base class for configuration and
 *  options files.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2018-11-23
 * \updates       2024-09-27
 * \license       GNU GPLv2 or above
 *
 *  This is actually an elegant little parser, and works well as long as one
 *  respects its limitations.
 */

#include <fstream>                      /* std::streampos                   */
#include <string>                       /* std::string, the ubiquitous one  */

#include "cpp_types.hpp"                /* std::string, tokenization alias  */
#include "util/strfunctions.hpp"        /* util::string_to_int()           */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace cfg
{

/*
 *  Currently these strings are just for reference, and not yet used in the
 *  code for each type of configuration file.
 *
 *  -   std::string filetag = "config-type";
 *  -   std::string ctrltag = "ctrl";
 *  -   std::string ctrltag = "drums";
 *  -   std::string ctrltag = "keymap";
 *  -   std::string mutetag = "mutes";
 *  -   std::string paltag  = "palette";
 *  -   std::string playtag = "playlist";
 *  -   std::string rctag   = "rc";
 *  -   std::string playtag = "session";
 *  -   std::string usrtag  = "usr";
 */

/**
 *    This class is the abstract base class for rcfile and usrfile.
 */

class configfile
{

    friend bool delete_configuration
    (
        const std::string & path,
        const std::string & basename
    );
    friend bool copy_configuration
    (
        const std::string & source,
        const std::string & basename,
        const std::string & destination
    );

private:

    /**
     *  Holds the last error message, if any.  Not a 100% foolproof yet.
     */

    static std::string sm_error_message;

    /**
     * Indicates if we are in an error status.
     */

    static bool sm_is_error;

    /**
     *  Provides a numerical flag to use a default value for an integer.
     */

    static int sm_int_missing;
    static int sm_int_default;

    /**
     *  Provides a numerical flag to use a default value for a double or float
     *  value.
     */

    static float sm_float_missing;
    static float sm_float_default;

    /**
     *  Supported ile-extensions.  The qss extension is included, but only
     *  canoncial names like qcfg66.qss will be manipulated (apart from being
     *  read).
     */

    static lib66::tokenization sm_file_extensions;

private:

#if defined CFG66_RCSETTINGS

    /**
     *  Hold a reference to the "rc" settings object. For cfg66, this
     *  will likely be a more expansive object based on inisettings.
     */

    rcsettings & m_rc;

#endif

    /**
     *  Indicates the variety (format) of the configuration file.
     *  It can be something like the file extension of the configuration
     *  file, but without the period. Examples: "rc", "session".
     */

    std::string m_file_type;

    /**
     *  Provides the name of the configuration or other file being parsed.
     *  This will normally be a full-path specification.
     */

    std::string m_file_name;

    /**
     *  Provides the current version of the derived configuration file format.
     *  This value is set in the constructor of the configfile-derived object,
     *  and is incremented in that object whenever a new way of reading,
     *  writing, or formatting the configuration file is created.  For
     *  example, a new version of the MIDI control file code might be
     *  incremented to "3".  If the user's MIDI control file specifies
     *  "version = 2", that means that the code for this file must revert to
     *  the old format for reading the data.  When saved, the old file is
     *  upgraded to the new version.  Also useful to turn on the "--user-save"
     *  option for changes in the format of the "usr" file.
     */

    std::string m_version;

    /**
     *  The actual version specified in the configuration file, which could be
     *  older than the newest version supported in the code.
     */

    std::string m_file_version;

protected:

    /**
     *  The current line of text being processed.  This member receives
     *  an input line, and so needs to be a character buffer.
     */

    std::string m_line;

    /**
     *  Provides the current line number, useful in troubleshooting.
     */

    int m_line_number;

    /**
     *  Holds the stream position before a line is obtained.
     */

    std::streampos m_line_position;

public:

    configfile
    (
        const std::string & name,
        const std::string & cfgtype
    );

    configfile () = delete;
    configfile (configfile &&) = delete;
    configfile (const configfile &) = delete;
    configfile & operator = (const configfile &) = delete;
    configfile & operator = (configfile &&) = delete;

    /**
     *  A rote destructor needed for a base class.
     */

    virtual ~configfile()
    {
        // empty body
    }

    virtual bool parse () = 0;
    virtual bool write () = 0;

    std::string parse_comments (std::ifstream & file);
    std::string parse_section_option
    (
        std::ifstream & file,
        const std::string & secname
    );
    int parse_list
    (
        std::ifstream & file,
        const std::string & section,
        lib66::tokenization & items,
        const std::string & valuetag    = ""
    );
    std::string parse_version (std::ifstream & file);
    bool file_version_old (std::ifstream & file);

    const std::string & file_type () const
    {
        return m_file_type;
    }

    const std::string & file_name () const
    {
        return m_file_name;
    }

    void file_name (const std::string & n)
    {
        m_file_name = n;
    }

    const std::string & version () const
    {
        return m_version;
    }

    int version_number () const
    {
        return version().empty() ? 0 : util::string_to_int(version()) ;
    }

    const std::string & file_version () const
    {
        return m_file_version;
    }

    int file_version_number () const
    {
        return file_version().empty() ? 0 : util::string_to_int(file_version()) ;
    }

    bool bad_position (int p) const
    {
        return p < 0;
    }

    int line_position () const
    {
        return int(std::streamoff(m_line_position));
    }

    static const std::string & get_error_message ()
    {
        return sm_error_message;
    }

    static bool is_error ()
    {
        return sm_is_error;
    }

    static bool is_default (int value)
    {
        return value == sm_int_default;
    }

    static bool is_missing (int value)
    {
        return value == sm_int_missing;
    }

    static bool is_default (float value)
    {
        return value == sm_float_default;
    }

    static bool is_missing (float value)
    {
        return value == sm_float_missing;
    }

protected:

    bool set_up_ifstream (std::ifstream & instream);
    bool section_name_valid (const std::string & s);
    std::string make_section_name (const std::string & s);
    std::string strip_section_name (const std::string & s);

    static void append_error_message (const std::string & msg);
    static bool make_error_message
    (
        const std::string & sectionname,
        const std::string & additional = ""
    );
    static bool version_error_message
    (
        const std::string & configtype, int vnumber
    );

    void file_version (const std::string & v)
    {
        if (! v.empty())
            m_file_version = v;
    }

    void version (const std::string & v)
    {
        if (! v.empty())
            m_version = v;
    }

    void version (int v)
    {
        m_version = std::to_string(v);
    }

    /**
     *  Sometimes we need to know if there are new data lines at the end of an
     *  existing section.  One clue that there is not is that we're at the
     *  next section marker.  This function tests for that condition.
     *
     * \return
     *      Returns true if m_line[0] is the left-bracket character.
     */

    bool at_section_start () const
    {
        return m_line[0] == '[';
    }

    /**
     *  Provides the input string, to keep m_line private.
     *
     * \return
     *      Returns a constant reference to m_line.
     */

    const std::string & line () const
    {
        return m_line;
    }

    int line_number () const
    {
        return m_line_number;
    }

    std::string trimline () const;

    /**
     *  Provides a pointer to the input string in a form that sscanf() can use.
     *
     * \return
     *      Returns a constant character pointer to the data in m_line.
     */

    const char * scanline () const
    {
        return m_line.c_str();
    }

    bool get_line (std::ifstream & file, bool strip = true);
    int position_of_section
    (
        std::ifstream & file,
        const std::string & s
    );
    bool line_after_section
    (
        std::ifstream & file,
        const std::string & s,
        int position = 0,
        bool strip = true
    );
    int find_section (std::ifstream & file, const std::string & s);
    int get_section_value (const std::string & s);
    void write_date
    (
        std::ofstream & file,
        const std::string & desc = ""
    );
    bool next_data_line (std::ifstream & file, bool strip = true);
    bool next_section (std::ifstream & file, const std::string & s);
    std::string get_variable
    (
        std::ifstream & file,
        const std::string & s,
        const std::string & variablename,
        int position = 0
    );
    std::string get_next_variable
    (
        std::ifstream & file,
        const std::string & variablename,
        bool partial = false
    );
    std::string extract_variable
    (
        const std::string & line,
        const std::string & variablename,
        bool partial = false
    );
    void write_cfg66_header
    (
        std::ofstream & file,
        const std::string & configtype,
        const std::string & version
    );
    void write_cfg66_footer (std::ofstream & file);
    int write_list
    (
        std::ofstream & file,
        const std::string & section,
        const lib66::tokenization & items,
        const std::string & valuetag
    );
    bool get_boolean
    (
        std::ifstream & file,
        const std::string & s,
        const std::string & variablename,
        int position = 0,
        bool defalt = false
    );
    void write_boolean
    (
        std::ofstream & file,
        const std::string & name,
        bool status
    );
    int get_integer
    (
        std::ifstream & file,
        const std::string & s,
        const std::string & variablename,
        int position = 0
    );
    void write_integer
    (
        std::ofstream & file,
        const std::string & name,
        int value,
        bool usehex = false
    );
    float get_float
    (
        std::ifstream & file,
        const std::string & s,
        const std::string & variablename,
        int position = 0
    );
    void write_float
    (
        std::ofstream & file,
        const std::string & name,
        float value
    );
    void write_string
    (
        std::ofstream & file,
        const std::string & name,
        std::string value,
        bool quote_it = false
    );
    bool get_file_status
    (
        std::ifstream & file,
        const std::string & s,
        std::string & filename,         /* a side-effect for returning name */
        int position = 0
    );
    void write_file_status
    (
        std::ofstream & file,
        const std::string & s,
        const std::string & filename,
        bool status
    );
    void write_comment                  /* the opposite of parse_comments() */
    (
        std::ofstream & file,
        const std::string & commenttext
    );

};          // class configfile

/*
 *  Free functions.
 */

extern bool delete_configuration
(
    const std::string & path,
    const std::string & basename
);
extern bool copy_configuration
(
    const std::string & source,
    const std::string & basename,
    const std::string & destination
);
extern std::string get_current_date_time ();

}           // namespace cfg

#endif      // CFG66_CFG_CONFIGFILE_HPP

/*
 * configfile.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

