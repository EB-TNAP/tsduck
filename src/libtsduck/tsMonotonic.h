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
//!  Basic monotonic clock & timer class
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsPlatform.h"
#include "tsException.h"

namespace ts {
    //!
    //! Basic monotonic clock & timer.
    //!
    class TSDUCKDLL Monotonic
    {
    public:
        //!
        //! Low-level monotonic clock error.
        //!
        tsDeclareException(MonotonicError);

        //!
        //! Default constructor.
        //!
        Monotonic();

        //!
        //! Copy constructor.
        //! @param [in] t Another instance to copy.
        //!
        Monotonic(const Monotonic& t);

        //!
        //! Destructor.
        //!
        ~Monotonic();

        //!
        //! Load this object with the current system time.
        //!
        void getSystemTime();

        //!
        //! Assigment operator.
        //! @param [in] t Another instance to copy.
        //! @return A reference to this object.
        //!
        Monotonic& operator=(const Monotonic& t)
        {
            _value = t._value;
            return *this;
        }

        //!
        //! Comparison operator.
        //! @param [in] t Another instance to compare.
        //! @return True if this object == @a t.
        //!
        bool operator==(const Monotonic& t) const {return _value == t._value;}

        //!
        //! Comparison operator.
        //! @param [in] t Another instance to compare.
        //! @return True if this object != @a t.
        //!
        bool operator!=(const Monotonic& t) const
        {
            return _value != t._value;
        }

        //!
        //! Comparison operator.
        //! @param [in] t Another instance to compare.
        //! @return True if this object < @a t.
        //!
        bool operator< (const Monotonic& t) const
        {
            return _value <  t._value;
        }

        //!
        //! Comparison operator.
        //! @param [in] t Another instance to compare.
        //! @return True if this object <= @a t.
        //!
        bool operator<=(const Monotonic& t) const
        {
            return _value <= t._value;
        }

        //!
        //! Comparison operator.
        //! @param [in] t Another instance to compare.
        //! @return True if this object > @a t.
        //!
        bool operator> (const Monotonic& t) const
        {
            return _value >  t._value;
        }

        //!
        //! Comparison operator.
        //! @param [in] t Another instance to compare.
        //! @return True if this object >= @a t.
        //!
        bool operator>=(const Monotonic& t) const
        {
            return _value >= t._value;
        }

        //!
        //! Increment operator.
        //! @param [in] ns A number of nanoseconds to add.
        //! @return A reference to this object.
        //!
        Monotonic& operator+=(const NanoSecond& ns)
        {
            _value += ns / NS_PER_TICK;
            return *this;
        }

        //!
        //! Decrement operator.
        //! @param [in] ns A number of nanoseconds to substract.
        //! @return A reference to this object.
        //!
        Monotonic& operator-=(const NanoSecond& ns)
        {
            _value -= ns / NS_PER_TICK;
            return *this;
        }

        //!
        //! Difference operator.
        //! @param [in] t Another instance.
        //! @return The number of nanoseconds between this object and @a t. Can be negative.
        //!
        NanoSecond operator-(const Monotonic& t) const
        {
            return (_value - t._value) * NS_PER_TICK;
        }

        //!
        //! Wait until the time of the monotonic clock.
        //!
        void wait();

        //!
        //! This static method requests a minimum resolution, in nano-seconds, for the timers.
        //! @param [in] precision Requested minimum resolution in nano-seconds.
        //! @return The guaranteed precision value (can be equal to or greater than the requested value).
        //! The default system resolution is 20 ms on Win32, which can be too long for applications.
        //!
        static NanoSecond SetPrecision(const NanoSecond& precision);

        //!
        //! This static method requests or disables the usage of the system real-time clock.
        //!
        //! This method is meaningful on Linux systems only. It is ignored on Windows and MacOS.
        //!
        //! On Linux systems, there are two kinds of clocks, normal ones and real-time ones.
        //! By default, the real-time clock is used. However, it has been noted that on some
        //! platforms (namely virtual machines), the real-time clock is broken. The symptom
        //! is usually a hang on waiting for timers. To avoid this, disable the real-time clock;
        //! the normal clock will be used.
        //!
        //! @param [in] use When false, the real-time clock is disabled.
        //!
        static void UseRealTimeClock(bool use)
        {
            _useRealTimeClock = use;
        }

        //!
        //! This static method reports if the system real-time clock is used.
        //! @return True if the system real-time clock is used.
        //! @see UseRealTimeClock()
        //!
        static bool RealTimeClockInUse()
        {
            return _useRealTimeClock;
        }

    private:
        // Monotonic clock value in system ticks
        int64_t _value;

        // Linux: use the real-time clock.
        static volatile bool _useRealTimeClock;

        // System-specific initialization
        void init();

#if defined(__windows)
        // Timer handle
        ::HANDLE _handle;

        // On Win32, a FILETIME is a 64-bit value representing the number
        // of 100-nanosecond intervals since January 1, 1601.
        // Number of nanoseconds per FILETIME ticks:
        static const int64_t NS_PER_TICK = 100;
#else
        // Jiffies per second (set by init())
        unsigned long _jps;

        // On POSIX systems, the clock unit is the nanosecond.
        static const int64_t NS_PER_TICK = 1;
#endif
    };
}
