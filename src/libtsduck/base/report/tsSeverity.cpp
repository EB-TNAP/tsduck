//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsSeverity.h"
#include "tsEnumeration.h"
#include "tsUString.h"


//----------------------------------------------------------------------------
// Enumeration to use severity values on the command line for instance.
//----------------------------------------------------------------------------

const ts::Enumeration ts::Severity::Enums({
    {u"fatal",   ts::Severity::Fatal},
    {u"severe",  ts::Severity::Severe},
    {u"error",   ts::Severity::Error},
    {u"warning", ts::Severity::Warning},
    {u"info",    ts::Severity::Info},
    {u"verbose", ts::Severity::Verbose},
    {u"debug",   ts::Severity::Debug},
});


//----------------------------------------------------------------------------
// Formatted line prefix header for a severity
//----------------------------------------------------------------------------

ts::UString ts::Severity::Header(int severity)
{
    if (severity < Fatal) {
        // Invalid / undefined severity.
        return UString::Format(u"[%d] ", severity);
    }
    else if (severity > Debug) {
        return UString::Format(u"Debug[%d]: ", severity);
    }
    else {
        switch (severity) {
            case Fatal:
                return u"FATAL ERROR: ";
            case Severe:
                return u"SEVERE ERROR: ";
            case Error:
                return u"Error: ";
            case Warning:
                return u"Warning: ";
            case Debug:
                return u"Debug: ";
            default: // Including Info and Verbose
                return UString();
        }
    }
}
