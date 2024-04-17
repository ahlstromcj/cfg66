#if ! defined CFG66_CFG_HISTORY_HPP
#define CFG66_CFG_HISTORY_HPP

/*
 *  This file is part of cfg66.
 *
 *  cfg66 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  cfg66 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with cfg66; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          history.hpp
 *
 *  This module summarizes or defines an undo/redo mechanism.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-01-06
 * \updates       2023-07-30
 * \license       GNU GPLv2 or above
 *
 *  Documented in the cpp file.
 */

#include <deque>                        /* std::deque<> template class      */

#include "cpp_types.hpp"                /* string, vector, opt, msglevel... */
#include "cfg/memento.hpp"              /* cfg::memento template class      */

/*
 * Do not attempt to Doxygenate the documentation here; it breaks Doxygen.
 */

namespace cfg
{

class options;

/**
 *
 *  TYPE must have:
 *
 *      -   Default constructor
 *      -   Copy construction
 *      -   Principal assignment operator
 */

template <typename TYPE>
class history
{
    /*
     * friend class originator;
     */

private:

    /**
     *  Provides a copy of a state.  We obviously can't use references.
     */

    std::deque<memento<TYPE>> m_history_list;

    /**
     *  To avoid unintentional bloat, we limit the number of elements in the
     *  deque.  When a push-back would increase the size beyond this value,
     *  then the earliest item is pop-fronted.
     */

    const size_t m_max_size;

    /**
     *  Provides the current active state.  If the maximum size has not been
     *  hit, and this value is 0, the state of the originator is "unmodified".
     *  Once the first (0) value is popped, the history cannot be unmodified.
     */

    size_t m_present;

public:

    history ();
    explicit history (size_t maximum);
    explicit history (size_t maximum, const TYPE & firstone);
    ~history () = default;
    history (const history &) = default;
    history & operator = (const history &) = default;

    bool active () const
    {
        return m_history_list.size() > 0;
    }

    size_t max_size () const
    {
        return m_max_size;
    }

    size_t size () const
    {
        return m_history_list.size();
    }

    int present () const
    {
        return int(m_present);
    }

    const TYPE & get (size_t index) const;
    const TYPE & get_present () const;

    bool undoable () const
    {
        return active() && m_present > 0;
    }

    bool redoable () const
    {
        return active() && m_present < (m_history_list.size() - 1);
    }

    bool add (const TYPE & s)
    {
        memento<TYPE> m{s};
        return push(m);
    }

    bool reset ();

    const TYPE & undo ();
    const TYPE & redo ();

protected:

    bool impl_undo (memento<TYPE> & m);
    bool impl_redo (memento<TYPE> & m);
    bool push (const memento<TYPE> & m);
    bool pop ();

    bool remove ()
    {
        return pop;
    }

};          // class history

/**
 *  Default constructor.
 */

template<typename TYPE>
history<TYPE>::history () :
    m_history_list  (),
    m_max_size      (32),
    m_present       (0)
{
    // no other code
}

/**
 *  Sizing constructor.
 */

template<typename TYPE>
history<TYPE>::history (size_t maximum) :
    m_history_list  (),
    m_max_size      (maximum),
    m_present       (0)
{
    // no other code
}

template<typename TYPE>
history<TYPE>::history (size_t maximum, const TYPE & firstone) :
    m_history_list  (),
    m_max_size      (maximum),
    m_present       (0)
{
    (void) add(firstone);
}

/**
 *  Adds a mememto to the back (end) of the history list. Returns true if
 *  no memento needed to be popped.
 */

template<typename TYPE>
bool
history<TYPE>::push (const memento<TYPE> & m)
{
    bool result = m_history_list.size() < m_max_size;
    if (! result)
        result = ! pop();               /* should never be false, though    */

    if (active())                       /* tricky code;gnore first push     */
        ++m_present;                    /* int would have been nice, but... */

    m_history_list.push_back(m);        /* copy m and add it to the deque   */
    return result;
}

/**
 *  Removes the first memento.  Returns false if there was no memento to
 *  pop.
 */

template<typename TYPE>
bool
history<TYPE>::pop ()
{
    bool result = active();
    if (result)
    {
        m_history_list.pop_front();
        --m_present;
    }
    return result;
}

/**
 *  Moves the "present" pointer toward the first entry in the history
 *  list and, if this is possible, copies the memento and returns true.
 *  Notice that this function does *not* change the history list.
 *  This allows for a redo().
 *
 * \param [out] m
 *      Contains the previous value of the TYPE object.
 *
 * \return
 *      Returns true if the item was undoable. No matter how many
 *      undo actions, the original start-up value of the TYPE object
 *      remains, if the history list didn't "overflow".
 */

template<typename TYPE>
bool
history<TYPE>::impl_undo (memento<TYPE> & m)
{
    bool result = undoable();
    if (result)
    {
        --m_present;
        m = m_history_list[m_present];
    }
    return result;
}

/**
 *  Moves the "present" pointer toward the first entry in the history
 *  list and, if this is possible, copies the memento and returns true.
 */

template<typename TYPE>
bool
history<TYPE>::impl_redo (memento<TYPE> & m)
{
    bool result = redoable();
    if (result)
    {
        ++m_present;
        m = m_history_list[m_present];
    }
    return result;
}

template<typename TYPE>
const TYPE &
history<TYPE>::undo ()
{
    static TYPE s_dummy;
    memento<TYPE> m;                    /* this is a throw-away temporary   */
    return impl_undo(m) ? m_history_list[m_present].get_state() : s_dummy ;
}

template<typename TYPE>
const TYPE &
history<TYPE>::redo ()
{
    static TYPE s_dummy;
    memento<TYPE> m;                    /* this is a throw-away temporary   */
    return impl_redo(m) ? m_history_list[m_present].get_state() : s_dummy ;
}

/**
 *  Clears the undo/redo status, emptying the history list.  This function
 *  is useful when a "Save" or "OK" button is pressed.  True is returned
 *  if there was anything to clear.
 */

template<typename TYPE>
bool
history<TYPE>::reset ()
{
    bool result = active();
    if (result)
    {
        m_history_list.clear();
        m_present = 0;
    }
    return result;
}

template<typename TYPE>
const TYPE &
history<TYPE>::get (size_t index) const
{
    static TYPE s_dummy;
    if (active())
    {
        bool ok = index < m_history_list.size();
        if (ok)
        {
            const memento<TYPE> & m = m_history_list[index];
            return m.get_state();
        }
        else
            return s_dummy;
    }
    else
        return s_dummy;
}

template<typename TYPE>
const TYPE &
history<TYPE>::get_present () const
{
    static TYPE s_dummy;
    return active() ? m_history_list[m_present] : s_dummy ;
}

/**
 *  Free functions for testing.
 */

extern std::string options_history (const history<options> & h);

}           // namespace cfg

#endif      // CFG66_CFG_HISTORY_HPP

/*
 * history.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

