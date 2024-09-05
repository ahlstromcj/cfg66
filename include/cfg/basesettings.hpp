#if ! defined CFG66_CFG_BASESETTINGS_HPP
#define CFG66_CFG_BASESETTINGS_HPP

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
 * \file          basesettings.hpp
 *
 *  This module declares/defines basic configuration settings.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2019-01-17
 * \updates       2024-09-05
 * \license       GNU GPLv2 or above
 *
 *  This module defines some items common to all configuration files that get
 *  written.
 */

#include <string>                       /* std::string character class      */
#include <vector>                       /* std::vector container class      */

#include "cfg/comments.hpp"             /* cfg::comments class              */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace cfg
{

/**
 *  Holds the current values of sequence settings and settings that can modify
 *  the number of sequences and the configuration of the user-interface.
 *  These settings will eventually be made part of the "user" settings file.
 */

class basesettings
{

private:

    /**
     *  Provides an optional file-name for the settings object.
     */

    std::string m_file_name;

    /**
     *  Indicates if the settings have been modified (in the user interface).
     *  Starts out false.
     */

    bool m_modified;

    /**
     *  A new item that indicates the format of the file. For now,
     *  it is a free-form string.  Possible values would be:
     *  "INI", "XML", "JSON".
     */

    std::string m_config_format;

    /**
     *  [Cfg66] config-type
     *
     *  This item is a short string that indicates something about the
     *  format or content of the file.  For example, in Seq66, common
     *  values were 'rc' and 'usr', both using Seq66's INI-like format,
     *  with these values also representing the file extension.
     */

    std::string m_config_type;

    /**
     *  [Cfg66] version
     *
     *  A [Cfg66] marker section indicates the ordinal version of the file.
     *  Starts at 0, and is incremented when a new feature is added or a
     *  change is made.
     */

    int m_ordinal_version;

    /**
     *  [comments]
     *
     *  Provides a way to embed comments in the configuration file and not lose
     *  them when the file is auto-saved.
     */

    comments m_comments_block;

    /**
     *  Holds a buffer of error message(s).
     */

    mutable std::string m_error_message;

    /**
     *  Indicates if the error message buffer contains error messages.
     */

    mutable bool m_is_error;

public:

    basesettings
    (
        const std::string & filename    = "",
        const std::string & cfgformat   = "",
        const std::string & cfgtype     = "",
        const std::string & comtext     = "",
        int version = 0
    );
    basesettings (const basesettings & rhs) = default;
    basesettings (basesettings && rhs) = default;
    basesettings & operator = (const basesettings & rhs) = default;
    basesettings & operator = (basesettings && rhs) = default;
    virtual ~basesettings () = default;

    virtual void set_defaults ();
    virtual void normalize ();

public:

    const std::string & file_name () const
    {
        return m_file_name;
    }

    bool modified () const
    {
        return m_modified;
    }

    void modify ()
    {
        m_modified = true;
    }

    void unmodify ()
    {
        m_modified = false;
    }

    const std::string & config_format ()
    {
        return m_config_format;
    }

    const std::string & config_type ()
    {
        return m_config_type;
    }

    int ordinal_version () const
    {
        return m_ordinal_version;
    }

    const comments & comments_block () const
    {
        return m_comments_block;
    }

    comments & comments_block ()
    {
        return m_comments_block;
    }

    const std::string & error_message () const
    {
        return m_error_message;
    }

    virtual bool set_error_message (const std::string & em) const;

    bool is_error () const
    {
        return m_is_error;
    }

protected:

    void ordinal_version (int value)
    {
        m_ordinal_version = value;
    }

    void increment_ordinal_version ()
    {
        ++m_ordinal_version;
    }

    void config_format (const std::string & cf)
    {
        m_config_format = cf;
    }

    void config_type (const std::string & ct)
    {
        m_config_type = ct;
    }

};          // class basesettings

}           // namespace cfg

#endif      // CFG66_CFG_BASESETTINGS_HPP

/*
 * basesettings.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

