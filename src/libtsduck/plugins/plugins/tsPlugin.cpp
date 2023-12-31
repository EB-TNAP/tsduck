//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsPlugin.h"

// Displayable names of plugin types.
const ts::Enumeration ts::PluginTypeNames({
    {u"input",            ts::PluginType::INPUT},
    {u"output",           ts::PluginType::OUTPUT},
    {u"packet processor", ts::PluginType::PROCESSOR},
});


//----------------------------------------------------------------------------
// Constructors and destructors.
//----------------------------------------------------------------------------

ts::Plugin::Plugin(TSP* to_tsp, const UString& description, const UString& syntax) :
    Args(description, syntax, NO_DEBUG | NO_VERBOSE | NO_VERSION | NO_CONFIG_FILE),
    tsp(to_tsp),
    duck(to_tsp)
{
}


//----------------------------------------------------------------------------
// Report implementation.
//----------------------------------------------------------------------------

void ts::Plugin::writeLog(int severity, const UString& message)
{
    // Force message to go through tsp
    tsp->log(severity, message);
}


//----------------------------------------------------------------------------
// Reset the internal TSDuck execution context of this plugin.
//----------------------------------------------------------------------------

void ts::Plugin::resetContext(const DuckContext::SavedArgs& state)
{
    duck.reset();
    duck.restoreArgs(state);
}


//----------------------------------------------------------------------------
// Default implementations of virtual methods.
//----------------------------------------------------------------------------

size_t ts::Plugin::stackUsage() const
{
    return DEFAULT_STACK_USAGE;
}

bool ts::Plugin::getOptions()
{
    return true;
}

bool ts::Plugin::start()
{
    return true;
}

bool ts::Plugin::stop()
{
    return true;
}

ts::BitRate ts::Plugin::getBitrate()
{
    return 0;
}

ts::BitRateConfidence ts::Plugin::getBitrateConfidence()
{
    return BitRateConfidence::LOW;
}

bool ts::Plugin::isRealTime()
{
    return false;
}

bool ts::Plugin::handlePacketTimeout()
{
    return false;
}
