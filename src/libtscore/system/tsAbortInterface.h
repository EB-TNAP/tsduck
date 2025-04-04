//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Abstract interface for abort polling.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsPlatform.h"

namespace ts {
    //!
    //! An interface to check for abort.
    //! @ingroup libtscore system
    //!
    //! This interface is implemented by classes which support interrupt and abort,
    //! for instance a Ctrl-C user interrupt.
    //!
    class TSCOREDLL AbortInterface
    {
        TS_INTERFACE(AbortInterface);
    public:
        //!
        //! This abstract method checks if the application is aborting for some reason.
        //! @return True if the application is aborting.
        //!
        virtual bool aborting() const = 0;
    };
}
