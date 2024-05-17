#if ! defined CFG66_UTIL_BYTEVECTOR_HPP
#define CFG66_UTIL_BYTEVECTOR_HPP

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
 * \file          bytevector.hpp
 *
 *  This module declares/defines a class for manipulating vectors of bytes,
 *  with support for other explicitly-sized numeric types.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-05-16
 * \updates       2024-05-17
 * \license       GNU GPLv2 or above
 *
 *  The bytevector class is meant for handling number binary data in chunks
 *  of 1, 2 4, and 8 bytes in an endian-independent way.
 *
 *  One use of this class is for handling MIDI data. MIDI files are big
 *  endian. Data transmitted over networks is supposed to be big endian.
 *  So this class is suitable for MIDI and network transmission.
 */

#if defined __cplusplus                 /* do not expose this to C code     */

#include <climits>                      /* LONG_MAX and other limits        */
#include <cstdint>                      /* uint64_t and other types         */
#include <string>                       /* std::string, basic_string        */
#include <vector>                       /* std::vector<byte> etc.           */

namespace util
{

/**
 *  Provides a fairly common type definition for a byte value.  This can be
 *  used for a buss/port number or any other byte-sized value.
 */

using byte = uint8_t;

/**
 *  A 2-byte integer value.
 */

using ushort = uint16_t;

/**
 *  A 4-byte integer value. This could be called uint with equal
 *  justification.
 */

using ulong = uint32_t;

/**
 *  An 8-byte integer value.
 */

using ulonglong = uint64_t;

/**
 *  Provides an array-like container for bytes.
 */

using bytes = std::vector<byte>;

/**
 *  This class handles the parsing and writing of byte data.
 */

class bytevector
{

private:

    /**
     *  Holds the initial or final size of the MIDI bytevector.
     */

    size_t m_size;

    /**
     *  Provides an option offset to indicate that this buffer was taken
     *  from part of a larger vector. Defaults to 0.
     */

    size_t m_offset;

    /**
     *  Holds the last error message, useful for trouble-shooting without
     *  having Seq66 running in a console window.  If empty, there's no
     *  pending error.  Currently most useful in the parse() function.
     */

    mutable std::string m_error_message;

    /**
     *  Indicates if the error should be considered fatal to the loading of
     *  the bytevector.  The caller can query for this value after getting the
     *  return value from parse().
     */

    mutable bool m_error_is_fatal;

    /**
     *  Indicates that vector read/writing has been disabled due to
     *  serious errors, so don't complain about it anymore. Once is enough.
     */

    mutable bool m_disable_reported;

    /**
     *  Holds all the bytes of the bytevector, read in at once.
     */

    util::bytes m_data;

    /**
     *  Holds the position in the bytevector. This is at least a 31-bit
     *  value in the recent architectures running Linux and Windows, so it
     *  will handle up to 2 Gb of data.  This member is used only to check
     *  the actual "reading" done by the track object.
     */

    mutable size_t m_position;

public:

    bytevector ();
    bytevector
    (
        const bytes & data,
        size_t offset   = 0,
        size_t amount   = 0
    );
    bytevector (const bytevector &) = default;
    bytevector (bytevector &&) = default;
    bytevector & operator = (const bytevector &) = default;
    ~bytevector () = default;

    void assign
    (
        const bytes & data,
        size_t offset,
        size_t amount
    );

    const std::string & error_message () const
    {
        return m_error_message;
    }

    bool error_is_fatal () const
    {
        return m_error_is_fatal;
    }

    util::bytes & byte_list ()
    {
        return m_data;
    }

    const util::bytes & byte_list () const
    {
        return m_data;
    }

    void clear ()
    {
        clear_errors();
        m_data.clear();
        m_position = 0;
    }

    void clear_errors ()
    {
        m_error_message.clear();
        m_disable_reported = false;
    }

    size_t size () const
    {
        return m_size;
    }

    size_t offset () const
    {
        return m_offset;
    }

    size_t position () const
    {
        return m_position;
    }

    /*
     *  Can be useful in debugging.
     */

    size_t real_position () const
    {
        return position() + offset();
    }

    bool seek (size_t pos)
    {
        bool result = pos < size();
        if (result)
            m_position = pos;

        return result;
    }

    void reset ()
    {
        m_position = 0;
    }

    void increment () const
    {
        if (m_position < (size() - 1))
            ++m_position;
    }

    void decrement () const
    {
        if (m_position > 0)
            --m_position;
    }

    void skip (size_t sz)
    {
        if (m_position < (size() - sz))
            m_position += sz;
    }

    util::byte get_byte () const;
    util::ushort get_short () const;
    util::ulong get_triple () const;
    util::ulong get_long () const;
    util::ulonglong get_longlong () const;
    util::ulong get_varinum ();
    std::string get_string (size_t len);

    util::byte peek_byte () const
    {
        return m_data[m_position];
    }

    util::byte peek_byte (size_t offset) const
    {
        return (m_position + offset) < m_size ?
            m_data[m_position + offset] : 0 ;
    }

    util::ushort peek_short () const;
    util::ulong peek_long () const;
    util::ulonglong peek_longlong () const;

    bool done () const
    {
        return m_position >= m_size;
    }

    bool safe () const
    {
        return ! done();
    }

    size_t remainder () const
    {
        return m_size - m_position;
    }

    void put_byte (util::byte c)
    {
        m_data.push_back(c);
        ++m_position;
    }

    void put_short (util::ushort value);
    void put_triple (util::ulong x);
    void put_long (util::ulong value);
    void put_longlong (util::ulonglong value);
    void put_varinum (util::ulong v);

    void poke_byte (util::byte c, size_t pos)
    {
        if (pos < m_size)
            m_data[pos] = c;
    }

    void poke_short (util::ushort value, size_t pos);
    void poke_long (util::ulong value, size_t pos);
    void poke_longlong (util::ulonglong value, size_t pos);

    bool read (const std::string & infilename);
    bool write (const std::string & outfilename);

public:

    bool set_error (const std::string & msg) const;
    bool set_error_dump (const std::string & msg) const;
    bool set_error_dump (const std::string & msg, unsigned long p) const;

};              // class bytevector

}               // namespace util

#endif          // defined __cplusplus : do not expose to C code

#endif          // CFG66_UTIL_BYTEVECTOR_HPP

/*
 * bytevector.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

