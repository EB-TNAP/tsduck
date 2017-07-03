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
//!  Abstract base class for MPEG PSI/SI descriptors
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsMPEG.h"

namespace ts {

    class Descriptor;
    class DescriptorList;
    class TablesDisplay;

    //!
    //! Abstract base class for MPEG PSI/SI descriptors.
    //!
    class TSDUCKDLL AbstractDescriptor
    {
    public:
        //!
        //! Check if the descriptor is valid.
        //! @return True if the descriptor object is valid, false otherwise.
        //!
        bool isValid() const {return _is_valid;}

        //!
        //! Invalidate the descriptor.
        //! The object must be rebuilt.
        //!
        void invalidate() {_is_valid = false;}

        //!
        //! Get the descriptor tag.
        //! @return The descriptor tag.
        //!
        DID tag() const {return _tag;}

        //!
        //! This abstract method serializes a descriptor.
        //! @param [out] bin A binary descriptor object.
        //! Its content is replaced with a binary representation of this object.
        //!
        virtual void serialize(Descriptor& bin) const = 0;

        //!
        //! This abstract method deserializes a binary descriptor.
        //! In case of success, this object is replaced with the interpreted content of @a bin.
        //! In case of error, this object is invalidated.
        //! @param [in] bin A binary descriptor to interpret according to the descriptor subclass.
        //!
        virtual void deserialize(const Descriptor& bin) = 0;

        //!
        //! Deserialize a descriptor from a descriptor list.
        //! In case of success, this object is replaced with the interpreted content of the binary descriptor.
        //! In case of error, this object is invalidated.
        //! @param [in] dlist A list of binary descriptors.
        //! @param [in] index Index of the descriptor to deserialize in @a dlist.
        //!
        void deserialize(const DescriptorList& dlist, size_t index);

        //!
        //! Virtual destructor
        //!
        virtual ~AbstractDescriptor () {}

        //!
        //! Profile of a function to display a descriptor.
        //! Each subclass should provide a static function named @e DisplayDescriptor
        //! which displays a descriptor of its type.
        //!
        //! @param [in,out] display Display engine.
        //! @param [in] did Descriptor id.
        //! @param [in] payload Address of the descriptor payload.
        //! @param [in] size Size in bytes of the descriptor payload.
        //! @param [in] indent Indentation width.
        //! @param [in] tid Table id of table containing the descriptors.
        //! This is optional. Used by some descriptors the interpretation of which may
        //! vary depending on the table that they are in.
        //! @param [in] pds Private Data Specifier. Used to interpret private descriptors.
        //!
        typedef void (*DisplayDescriptorFunction)(TablesDisplay& display,
                                                  DID did,
                                                  const uint8_t* payload,
                                                  size_t size,
                                                  int indent,
                                                  TID tid,
                                                  PDS pds);

    protected:
        //!
        //! The descriptor tag can be modified by subclasses only
        //!
        DID _tag;

        //!
        //! It is the responsibility of the subclasses to set the valid flag
        //!
        bool _is_valid;

        //!
        //! Protected constructor for subclasses.
        //! @param [in] tag Descriptor tag.
        //!
        AbstractDescriptor(DID tag) : _tag (tag), _is_valid (false) {}

    private:
        // Unreachable constructors and operators.
        AbstractDescriptor() = delete;
    };

    //!
    //! Safe pointer for AbstractDescriptor (not thread-safe).
    //!
    typedef SafePtr <AbstractDescriptor, NullMutex> AbstractDescriptorPtr;
}
