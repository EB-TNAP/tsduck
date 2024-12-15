//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsISPAccessModeDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsPSIBuffer.h"
#include "tsDuckContext.h"
#include "tsxmlElement.h"

#define MY_XML_NAME u"ISP_access_mode_descriptor"
#define MY_CLASS    ts::ISPAccessModeDescriptor
#define MY_EDID     ts::EDID::TableSpecific(ts::DID_INT_ISP_ACCESS, ts::Standards::DVB, ts::TID_INT)

TS_REGISTER_DESCRIPTOR(MY_CLASS, MY_EDID, MY_XML_NAME, MY_CLASS::DisplayDescriptor);

namespace {
    const ts::Enumeration AccessModeNames({
        {u"unused", 0},
        {u"dialup", 1},
    });
}

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::ISPAccessModeDescriptor::ISPAccessModeDescriptor(uint8_t mode) :
    AbstractDescriptor(MY_EDID, MY_XML_NAME),
    access_mode(mode)
{
}

ts::ISPAccessModeDescriptor::ISPAccessModeDescriptor(DuckContext& duck, const Descriptor& desc) :
    ISPAccessModeDescriptor()
{
    deserialize(duck, desc);
}

void ts::ISPAccessModeDescriptor::clearContent()
{
    access_mode = 0;
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::ISPAccessModeDescriptor::serializePayload(PSIBuffer& buf) const
{
    buf.putUInt8(access_mode);
}

void ts::ISPAccessModeDescriptor::deserializePayload(PSIBuffer& buf)
{
    access_mode = buf.getUInt8();
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------

void ts::ISPAccessModeDescriptor::DisplayDescriptor(TablesDisplay& disp, const ts::Descriptor& desc, PSIBuffer& buf, const UString& margin, const ts::DescriptorContext& context)
{
    if (buf.canReadBytes(1)) {
        const uint8_t mode = buf.getUInt8();
        disp << margin << UString::Format(u"Access mode: 0x%X (%s)", mode, AccessModeNames.name(mode)) << std::endl;
    }
}


//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::ISPAccessModeDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    root->setIntEnumAttribute(AccessModeNames, u"access_mode", access_mode);
}

bool ts::ISPAccessModeDescriptor::analyzeXML(DuckContext& duck, const xml::Element* element)
{
    return element->getIntEnumAttribute(access_mode, AccessModeNames, u"access_mode", true);
}
