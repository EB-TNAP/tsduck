//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  Transport stream processor shared library:
//  Decapsulate TS packets from one single PID. See also tsplugin_encap.cpp.
//
//----------------------------------------------------------------------------

#include "tsPluginRepository.h"
#include "tsPacketDecapsulation.h"


//----------------------------------------------------------------------------
// Plugin definition
//----------------------------------------------------------------------------

namespace ts {
    class DecapPlugin: public ProcessorPlugin
    {
        TS_PLUGIN_CONSTRUCTORS(DecapPlugin);
    public:
        // Implementation of plugin API
        virtual bool getOptions() override;
        virtual bool start() override;
        virtual Status processPacket(TSPacket&, TSPacketMetadata&) override;

    private:
        bool                _ignoreErrors;  // Ignore encapsulation errors.
        PID                 _pid;           // Input PID.
        PacketDecapsulation _decap;         // Decapsulation engine.
    };
}

TS_REGISTER_PROCESSOR_PLUGIN(u"decap", ts::DecapPlugin);


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

ts::DecapPlugin::DecapPlugin(TSP* tsp_) :
    ProcessorPlugin(tsp_, u"Decapsulate TS packets from a PID produced by encap plugin", u"[options]"),
    _ignoreErrors(false),
    _pid(PID_NULL),
    _decap()
{
    option(u"ignore-errors", 'i');
    help(u"ignore-errors",
         u"Ignore errors such malformed encapsulated stream.");

    option(u"pid", 'p', PIDVAL);
    help(u"pid",
         u"Specify the input PID containing all encapsulated PID's. "
         u"This is a mandatory parameter, there is no default.");
}


//----------------------------------------------------------------------------
// Get options method
//----------------------------------------------------------------------------

bool ts::DecapPlugin::getOptions()
{
    _ignoreErrors = present(u"ignore-errors");
    _pid = intValue<PID>(u"pid", PID_NULL);
    return true;
}


//----------------------------------------------------------------------------
// Start method
//----------------------------------------------------------------------------

bool ts::DecapPlugin::start()
{
    _decap.reset(_pid);
    return true;
}


//----------------------------------------------------------------------------
// Packet processing method
//----------------------------------------------------------------------------

ts::ProcessorPlugin::Status ts::DecapPlugin::processPacket(TSPacket& pkt, TSPacketMetadata& pkt_data)
{
    if (_decap.processPacket(pkt) || _ignoreErrors || _decap.lastError().empty()) {
        return TSP_OK;
    }
    else {
        tsp->error(_decap.lastError());
        return TSP_END;
    }
}
