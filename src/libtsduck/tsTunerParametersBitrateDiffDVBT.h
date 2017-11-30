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
//!  A variant of DVB-T tuners parameters with an bitrate offset.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsTunerParametersDVBT.h"

namespace ts {

    class TunerParametersBitrateDiffDVBT;

    //!
    //! List of TunerParametersBitrateDiffDVBT.
    //!
    typedef std::list<TunerParametersBitrateDiffDVBT> TunerParametersBitrateDiffDVBTList;

    //!
    //! A variant of DVB-T tuners parameters with an offset between a target bitrate and their theoretical bitrate.
    //!
    class TSDUCKDLL TunerParametersBitrateDiffDVBT: public TunerParametersDVBT
    {
    public:
        //!
        //! Difference between a target bitrate and the theoretial bitrate for these tuner parameters.
        //!
        int bitrate_diff;

        //!
        //! Default constructor.
        //!
        TunerParametersBitrateDiffDVBT();

        //!
        //! Comparison operator for list sort.
        //! Sort criterion: increasing order of absolute value of bitrate_diff.
        //! @param [in] other Other instance to compare.
        //! @return True if this object is logically less than @a other.
        //!
        bool operator<(const TunerParametersBitrateDiffDVBT& other) const;

        //!
        //! Build a list of all possible combinations of DVB-T parameters for a target bitrate.
        //! @param [out] params List of all possible combinations of bandwidth, constellation,
        //! guard interval and high-priority FEC, sorted in increasing order of bitrate
        //! difference from a @a bitrate.
        //! @param [in] bitrate Target bitrate in bits/second.
        //!
        static void EvaluateToBitrate(TunerParametersBitrateDiffDVBTList& params, BitRate bitrate);

        // Reimplemented from TunerParametersDVBT
        virtual void copy(const TunerParameters&) override;
    };
}
