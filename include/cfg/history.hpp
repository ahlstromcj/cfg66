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
 * \updates       2026-02-03
 * \license       GNU GPLv2 or above
 *
 *  Documented in the cpp file.
 *
 *  DEPRECATED: It has some issues when additions are made after
 *              undoes and redoes.
 */

#include <stack>                        /* std::stack<> template class      */

#include "cpp_types.hpp"                /* string, vector, opt, msglevel... */
#include "cfg/memento.hpp"              /* cfg::memento template class      */

/**
 *  Provides a safety limit for the number of undo/redo operations supported.
 *  A constructor is provided to allow a different value.
 */

const int c_default_undo_history_limit { 64 };

namespace cfg
{

class options;

/**
 *
 *  TYPE must have:
 *
 *      -   Default constructor
 *      -   Copy constructor
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
     *  Provides a type for a history-list.
     */

    using container = std::stack<memento<TYPE>>;

    /**
     *  Provides copies of the states of a TYPE. We obviously can't use
     *  references or pointers; we must keep history items around.
     *
     *  Note that a deque is a double-ended queue. It can generalize both
     *  stacks (LIFO) and queues (FIFO).
     *
     *  We could consider using std::stack(), but that would not support
     *  the redo() operation.
     */

    container m_undo_stack;
    container m_redo_stack;

    /**
     *  To avoid unintentional bloat, we limit the number of elements in the
     *  stacks.  When a push-back would increase the size beyond this value,
     *  then the earliest item is pop-fronted.
     */

    const size_t m_max_size;

    /**
     *  Keeps track of the number of undo and redo operations. For your
     *  information only.
     */

    size_t m_change_count;

    /**
     *  Holds the very first item pushed. This should be the state
     *  of the memento after the application load or save operations.
     *  It can be set in a call to reset().
     */

    memento<TYPE> m_original;

    /**
     *  Holds the current state.
     */

    memento<TYPE> m_current;

#if 0
    /**
     *  Provides the current active state.  If the maximum size has not been
     *  hit, and this value is 0, the state of the originator is "unmodified".
     *  Once the first (0) value is popped, the history cannot be unmodified.
     */

    size_t m_present;
#endif

public:

    history ();
    explicit history (size_t maximum);
    explicit history (size_t maximum, const TYPE & firstone);
    ~history () = default;
    history (const history &) = default;
    history (history &&) = delete;
    history & operator = (const history &) = default;
    history & operator = (history &&) = delete;

    void reset (const TYPE & neworiginal);
    void reset ();
    bool push_undo (const TYPE & s);
    bool undo (TYPE & s);
    bool redo (TYPE & s);

    bool active () const
    {
        return true;        /* TODO */
    }

    size_t max_size () const
    {
        return m_max_size;
    }

    size_t change_count () const
    {
        return m_change_count;
    }

    bool is_dirty () const;

protected:

    bool undoable () const
    {
        return m_undo_stack.size() > 0;
    }

    bool redoable () const
    {
        return m_redo_stack.size() > 0;
    }

};          // class history

/**
 *  Default constructor.
 */

template<typename TYPE>
history<TYPE>::history () :
    m_undo_stack    (),
    m_redo_stack    (),
    m_max_size      (c_default_undo_history_limit),
    m_change_count  (0),
    m_original      (),
    m_current       ()
{
    // no other code
}

/**
 *  Sizing constructor.
 */

template<typename TYPE>
history<TYPE>::history (size_t maximum) :
    m_undo_stack    (),
    m_redo_stack    (),
    m_max_size      (maximum),
    m_change_count  (0),
    m_original      (),
    m_current       ()
{
    // no other code
}

template<typename TYPE>
history<TYPE>::history (size_t maximum, const TYPE & firstone) :
    m_undo_stack    (),
    m_redo_stack    (),
    m_max_size      (maximum),
    m_change_count  (0),
    m_original      (),
    m_current       ()
{
    (void) push_undo(firstone);
}

/**
 *  Clears the undo/redo status, emptying the history list.  This function
 *  is useful when a "Save" or "OK" button is pressed.  True is returned
 *  if there was anything to clear.
 */

template<typename TYPE>
void
history<TYPE>::reset ()
{
    static memento<TYPE> s_dummy;
    m_change_count  = 0;
    m_original      = s_dummy;
    m_current       = s_dummy;
    m_undo_stack.clear();
    m_redo_stack.clear();
}

template<typename TYPE>
void
history<TYPE>::reset (const TYPE & neworiginal)
{
    reset();
    m_original      = neworiginal;
    m_current       = neworiginal;
}

template<typename TYPE>
bool
history<TYPE>::push_undo (const TYPE & s)               /* add()            */
{
    bool result { m_undo_stack.size() < max_size() };
    if (result)
    {
        memento<TYPE> m { s };
        m_undo_stack.push(m);                           /* emplace() ?      */
    }
    return result;
}

template<typename TYPE>
bool
history<TYPE>::undo (TYPE & s)                          /* pop_undo()       */
{
    bool result { m_undo_stack.size() > 0 };            /* undoable()       */
    if (result)
    {
        result = m_redo_stack.size() < max_size();
        if (result)
        {
            memento<TYPE> m1 { s };
            m_redo_stack.push(m1);

            memento<TYPE> m2 { m_undo_stack.top() };
            m_undo_stack.pop();
            s = m2.get_state();
        }
    }
    return result;
}

template<typename TYPE>
bool
history<TYPE>::redo (TYPE & s)                          /* pop_redo()       */
{
    bool result { m_redo_stack.size() > 0 };            /* redoable()       */
    if (result)
    {
        result = m_undo_stack.size() < max_size();
        if (result)
        {
            memento<TYPE> m1 { s };
            m_undo_stack.push(m1);

            memento<TYPE> m2 { m_redo_stack.top() };
            m_redo_stack.pop();
            s = m2.get_state();
        }
    }
    return result;
}

template<typename TYPE>
bool
history<TYPE>::is_dirty () const
{
    bool result { m_change_count > 0 };
    if (result)
    {
    }
    return result;
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

