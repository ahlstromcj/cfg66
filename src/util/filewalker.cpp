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
 * \file          filewalker.cpp
 *
 *      This module uses the <filesystem> functions to traverse a
 *      directory hierarchy and do a few things with it.
 *
 * \library       filewalker
 * \author        Chris Ahlstrom
 * \date          2026-04-02
 * \updates       2026-04-03
 * \version       $Revision$
 * \license       GNU GPL v2 or above
 *
 *      This module defines the util::filewalker class to manage some aspects
 *      of file-tree walking, as well as a few free functions in the
 *      util and anonymous namespaces.
 *
 *      Implementing the GNU File Tree Walk (FTS) API using std::filesystem
 *      involves using std::filesystem::recursive_directory_iterator to
 *      emulate the fts_open(), fts_read(), and fts_close() paradigm,
 *      to provide high-level directory traversal.
 *
 *      -   fts_open() / fts_read() are replaced by
 *          std::filesystem::recursive_directory_iterator.
 *      -   FTSENT * (entry info) is replaced by
 *          std::filesystem::directory_entry (accessed via *iterator).
 *      -   Depth Information is accessed via iterator.depth().
 *      -   Error Handling is managed using
 *          std::filesystem::filesystem_error exceptions.
 *      -   Use std::filesystem::path for portable paths.
 *      -   Use symlink_options in the iterator constructor to mimic
 *          FTS_LOGICAL or FTS_PHYSICAL.
 *      -   Requires C++17 or above.
 */

// #include <cerrno>                    /* #include <errno.h>               */
// #include <cstring>                   /* std::strerror()                  */
// #include <cstdlib>                   /* std::getenv(), std::rand()       */

#include <iostream>                     /* std::cout, std::cerr             */

#include "util/filewalker.hpp"          /* file-tree traversal declarations */
#include "util/filefunctions.hpp"       /* cfg66: util::file_write_lines()  */
#include "util/msgfunctions.hpp"        /* cfg66: util::error_message() etc */
#include "util/strfunctions.hpp"        /* cfg66: util::strcompare() etc    */

/**
 *  file_status (const path & p (string_type && s, format = auto_format))
 *
 *      where string_type is a C-type string, a basic_string, a
 *      basic_string_view, or a pair of iterators.
 *
 * FTS review:
 *
 *      FTS_D       A directory being visited in preorder.
 *      FTS_DC      A directory that causes a cycle in the tree.
 *      FTS_DEFAULT An FTSENT that represents an "other type" of file.
 *      FTS_DNR     A directory which cannot be read. This is an error return.
 *      FTS_DOT     A file named "."  or ".."  not specified in fts_open().
 *      FTS_DP      A directory being visited in postorder.
 *      FTS_ERR     An error return, fts_errno set.
 *      FTS_F       A regular file.
 *      FTS_NS      A file with no stat(2) information available; an error.
 *      FTS_NSOK    A file for which no stat(2) information was requested.
 *      FTS_SL      A symbolic link.
 *      FTS_SLNONE Symbolic link with a nonexistent target.
 *
 */

namespace util
{

/**
 *  Principal constructor.
 */

filewalker::filewalker (const std::string & path) :
    m_search_directories    ()
{
    m_search_directories.push_back(path);
}

filewalker::filewalker (const lib66::tokenization & paths) :
    m_search_directories    (paths)
{
    // no code
}

/**
 *  A generic search to build a list of locations for the target file.
 *  This function searches the path(s) provided during the construction
 *  of this object.
 *
 *  The "compar()" argument is NULL, therefore the directory traversal order
 *  is in the order listed in the root paths parameter, and in the order
 *  listed in the directory for everything else.
 *
 * \param target
 *      Provides the name of the file for which we're searching.
 *
 * \param [out] destination
 *      A vector of strings to hold the full path names of the files
 *      found that matched the target. This item is not cleared,
 *      so one theoretically do a number of searches.
 *
 * \return
 *      Returns true if no error occurred and any files were found.
 *      That is, the destination results can be used.
 */

bool
filewalker::find_file
(
    const std::string & target,
    lib66::tokenization & destination
)
{
    (void) destination;

    bool result { ! target.empty() };
    if (result)
    {
        // TODO
    }
    return result;
}

/**
 *  This function searches the path(s) provided during the construction
 *  of this object. Unlike find_file() above, it does not look for a
 *  specific and collect the occurrences; instead it gets all the regular
 *  files in the directory and collects them.
 *
 * \param [out] destination
 *      A vector of strings to hold the full path names of the files
 *      found that are regular files. This item is not cleared,
 *      so one theoretically do a serial number of searches.
 *
 * \return
 *      Returns true if no error occurred and any files were found.
 *      That is, the destination results can be used.
 */

bool
filewalker::find_regular_files (lib66::tokenization & destination)
{
    (void) destination;

    bool result { true };
    if (result)
    {
        // TODO
    }
    return result;
}

/**
 *  The traverse() function simply shows the files and type in the
 *  given path.
 *
 *  The recursive iterator handles the "depth-first" traversal. The type
 *  of each item iterated is std::filesystem::directory_entry, similar
 *  to FTSENT.
 */

bool
filewalker::traverse (const std::string & path) const
{
    bool result { std::filesystem::exists(path) };
    if (result)
    {
        try
        {
            /*
             * We must use an iterator to get the depth(). Feh!
             * Note the tricky use of a free end() function.
             */

            for
            (
                auto it = std::filesystem::recursive_directory_iterator(path);
                it != std::filesystem::end(it);
                ++it
            )
            {
                /*
                 * Get file attributes similar to fts_ent.
                 */

                const auto & entry = *it;
                std::cout
                    << std::string(it.depth() * 2, ' ')     // indent by depth
                    << (entry.is_directory() ? "[D] " : "[F] ")
                    << entry.path().filename().string()
                    << std::endl
                    ;
            }
        }
        catch (const std::filesystem::filesystem_error & e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            result = false;
        }
    }
    return result;
}

/**
 *  As all the directories and files are traversed, this function calls
 *  the given filewalker function.
 *
 * \param fn
 *      Provides the free function or static function to be called for
 *      every entry. It should at least handle the values in the
 *      filewalker::FTS enumeration.
 *
 *          function = bool (*) (const std::string &, FTS);
 *
 * \param target
 *      If not empty, then only matching files will be processed.
 *      The default is empty. NEW: the file base ("xyz.txt") is no longer
 *      extracted for the match. Use regular expressions, as
 *      described in util::regex_match() in the strfunctions module.
 *
 * \param cfn
 *      If not null (the default), this function is used in ordering the
 *      traversal. By default, the directory traversal order is in the
 *      order listed in the root paths, and in the order listed in the
 *      directory for everything else.
 *
 * \return
 *      If all the calls to \a fn return true, this function returns true.
 */

bool
filewalker::process_files
(
    function fn,
    const std::string & rgx,
    comparator cfn
)
{
    (void) fn;
    (void) rgx;
    (void) cfn;

    bool result { not_nullptr(fn) };
    if (result)
    {
        // TODO
    }
    return result;
}

/**
 *  Here is how the processing works. The path starts at "tests/data/fts",
 *  and the target is "build/tests" == $DEST:
 *
 *      1.  Get the current entry. Four examples are shown:
 *
 *          a.  "tests/data/fts"
 *          b.  "tests/data/fts/session.fts"
 *          c.  "tests/data/fts/session_2"
 *          d.  "tests/data/fts/session_2/session.fts"
 *
 *      2.  Assemble the destinations as encountered in "compare files
 *          before directories" order:
 *
 *          a.  "$DEST/fts". The source, "tests/data/fts" is a directory.
 *
 *              i.  Get the file-name base "fts" from the source by a
 *                  split with the "no-extension" option.
 *             ii.  Append it to $DEST.
 *            iii.  Create the directory.
 *             iv.  If successful, store "$DEST/fst" as the last-found
 *                  destination, $LASTDEST. This is done in process_file().
 *
 *          b.  "$DEST/fts/session.fts". This will be a file.
 *
 *              i. Split out the source path and file-name ("session.fts").
 *             ii. Verify that the destination directory ($LASTDEST) exists.
 *            iii.  Copy or process the file to the destination.
 *
 *          c.  "$DEST/fts/session_2". The source is a directory,
 *              "tests/data/fts/session_2".
 *
 *              i.  Get the file-name base "session_2" from the source by a
 *                  split with the "no-extension" option.
 *             ii.  Append it to $LASTDEST.
 *            iii.  Create the directory.
 *             iv.  If successful, store "$DEST/fst/session_2" as $LASTDEST.
 *
 *          d.  "$DEST/fts/session_2/session.fts". This will be a file.
 *
 *              i.  Split out the path and the filename.
 *             ii. Verify that the destination directory ($LASTDEST) exists.
 *            iii.  Copy the file to the destination.
 *
 * \param fn
 *      The function that takes the source file/directory as retrieved
 *      by fts_read_entry() and the target directory and does some cross
 *      processing, such as copying. Remember that the source directory
 *      comes from the path_ptrs() accessor.
 *
 * \param target
 *      The destination for the processed file. An example would be
 *      "build/tests".
 *
 * \param cfn
 *      If not null (the default), this function is used in ordering the
 *      traversal. By default, the directory traversal order is in the
 *      order listed in the root paths, and in the order listed in the
 *      directory for everything else.
 *
 * \return
 *      Returns true if no errors occurred.
 */

bool
filewalker::process_bi_files
(
    bifunction fn,
    const std::string & source,
    const std::string & dest,
    comparator cfn
)
{
    bool result
    {
        not_nullptr(fn) && util::file_is_directory(source)
    };
    if (result)
    {
        (void) dest;
        (void) cfn;
    }
    return result;
}

/*-------------------------------------------------------------------------
 * Free functions in the util::file namespace
 *-------------------------------------------------------------------------*/

namespace file
{

/**
 *  The argument compare_whatever() specifies a user-defined function
 *  which can be used to order the traversal of the hierarchy.
 *
 *  It return ones of the following values to indicate if the file
 *  referenced by its first argument comes before, in any order with respect
 *  to, or after, the file referenced by its second argument.
 *
 *      -   A negative value. First comes before second.
 *      -   Zero. First and second aren't ordered.
 *      -   A positive value. First comes after second.
 *
 *  If the compare_whatever() argument is NULL, the directory traversal order
 *  is in the order listed in path_argv for the root paths, and in the order
 *  listed in the directory for everything else.
 *
 *      return strcmp((*first)->fts_name, (*second)->fts_name);
 */

int
compare_files_before_dirs
(
    const std::filesystem::directory_entry & afirst,
    const std::filesystem::directory_entry & asecond
)
{
    if (afirst.is_regular_file())
    {
        return (-1);                                    /* first    */
    }
    else if (asecond.is_regular_file())
    {
        return 1;                                       /* last     */
    }
    else
    {
        const std::filesystem::path & p1 { afirst.path() };
        const std::filesystem::path & p2 { asecond.path() };
        return p1.string().compare(p2.string());
    }
}

/**
 *  Just a simple test callback function; see tests/filewalker_test.
 *  Must apply the --verbose flag to see the output.
 */

bool
show_target
(
    const std::string & match,          // util::filewalker::FTS ft
    std::filesystem::file_type ft
)
{
    std::string t { get_type_name(ft) };
    std::string m { match.empty() ? "---" : match };
    if (t.empty())
        t = "?";                        /* this should not happen           */

    util::info_message(t, m);           /* show message if --verbose        */
    return true;
}

bool
item_copy
(
    const std::string & source,
    const std::string & target,
    std::filesystem::file_type ft
)
{
    bool result { true };                               /* no action ok     */
    if (ft == std::filesystem::file_type::directory)    /* 1st directory    */
    {
        util::info_message("Entered directory", source);
        result = util::make_directory_path(target);
    }
    else if (ft == std::filesystem::file_type::regular) /* regular file     */
    {
        util::info_printf("Copying %s to %s", V(source), V(target));
        result = util::file_copy_to_path(source, target);
    }
    else if (ft == std::filesystem::file_type::symlink) /* a symbolic link  */
    {
        util::info_printf("Copying %s to %s", V(source), V(target));
        result = util::file_copy_to_path(source, target);
    }
#if 0
    else if (ft == std::filesystem::file_type::symlink) /* link w/no target */
    {
        util::info_message("Empty soft link", source);
    }
    else if (ft == std::filesystem::file_type::directory) /* last directory */
    {
        // nothing yet
    }
#endif
    return result;
}

/**
 *  An ftswalker::function callback for deleting a file, directory,
 *  and directory hierarchy.
 *
 *  Note that a directory cannot be deleted until all its children
 *  are deleted, which is done once the file type is FTS::DP.
 */

bool
item_delete (const std::string & item, std::filesystem::file_type ft)
{
    bool result { file_exists(item) };
    if (result)
    {
        if (ft == std::filesystem::file_type::directory) /* 1st directory   */
        {
            /*
             * TMI: util::info_message("Entered directory", item);
             */
        }
#if 0
        else if (ft == std::filesystem::file_type::directory) /* last dir   */
        {
            util::info_message("Deleting directory", item);
            result = util::delete_directory(item);
        }
        else if (ft == util::ftswalker::FTS::SLNONE)    /* link w/no target */
        {
            util::info_message("Deleting ophan link", item);
            result = util::file_delete(item);
        }
#endif
        else if (ft == std::filesystem::file_type::regular) /* regular file */
        {
            util::info_message("Deleting", item);
            result = util::file_delete(item);
        }
        else if (ft == std::filesystem::file_type::symlink) /* soft link    */
        {
            // TODO
        }
    }
    return result;
}

/**
 *  Copies a directory hierarchy to another directory. See the
 *  copy_test() function in the filewalker_test program
 *  for more explanation.
 */

bool
copy_directory (const std::string & source, const std::string & dest)
{
    bool result { file_is_directory(source) && file_is_directory(dest) };
    if (result)
    {
        util::filewalker walker(source);
        result = walker.process_bi_files
        (
            item_copy, source, dest // , util::compare_files_before_dirs
        );
    }
    return result;
}

/**
 *  This function removes the files and directories it encounters, including
 *  sub-directories.
 *
 *  In general, directories are visited two distinguishable times; in
 *  preorder (before any of their descendants are visited) and in
 *  postorder (after all of their descendants have been visited).
 *  Files are visited once.  It is possible to walk the hierarchy
 *  "logically" (visiting the files that symbolic links point to) or
 *  physically (visiting the symbolic links themselves), order the
 *  walk of the hierarchy or prune and/or revisit portions of the
 *  hierarchy.
 *
 *  Note that this function is not a callback.
 */

bool
delete_directory (const std::string & path)
{
    bool result { file_is_directory(path) };
    if (result)
    {
        util::filewalker walker(path);
        result = walker.process_files
        (
            item_delete, path // , std::filesystem::file_type::directory
        );
    }
    return result;
}

bool
find_file
(
    const std::string & rootdir,
    const std::string & target
)
{
    bool result { ! rootdir.empty() && ! target.empty() };

    return result;
}

/**
 *  Inspired by a similarly-named function in the Ardour source code.
 *  However, it does not directly use a searchpath; instead, created the
 *  desired search and use it as noted below.
 *
 *  It is a more hardwired version of filewalker::process_files(). It
 *  does not call an filewalker::function. Instead, it just copies
 *  path to the 'collected' parameter.
 *
 *  We'll depend on regex to filter regular files
 *  versus directories. Compare to item_copy().
 *
 *          FTS ft = get_fts_type(ent);
 *          result = fn(p, ft);
 *          if (! result) break;
 *
 *  Another option is bind an filewalker::function to a non-auto
 *  lib66::tokenization object into which the findings are pushed.
 *
 * \param [out] collected
 *      Holds the files that were found. Use it if true is returned.
 *
 * \param paths
 *      Holds all of the paths to be searched. The searchpath class
 *      can be used to get all of the paths in a "PATH"-type variable,
 *      and then the searchpath::paths() function can be used for
 *      this parameter.
 *
 * \param rgx
 *      Holds a regular expression, which must be valid, to define the
 *      objects of the search. If a glob is needed, first convert it to
 *      a regex via the util::glob_to_regex() function in the strfunctions
 *      module.
 *
 * \return
 *      Returns true if no errors occurred and at least one
 *      file was found.
 */

bool
find_files_by_regex
(
    lib66::tokenization & collected,
    const lib66::tokenization & paths,
    const std::string & rgx,
    filewalker::comparator cfn
)
{
    (void) collected;
    (void) cfn;

    int count { int(paths.size()) };
    bool result { count > 0  && ! rgx.empty() };
    if (result)
    {
        // TODO
    }
    return result;
}

/**
 *  Gets a brief description of an <filesystem> file type.
 */

std::string
get_type_name (std::filesystem::file_type ft)
{
    std::string result;
    switch (ft)
    {
        case std::filesystem::file_type::none:

            result = "Undetermined/error";
            break;

        case std::filesystem::file_type::not_found:

            result = "File not found";
            break;

        case std::filesystem::file_type::regular:

            result = "Regular file";
            break;

        case std::filesystem::file_type::directory:

            result = "Directory";
            break;

        case std::filesystem::file_type::symlink:

            result = "Symbolic link";
            break;

        case std::filesystem::file_type::block:

            result = "Block device";
            break;

        case std::filesystem::file_type::character:

            result = "Character device";
            break;

        case std::filesystem::file_type::fifo:

            result = "FIFO/pipe";
            break;

        case std::filesystem::file_type::socket:

            result = "Socket";
            break;

        case std::filesystem::file_type::unknown:

            result = "Unknown file type";
            break;

#if 0
        case util::filewalker::FTS::SLNONE:
            result = "Untargeted symbolic link";
        case util::filewalker::FTS::NSOK:
            result = "No stat request";
        case util::filewalker::FTS::DC:
            result = "Directory cycle";
        case util::filewalker::FTS::DOT:
            result = "Dot file";
        case util::filewalker::FTS::DP:
            result = "Postorder directory";
        case util::filewalker::FTS::DNR:
            result = "Directory read error";
        case util::filewalker::FTS::ERR:
            result = "Error";
        case util::filewalker::FTS::NS:
            result = "No-stat error";
#endif

        default:

            result = "Unknown";
            break;
    }
    return result;
}

/*--------------------------------------------------------------------------
 * Free functions in the util::file namespace
 *--------------------------------------------------------------------------*/

#if 0

/**
 *  Indicates that the file-type has not yet been evaluated, or that
 *  an error occurred when evaluating it.
 */

bool
is_none (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::none;
}

bool
is_none (std::filesystem::file_status s)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_none(s);
}

/**
 *  Not finding a file is not an error.
 */

bool
is_not_found (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::not_found;
}

bool
is_not_found (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_not_found(s);
}

/**
 *  A normal, regular file, found on "disk".
 */

bool
is_regular_file (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::regular;
}

bool
is_regular_file (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_regular_file(s);
}

/**
 *  A directory, what inferior operating systems call a "folder" :-)
 */

bool
is_directory (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::directory;
}

bool
is_directory (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_directory(s);
}

/**
 *  A symbolic link. Probably doesn't distinguish a hard link
 *  versus a soft link.
 */

bool
is_symlink (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::symlink;
}

bool
is_symlink (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_symlink(s);
}

/**
 *
 *  A block special file, e.g. a block device.
 */

bool
is_block (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::block;
}

bool
is_block (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_block(s);
}

/**
 *
 *  A character special file, e.g. a character device.
 */

bool
is_character (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::character;
}

bool
is_character (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_character(s);
}

/**
 *  A FIFO, also known as a pipe.
 */

bool
is_fifo (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::fifo;
}

bool
is_fifo (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_fifo(s);
}

/**
 *  A socket file.
 */

bool
is_socket (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::socket;
}

bool
is_socket (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_socket(s);
}

/**
 *  The file exists, but it's type could not be determined.
 */

bool
is_unknown (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::unknown;
}

bool
is_unknown (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_unknown(s);
}

#if defined PLATFORM_WINDOWS

/**
 *  A NTFS junction file.
 */

bool
is_ntfs_junction (std::filesystem::file_status s)
{
    return s.type() == std::filesystem::file_type::junction;
}

bool
is_ntfs_junction (const std::string & pathname)
{
    std::filesystem::path p { pathname };
    std::filesystem::file_status s { p };
    return is_ntfs_junction(s);
}

bool
is_other_file_type (...)
{
    return ...
}

#endif  // defined PLATFORM_WINDOWS

#if 0

bool
is_leaving_directory (...)
{
    return ...
}

bool
is_file_error (...)
{
    return
        entry->fts_info == FTS_ERR || entry->fts_info == FTS_DNR ||
        entry->fts_info == FTS_NS
        ;
}

#endif

/**
 *  We need to allow FTS_DP to indicate all subs in the directory
 *  have been traversed.
 */

bool
is_actionable_file (const std::string & pathname)
{
    return ! is_none(pathname) && entry->fts_info != FTS_DOT;
}

#endif

}           // namespace file

}           // namespace util

/*
 * filewalker.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
