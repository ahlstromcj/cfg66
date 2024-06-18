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
 * \file          bytevector.cpp
 *
 *  This module declares/defines the base class for handling data in
 *  big-endian fashion.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-05-16
 * \updates       2024-05-21
 * \license       GNU GPLv2 or above
 *
 *  The bytevector class is meant to handle big-endian data in a byte-by-byte
 *  manner. We could have called it "bigendianvector" or "bigbytevector.".
 *
 *  One usage for this class is to extract datad from MIDI files. MIDI files,
 *  like network data, are big-endian. (Intel processors are little endian,
 *  Motorola processors are big-endian).
 */

#include <fstream>                      /* std::ifstream & std::ofstream    */

#include "util/bytevector.hpp"          /* util::bytevector class           */
#include "util/msgfunctions.hpp"        /* msglevel & util::msgfunctions    */

namespace util
{

/**
 *  A manifest constant for controlling the length of the stream buffering
 *  array in a file.
 */

static const int c_util_line_max = 1024;

/**
 *  Principal constructor.
 *
 * \param filespec
 *      Provides the name of the MIDI file to be read or written. This is the
 *      full path including the file-name.
 *
 * \param p
 *      Provides a mandator player object, which wll hold some important
 *      global parameters (BPM, PPQN, etc.) about the file.
 *
 * \param smf0split
 *      If true (the default), and the MIDI file is an SMF 0 file, then it
 *      will be split into multiple tracks by channel.
 */

bytevector::bytevector () :
    m_nominal_size      (0),
    m_offset            (0),
    m_error_message     (),
    m_error_is_fatal    (false),
    m_disable_reported  (false),
    m_data              (),                 /* vector of bytes              */
    m_position          (0)                 /* byte position in vector      */
{
    // no other code needed
}

/**
 *  Creates a vector of bytes from all characters in a string.
 *
 * \param s
 *      Provides a std::string to use to initialize m_data.
 */

bytevector::bytevector (const std::string & s) :
    m_nominal_size      (0),
    m_offset            (0),
    m_error_message     (),
    m_error_is_fatal    (false),
    m_disable_reported  (false),
    m_data              (),                 /* vector of bytes              */
    m_position          (0)                 /* byte position in vector      */
{
    assign(s, 0, 0);                        /* use the whole string         */
}

/**
 *  Creates a vector of bytes from all or part of another vector of bytes.
 *  Declared explicit so that a bytes parameter cannot cause bytevector(...)
 *  to be invoked.
 *
 * \param data
 *      Provides a util::bytes vector to use to initialize m_data.
 *
 * \param offset
 *      Provides the offset into \a data from which to start copying bytes.
 *
 * \param amount
 *      The number of bytes to be copied, starting at \a offset.
 */

bytevector::bytevector
(
    const bytes & data,
    size_t offset,
    size_t amount
) :
    m_nominal_size      (0),
    m_offset            (offset),
    m_error_message     (),
    m_error_is_fatal    (false),
    m_disable_reported  (false),
    m_data              (),                 /* vector of bytes              */
    m_position          (0)                 /* byte position in vector      */
{
    if (offset == 0 && amount == 0)
    {
        m_data = data;
        m_nominal_size = m_data.size();
    }
    else
        assign(data, offset, amount);
}

/**
 *  Passes an std::initializer_list<> to std::vector<>::operator =() to
 *  assign part of the data vector to m_data.
 */

void
bytevector::assign
(
    const bytes & data,
    size_t offset,
    size_t amount
)
{
    size_t high = offset + amount - 1;
    if (offset == 0 && amount == 0)
        high = data.size() - 1;

    bool ok = offset < data.size() && high < data.size();
    if (ok)
    {
        m_data = {data.begin() + offset, data.begin() + high + 1};
        m_nominal_size = m_data.size();
        m_offset = offset;
    }
}

void
bytevector::assign
(
    const bytevector & data,
    size_t offset,
    size_t amount
)
{
    const auto & dvec = data.byte_list();       /* const bytes & */
    assign(dvec, offset, amount);
}

void
bytevector::assign
(
    const std::string & s,
    size_t offset,
    size_t amount
)
{
    if (! s.empty())
    {
        size_t high = offset + amount - 1;
        if (offset == 0 && amount == 0)
            high = s.length() - 1;

        bool ok = offset < s.length() && high < s.length();
        if (ok)
        {
            size_t index = 0;
            m_data.clear();
            for (auto c : s)
            {
                if (index >= offset)
                {
                    if (index <= high)
                    {
                        util::byte b = static_cast<util::byte>(c);
                        m_data.push_back(b);
                    }
                    else
                        break;
                }
                ++index;
            }
            m_nominal_size = m_data.size();
            m_offset = offset;
        }
    }
}

/**
 *  Defined here for gdb debugging.
 */

size_t
bytevector::position () const
{
    return m_position;
}

/**
 *  Can be useful in debugging.
 */

size_t
bytevector::real_position () const
{
    return position() + offset();
}

/*-------------------------------------------------------------------------
 * get() functions
 *-------------------------------------------------------------------------*/

/**
 *  Reads 1 byte of data directly from the m_data vector, incrementing
 *  m_position after doing so.
 *
 * \return
 *      Returns the byte that was read.  Returns 0 if there was an error,
 *      though there's no way for the caller to determine if this is an error
 *      or a good value.
 */

util::byte
bytevector::get_byte () const
{
    if (m_position < m_data.size())
    {
        return m_data[m_position++];
    }
    else if (! m_disable_reported)
    {
        (void) set_error_dump("End of data encountered");
    }
    return 0;
}

/**
 *  Reads 2 bytes of data using get_byte().
 *
 * \return
 *      Returns the two bytes, shifted appropriately and added together,
 *      most-significant byte first, to sum to a short value.
 */

util::ushort
bytevector::get_short () const
{
    util::ushort result = get_byte() << 8;
    result += get_byte();
    return result;
}

/**
 *  Reads 3 bytes of data using get_byte().
 *
 * \return
 *      Returns the four bytes, shifted appropriately and added together,
 *      most-significant byte first, to sum to a long value.
 */

util::ulong
bytevector::get_triple () const
{
    util::ulong result = get_byte() << 16;
    result += get_byte() << 8;
    result += get_byte();
    return result;
}

/**
 *  Reads 4 bytes of data using get_byte().
 *
 * \return
 *      Returns the four bytes, shifted appropriately and added together,
 *      most-significant byte first, to sum to a long value.
 */

util::ulong
bytevector::get_long () const
{
    util::ulong result = get_byte() << 24;
    result += get_byte() << 16;
    result += get_byte() << 8;
    result += get_byte();
    return result;
}

/**
 *  Reads 8 bytes of data using get_byte().
 *
 * \return
 *      Returns the eight bytes, shifted appropriately and added together,
 *      most-significant byte first, to sum to a longlong value.
 */

util::ulonglong
bytevector::get_longlong () const
{
    util::ulonglong result = util::ulonglong(get_byte()) << 56;
    result += util::ulonglong(get_byte()) << 48;
    result += util::ulonglong(get_byte()) << 40;
    result += util::ulonglong(get_byte()) << 32;
    result += util::ulonglong(get_byte()) << 24;
    result += util::ulonglong(get_byte()) << 16;
    result += util::ulonglong(get_byte()) << 8;
    result += util::ulonglong(get_byte());
    return result;
}

/**
 *  Read a Variable-Length Value (VLV), which has a variable number
 *  of bytes.  This function reads the bytes while bit 7 is set in each
 *  byte.  Bit 7 is a continuation bit.  See write_varinum() for more
 *  information.
 *
 *  Duplicates the midi::bytes_to_varinum() function in the rtl66 MIDI
 *  calculations module.
 *
 * \return
 *      Returns the accumulated values as a single number.
 */

util::ulong
bytevector::get_varinum ()
{
    util::ulong result = 0;
    util::byte c;
    while (((c = get_byte()) & 0x80) != 0x00)       /* while bit 7 is set  */
    {
        result <<= 7;                               /* shift result 7 bits */
        result += c & 0x7F;                         /* add bits 0-6        */
    }
    result <<= 7;                                   /* bit was clear       */
    result += c & 0x7F;
    return result;
}

/**
 *  A function to simplify reading data from a file, starting from the
 *  current position in getting the data. It uses a standard
 *  string object instead of a buffer.
 *
 * \param b
 *      The std::string to receive the data.
 *
 * \param sz
 *      The number of bytes to be read. If zero (the default), then
 *      the position is reset and the whole vector is obtained.
 *
 * \return
 *      Returns the string, empty if an error occurs.
 */

std::string
bytevector::get_string (size_t sz)
{
    std::string result;
    if (sz == 0)
    {
        reset();
        sz = m_data.size();
    }
    for (size_t i = 0; i < sz; ++i)
    {
        if (m_position < m_data.size())
        {
            result.push_back(char(get_byte()));
        }
        else
        {
            result.clear();
            break;
        }
    }
    return result;
}

/*-------------------------------------------------------------------------
 * peek() functions
 *-------------------------------------------------------------------------*/

util::byte
bytevector::peek_byte () const
{
    if (m_position < m_data.size())
        return m_data[m_position];
    else if (! m_disable_reported)
        (void) set_error_dump("'End-of-vector', further reading disabled");

    return 0;
}

util::byte
bytevector::peek_byte (size_t offset) const
{
    if ((m_position + offset) < m_data.size())
        return m_data[m_position + offset];
    else
        util::msgprintf(lib66::msglevel::warn, "Peeking past data!");

    return 0;
}

util::byte
bytevector::peek_byte_at (size_t offset) const
{
    if (offset < m_data.size())
        return m_data[offset];
    else
        util::msgprintf(lib66::msglevel::warn, "Peeking past data!");

    return 0;
}

util::ushort
bytevector::peek_short () const
{
    util::ushort result = peek_byte() << 8;
    result += peek_byte(1);
    return result;
}

util::ulong
bytevector::peek_long () const
{
    util::ulong result = peek_byte();
    result <<= 24;
    result += peek_byte(1) << 16;
    result += peek_byte(2) << 8;
    result += peek_byte(3);
    return result;
}

util::ulonglong
bytevector::peek_longlong () const
{
    util::ulonglong result = util::ulonglong(peek_byte()) << 56;
    result += util::ulonglong(peek_byte(1)) << 48;
    result += util::ulonglong(peek_byte(2)) << 40;
    result += util::ulonglong(peek_byte(3)) << 32;
    result += util::ulonglong(peek_byte(4)) << 24;
    result += util::ulonglong(peek_byte(5)) << 16;
    result += util::ulonglong(peek_byte(6)) << 8;
    result += util::ulonglong(peek_byte(7));
    return result;
}

/**
 *  A bit too brute force, at this time.
 */

std::string
bytevector::peek_string (size_t offset, size_t amount)
{
    std::string result;
    size_t high = offset + amount - 1;
    if (offset == 0 && amount == 0)
        high = m_data.size() - 1;

    bool ok = offset < m_data.size() && high < m_data.size();
    if (ok)
    {
        for (size_t index = offset; index <= high; ++index)
        {
            char c = static_cast<char>(m_data[index]);
            result.push_back(c);
        }
    }
    return result;
}

/*-------------------------------------------------------------------------
 * put() functions
 *-------------------------------------------------------------------------*/

/**
 *  Writes 2 bytes, each extracted from the long value and shifted rightward
 *  down to byte size, using the put() function.
 *
 * \warning
 *      This code looks endian-dependent.
 *
 * \param x
 *      The short value to be written to the MIDI bytevector.
 */

void
bytevector::put_short (util::ushort x)
{
    put_byte((x & 0xFF00) >> 8);
    put_byte((x & 0x00FF));
}

/**
 *  Writes 3 bytes, each extracted from the long value and shifted rightward
 *  down to byte size, using the put_byte() function.
 *
 *  This function is kind of the reverse of tempo_us_to_bytes() defined in the
 *  calculations.cpp module of the rtl66 MIDI library.
 *
 * \param x
 *      The long value to be written to the MIDI file.
 */

void
bytevector::put_triple (util::ulong x)
{
    put_byte((x & 0x00FF0000) >> 16);
    put_byte((x & 0x0000FF00) >> 8);
    put_byte((x & 0x000000FF));
}

/**
 *  Writes 4 bytes, each extracted from the long value and shifted rightward
 *  down to byte size, using the put_byte() function.
 *
 * \param x
 *      The long value to be written to the MIDI bytevector.
 */

void
bytevector::put_long (util::ulong x)
{
    put_byte((x & 0xFF000000) >> 24);
    put_byte((x & 0x00FF0000) >> 16);
    put_byte((x & 0x0000FF00) >> 8);
    put_byte((x & 0x000000FF));
}

/**
 *  Writes 8 bytes, each extracted from the longlong value and shifted
 *  rightward down to byte size, using the put_byte() function.
 *
 * \param x
 *      The long value to be written to the MIDI bytevector.
 */

void
bytevector::put_longlong (util::ulonglong x)
{
    put_byte((x & 0xFF00000000000000) >> 56);
    put_byte((x & 0x00FF000000000000) >> 48);
    put_byte((x & 0x0000FF0000000000) >> 40);
    put_byte((x & 0x000000FF00000000) >> 32);
    put_byte((x & 0x00000000FF000000) >> 24);
    put_byte((x & 0x0000000000FF0000) >> 16);
    put_byte((x & 0x000000000000FF00) >> 8);
    put_byte(x & 0x00000000000000FF);
}

/**
 *  Writes a MIDI Variable-Length Value (VLV), which has a variable number
 *  of bytes. A MIDI file Variable Length Value is stored in bytes.
 *  See the function varinum_to_bytes() in the calculations module.
 *
 * \param v
 *      The data value to be added to the current event in the MIDI container.
 */

void
bytevector::put_varinum (util::ulong v)
{
    util::ulong buffer = v & 0x7F;                  /* mask a no-sign byte  */
    while (v >>= 7)                                 /* shift right, test    */
    {
        buffer <<= 8;                               /* move LSB bits to MSB */
        buffer |= ((v & 0x7F) | 0x80);              /* add LSB, set bit 7   */
    }
    for (;;)
    {
        m_data.push_back(util::byte(buffer) & 0xFF);    /* add the LSB      */
        if (buffer & 0x80)                              /* if bit 7 set     */
            buffer >>= 8;                               /* get next MSB     */
        else
            break;
    }
}

/*-------------------------------------------------------------------------
 * poke() functions
 *-------------------------------------------------------------------------*/

void
bytevector::poke_short (util::ushort x, size_t pos)
{
    poke_byte((x & 0xFF00) >> 8, pos);
    poke_byte(x & 0x00FF, ++pos);
}

void
bytevector::poke_long (util::ulong x, size_t pos)
{
    poke_byte((x & 0xFF000000) >> 24, pos);
    poke_byte((x & 0x00FF0000) >> 16, ++pos);
    poke_byte((x & 0x0000FF00) >> 8, ++pos);
    poke_byte(x & 0x000000FF, ++pos);
}

void
bytevector::poke_longlong (util::ulonglong x, size_t pos)
{
    poke_byte((x & 0xFF00000000000000) >> 56, pos);
    poke_byte((x & 0x00FF000000000000) >> 48, ++pos);
    poke_byte((x & 0x0000FF0000000000) >> 40, ++pos);
    poke_byte((x & 0x000000FF00000000) >> 32, ++pos);
    poke_byte((x & 0x00000000FF000000) >> 24, ++pos);
    poke_byte((x & 0x0000000000FF0000) >> 16, ++pos);
    poke_byte((x & 0x000000000000FF00) >> 8, ++pos);
    poke_byte(x & 0x00000000000000FF, ++pos);
}

/**
 *  Reads a file into the data vector.
 */

bool
bytevector::read (const std::string & infilename)
{
    bool result = ! infilename.empty();
    if (result)
    {
        std::ios_base::openmode m =
            std::ios::in | std::ios::binary | std::ios::ate;

        std::ifstream ifs(infilename, m);
        result = ifs.is_open();
        if (result)
        {
            size_t file_size;
            try
            {
                clear();                        /* bytes, errors, etc.      */
                (void) ifs.seekg(0, ifs.end);   /* seek to the file's end   */
                file_size = ifs.tellg();        /* get the end offset       */
                ifs.seekg(0, std::ios::beg);    /* seek to the file's start */
                m_data.resize(file_size);       /* allocate the data        */
                ifs.read((char *)(m_data.data()), file_size);
                m_nominal_size = file_size;
            }
            catch (const std::bad_alloc & ex)
            {
                result = set_error("file stream allocation failed");
            }
            catch (...)
            {
                result = set_error("file stream error");
            }
            ifs.close();
        }
        else
        {
            std::string errmsg = "Open failed: '";
            errmsg += infilename;
            errmsg += "'";
            result = set_error(errmsg);
        }
    }
    return result;
}

/**
 *  Write the whole data out to the bytevector.
 *
 * \return
 *      Returns true if the write operations succeeded.  If false is returned,
 *      then m_error_message will contain a description of the error.
 */

bool
bytevector::write (const std::string & outfilename)
{
    bool result = m_data.size() > 0;
    if (result)
    {
        std::ofstream file
        (
            outfilename.c_str(),
            std::ios::out | std::ios::binary | std::ios::trunc
        );
        if (file.is_open())
        {
            char file_buffer[c_util_line_max];  /* enable bufferization */
            file.rdbuf()->pubsetbuf(file_buffer, sizeof file_buffer);
            for (auto c : m_data)
            {
                char kc = char(c);
                file.write(&kc, 1);
                if (file.fail())
                {
                    m_error_message = "Error writing byte.";
                    result = false;
                    break;
                }
            }
        }
        else
        {
            m_error_message = "Failed to open file for writing.";
            result = false;
        }
    }
    else
        m_error_message = "No data write.";

    return result;
}

/**
 *  A function that just sets the fatal-error status and the error message.
 *
 * \param msg
 *      Provides the error message.
 *
 * \return
 *      Returns false, so that the caller can just assign this as the erroneous
 *      boolean function result.
 */

bool
bytevector::set_error (const std::string & msg) const
{
    m_error_message = msg;
    errprint(msg.c_str());
    m_error_is_fatal = true;
    m_disable_reported = true;
    return false;
}

/**
 *  Helper function to emit more useful error messages.  It adds the bytevector
 *  offset to the message.
 *
 * \param msg
 *      The main error message string, without an ending newline character.
 *
 * \return
 *      Always returns false, to make it easier on the caller.  The
 *      constructed string is returned as a side-effect (m_error_message),
 *      plus some other side-effects (m_error_is_fatal, m_disabled_reported)
 *      in case we want to pass it along to the externally-accessible
 *      error-message buffer.
 */

bool
bytevector::set_error_dump (const std::string & msg) const
{
    char temp[80];
    snprintf
    (
        temp, sizeof temp, "At 0x%zx of 0x%zx (real 0x%zx): ",
        position(), m_data.size(), real_position()
    );
    std::string result = temp;
    result += msg;
    util::msgprintf(lib66::msglevel::error, "%s", result.c_str());
    return set_error(result);
}

/**
 *  Helper function to emit more useful error messages for erroneous long
 *  values.  It adds the bytevector offset to the message.
 *
 * \param msg
 *      The main error message string, without an ending newline character.
 *
 * \param value
 *      The long value to show as part of the message.
 *
 * \return
 *      Always returns false, to make it easier on the caller.
 */

bool
bytevector::set_error_dump (const std::string & msg, unsigned long value) const
{
    char temp[64];
    snprintf(temp, sizeof temp, "; 0x%lx.", value);
    std::string result = msg;
    result += temp;
    return set_error_dump(result);
}

}           // namespace seq66

/*
 * bytevector.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

