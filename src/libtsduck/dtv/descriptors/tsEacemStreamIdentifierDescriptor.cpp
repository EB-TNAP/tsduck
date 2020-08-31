//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2020, Thierry Lelegard
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

#include "tsEacemStreamIdentifierDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsDuckContext.h"
#include "tsxmlElement.h"
TSDUCK_SOURCE;

#define MY_XML_NAME u"eacem_stream_identifier_descriptor"
#define MY_CLASS ts::EacemStreamIdentifierDescriptor
#define MY_DID ts::DID_EACEM_STREAM_ID
#define MY_PDS ts::PDS_EACEM
#define MY_STD ts::Standards::DVB

TS_REGISTER_DESCRIPTOR(MY_CLASS, ts::EDID::Private(MY_DID, MY_PDS), MY_XML_NAME, MY_CLASS::DisplayDescriptor);

// Incorrect use of TPS private data, TPS broadcasters should use EACEM/EICTA PDS instead.
TS_REGISTER_DESCRIPTOR(MY_CLASS, ts::EDID::Private(MY_DID, ts::PDS_TPS), MY_XML_NAME, MY_CLASS::DisplayDescriptor);


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::EacemStreamIdentifierDescriptor::EacemStreamIdentifierDescriptor(uint8_t version_) :
    AbstractDescriptor(MY_DID, MY_XML_NAME, MY_STD, MY_PDS),
    version(version_)
{
}

void ts::EacemStreamIdentifierDescriptor::clearContent()
{
    version = 0;
}

ts::EacemStreamIdentifierDescriptor::EacemStreamIdentifierDescriptor(DuckContext& duck, const Descriptor& desc) :
    AbstractDescriptor(MY_DID, MY_XML_NAME, MY_STD, MY_PDS),
    version(0)
{
    deserialize(duck, desc);
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::EacemStreamIdentifierDescriptor::serialize(DuckContext& duck, Descriptor& desc) const
{
    ByteBlockPtr bbp(serializeStart());
    bbp->appendUInt8(version);
    serializeEnd(desc, bbp);
}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::EacemStreamIdentifierDescriptor::deserialize(DuckContext& duck, const Descriptor& desc)
{
    _is_valid = desc.isValid() && desc.tag() == tag() && desc.payloadSize() == 1;

    if (_is_valid) {
        const uint8_t* data = desc.payload();
        version = data[0];
    }
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------

void ts::EacemStreamIdentifierDescriptor::DisplayDescriptor(TablesDisplay& disp, DID did, const uint8_t* data, size_t size, int indent, TID tid, PDS pds)
{
    const UString margin(indent, ' ');

    if (size >= 1) {
        uint8_t version = data[0];
        data += 1; size -= 1;
        disp << margin << "Version: " << int(version) << std::endl;
    }

    disp.displayExtraData(data, size, margin);
}


//----------------------------------------------------------------------------
// XML
//----------------------------------------------------------------------------

void ts::EacemStreamIdentifierDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    root->setIntAttribute(u"version_byte", version, true);
}

bool ts::EacemStreamIdentifierDescriptor::analyzeXML(DuckContext& duck, const xml::Element* element)
{
    return element->getIntAttribute<uint8_t>(version, u"version_byte", true, 0, 0x00, 0xFF);
}
