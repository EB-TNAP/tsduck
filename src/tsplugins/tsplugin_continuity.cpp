//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  Transport stream processor shared library:
//  Check or fix continuity counters
//
//----------------------------------------------------------------------------

#include "tsPluginRepository.h"
#include "tsContinuityAnalyzer.h"


//----------------------------------------------------------------------------
// Plugin definition
//----------------------------------------------------------------------------

namespace ts {
    class ContinuityPlugin: public ProcessorPlugin
    {
        TS_NOBUILD_NOCOPY(ContinuityPlugin);
    public:
        // Implementation of plugin API
        ContinuityPlugin(TSP*);
        virtual bool getOptions() override;
        virtual bool start() override;
        virtual Status processPacket(TSPacket&, TSPacketMetadata&) override;

    private:
        UString            _tag {};                      // Message tag
        bool               _fix = false;                 // Fix incorrect continuity counters
        bool               _no_replicate = false;        // Option --no-replicate-duplicated
        int                _log_level = Severity::Info;  // Log level for discontinuity messages
        PIDSet             _pids {};                     // PID values to check or fix
        ContinuityAnalyzer _cc_analyzer {NoPID, tsp};    // Continuity counters analyzer
    };
}

TS_REGISTER_PROCESSOR_PLUGIN(u"continuity", ts::ContinuityPlugin);


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

ts::ContinuityPlugin::ContinuityPlugin(TSP* tsp_) :
    ProcessorPlugin(tsp_, u"Check or fix continuity counters on TS packets", u"[options]")
{
    option(u"fix", 'f');
    help(u"fix",
         u"Fix incorrect continuity counters. By default, only display discontinuities.");

    option(u"no-replicate-duplicated");
    help(u"no-replicate-duplicated",
         u"Two successive packets in the same PID are considered as duplicated if they have "
         u"the same continuity counter and same content (except PCR, if any). "
         u"By default, with --fix, duplicated input packets are replicated as duplicated on output "
         u"(the corresponding output packets have the same continuity counters). "
         u"When this option is specified, the input packets are not considered as duplicated and "
         u"the output packets receive individually incremented countinuity counters.");

    option(u"pid", 'p', PIDVAL, 0, UNLIMITED_COUNT);
    help(u"pid", u"pid1[-pid2]",
         u"Check or fix continuity counters only in packets with this PID value or range of values. "
         u"Several -p or --pid options may be specified. By default, all PID's "
         u"are checked or fixed.");

    option(u"tag", 't', STRING);
    help(u"tag", u"'string'",
         u"Message tag to be displayed when packets are missing. Useful when "
         u"the plugin is used several times in the same process.");
}


//----------------------------------------------------------------------------
// Get options method
//----------------------------------------------------------------------------

bool ts::ContinuityPlugin::getOptions()
{
    // Command line arguments
    getIntValues(_pids, u"pid", true);
    _fix = present(u"fix");
    _no_replicate = present(u"no-replicate-duplicated");
    _tag = value(u"tag");
    if (!_tag.empty()) {
        _tag += u": ";
    }

    // Null packets are not subject to continuity counters. Never check the null PID.
    _pids.reset(PID_NULL);

    // Without --fix, all discontinuities are always reported.
    // With --fix, this is only a verbose message.
    _log_level = _fix ? Severity::Verbose : Severity::Info;

    return true;
}


//----------------------------------------------------------------------------
// Start method
//----------------------------------------------------------------------------

bool ts::ContinuityPlugin::start()
{
    _cc_analyzer.reset();
    _cc_analyzer.setPIDFilter(_pids);
    _cc_analyzer.setDisplay(true);
    _cc_analyzer.setMessagePrefix(_tag);
    _cc_analyzer.setMessageSeverity(_log_level);
    _cc_analyzer.setFix(_fix);
    _cc_analyzer.setReplicateDuplicated(!_no_replicate);
    return true;
}


//----------------------------------------------------------------------------
// Packet processing method
//----------------------------------------------------------------------------

ts::ProcessorPlugin::Status ts::ContinuityPlugin::processPacket(TSPacket& pkt, TSPacketMetadata& pkt_data)
{
    _cc_analyzer.feedPacket(pkt);
    return TSP_OK;
}
