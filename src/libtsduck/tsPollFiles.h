//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2017, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Poll for files.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsCerrReport.h"
#include "tsEnumeration.h"
#include "tsSafePtr.h"
#include "tsTime.h"

namespace ts {
    //!
    //! Description of a polled file.
    //!
    class TSDUCKDLL PolledFile
    {
    public:
        //!
        //! Status of a polled file.
        //!
        enum Status {
            MODIFIED,  //!< The file was modified since last time.
            ADDED,     //!< The file was added since last time.
            DELETED    //!< The file was deleted since last time.
        };

        //!
        //! Enumeration names for Status.
        //!
        static const ts::Enumeration StatusEnumeration;

        //!
        //! Get the file name.
        //! @return The file name.
        //!
        const UString& getFileName() const
        {
            return _name;
        }

        //!
        //! Get file status since last notification.
        //! @return The file status since last notification.
        //!
        Status getStatus() const
        {
            return _status;
        }

    private:
        friend class PollFiles;

        const UString _name;           // File name
        Status        _status;         // Status since last report
        int64_t       _file_size;      // File size in bytes
        Time          _file_date;      // Last file modification date (UTC)
        bool          _pending;        // Not yet notified, waiting for stable state
        Time          _found_date;     // First time (UTC) this size/date state was reported

        // Constructor
        PolledFile(const UString& name, const int64_t& size, const Time& date, const Time& now);

        // Check if file has changed size or date. If yes, return to pending state.
        void trackChange(const int64_t& size, const Time& date, const Time& now);
    };

    //!
    //! Safe pointer to a PolledFile (not thread-safe).
    //!
    typedef SafePtr<PolledFile, NullMutex> PolledFilePtr;

    //!
    //! List of safe pointers to PolledFile (not thread-safe).
    //!
    typedef std::list<PolledFilePtr> PolledFileList;

    //!
    //! Interface for classes listening for file modification.
    //!
    class TSDUCKDLL PollFilesListener
    {
    public:
        //!
        //! Invoked when files have changed.
        //! @param [in] files List of changed files since last time.
        //! The entries in the list are sorted by file names.
        //! @return True to continue polling, false to exit PollFiles().
        //!
        virtual bool handlePolledFiles(const PolledFileList& files) = 0;

        //!
        //! Invoked before each poll to give the opportunity to change where and how the files are polled.
        //! This is an optional feature, the default implementation does not change anything.
        //! @param [in,out] wildcard Wildcard specification of files to poll (eg "/path/to/*.dat").
        //! @param [in,out] poll_interval Interval in milliseconds between two poll operations.
        //! @param [in,out] min_stable_delay A file size needs to be stable during that duration
        //! for the file to be reported as added or modified. This prevent too frequent
        //! poll notifications when a file is being written and his size modified at each poll.
        //! @return True to continue polling, false to exit PollFiles().
        //!
        virtual bool updatePollFiles(UString& wildcard, MilliSecond& poll_interval, MilliSecond& min_stable_delay)
        {
            return true;
        }

        //!
        //! Virtual destructor.
        //!
        virtual ~PollFilesListener() {}
    };

    //!
    //! A class to poll files for modifications.
    //!
    //! To poll files for modification, we need a method like PollFiles(...).
    //! For technical reasons, we need a class. The method PollFiles() is
    //! actually a constructor, but we do not need to know...
    //!
    class TSDUCKDLL PollFiles
    {
    public:
        //!
        //! Constructor, acting as pseudo-method PollFiles(...).
        //! Invoke the listener each time something has changed.
        //! The first time, all files are reported as "added".
        //!
        //! @param [in] wildcard Wildcard specification of files to poll (eg "/path/to/*.dat").
        //! @param [in] poll_interval Interval in milliseconds between two poll operations.
        //! @param [in] min_stable_delay A file size needs to be stable during that duration
        //! for the file to be reported as added or modified. This prevent too frequent
        //! poll notifications when a file is being written and his size modified at each poll.
        //! @param [in,out] listener Invoked on file modification.
        //! @param [in,out] report For debug messages only.
        //!
        PollFiles(const UString& wildcard,
                  MilliSecond poll_interval,
                  MilliSecond min_stable_delay,
                  PollFilesListener& listener,
                  Report& report = CERR);

    private:
        UString            _files_wildcard;
        Report&            _report;
        PollFilesListener& _listener;
        PolledFileList     _polled_files;   // Updated at each poll, sorted by file name
        PolledFileList     _notified_files; // Modifications to notify

        // Mark a file as deleted, move from polled to notified files.
        void deleteFile(PolledFileList::iterator&);

        // Notify listener. Return true to continue polling, false to exit PollFiles().
        bool notifyListener();

        // Unaccessible operations
        PollFiles() = delete;
        PollFiles(const PollFiles&) = delete;
    };
}
