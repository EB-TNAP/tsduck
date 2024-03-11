//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  @ingroup cpp
//!  Handle some fatal situations.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsPlatform.h"

//!
//! Build a string literal for fatal error messages.
//! @param literal 8-bit string literal
//!
#define TS_FATALMSG(literal) "\n\n*** " literal ", aborting...\n\n"

//!
//! @hideinitializer
//! Abort the application with a fatal error message.
//! @param literal 8-bit string literal error message.
//! @see ts::FatalError
//!
#define TS_FATAL(literal) ts::FatalError(TS_FATALMSG(literal), sizeof(TS_FATALMSG(literal)) - 1)

namespace ts {
    //!
    //! Handle a fatal error.
    //! An emergency message is output and the application is terminated.
    //! @param [in] message Address of an emergency error message to output.
    //! @param [in] length Length of @a message. The caller must specify @a length
    //! in a static way. In that kind of fatal error, we can't even dare to call strlen().
    //!
    [[noreturn]] TSDUCKDLL void FatalError(const char* message, size_t length);

    //!
    //! Handle fatal memory allocation failure.
    //! Out of virtual memory, very dangerous situation, really can't
    //! recover from that, need to abort immediately. An emergency error
    //! message is output and the application is terminated.
    //!
    [[noreturn]] TSDUCKDLL void FatalMemoryAllocation();

    //!
    //! Check the value of a pointer and abort the application when zero.
    //! This function is typically after a new.
    //! @param [in] ptr The pointer to check.
    //! @see FatalMemoryAllocation()
    //!
    TSDUCKDLL inline void CheckNonNull(const void* ptr)
    {
        if (ptr == nullptr) {
            FatalMemoryAllocation();
        }
    }
}
