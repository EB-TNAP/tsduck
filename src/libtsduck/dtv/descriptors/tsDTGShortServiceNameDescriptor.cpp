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

#include "tsDTGShortServiceNameDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsDuckContext.h"
#include "tsxmlElement.h"
TSDUCK_SOURCE;

#define MY_XML_NAME u"dtg_short_service_name_descriptor"
#define MY_CLASS ts::DTGShortServiceNameDescriptor
#define MY_DID ts::DID_OFCOM_SHORT_SRV_NAM
#define MY_PDS ts::PDS_OFCOM
#define MY_STD ts::Standards::DVB

TS_REGISTER_DESCRIPTOR(MY_CLASS, ts::EDID::Private(MY_DID, MY_PDS), MY_XML_NAME, MY_CLASS::DisplayDescriptor);


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::DTGShortServiceNameDescriptor::DTGShortServiceNameDescriptor(const UString& name_) :
    AbstractDescriptor(MY_DID, MY_XML_NAME, MY_STD, MY_PDS),
    name(name_)
{
}

ts::DTGShortServiceNameDescriptor::DTGShortServiceNameDescriptor(DuckContext& duck, const Descriptor& desc) :
    DTGShortServiceNameDescriptor()
{
    deserialize(duck, desc);
}

void ts::DTGShortServiceNameDescriptor::clearContent()
{
    name.clear();
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::DTGShortServiceNameDescriptor::serialize(DuckContext& duck, Descriptor& desc) const
{
    ByteBlockPtr bbp(serializeStart());
    bbp->append(duck.encoded(name));
    serializeEnd(desc, bbp);
}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::DTGShortServiceNameDescriptor::deserialize(DuckContext& duck, const Descriptor& desc)
{
    _is_valid = desc.isValid() && desc.tag() == tag();

    if (_is_valid) {
        duck.decode(name, desc.payload(), desc.payloadSize());
    }
    else {
        name.clear();
    }
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------

void ts::DTGShortServiceNameDescriptor::DisplayDescriptor(TablesDisplay& disp, DID did, const uint8_t* payload, size_t size, int indent, TID tid, PDS pds)
{
    const UString margin(indent, ' ');
    disp << margin << "Name: \"" << disp.duck().decoded(payload, size) << "\"" << std::endl;
}


//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::DTGShortServiceNameDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    root->setAttribute(u"name", name);
}


//----------------------------------------------------------------------------
// XML deserialization
//----------------------------------------------------------------------------

bool ts::DTGShortServiceNameDescriptor::analyzeXML(DuckContext& duck, const xml::Element* element)
{
    return element->getAttribute(name, u"name", true, u"", 0, MAX_DESCRIPTOR_SIZE - 2);
}
