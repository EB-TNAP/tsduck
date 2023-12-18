//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Definition of the API of a tsp input plugin.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsPlugin.h"
#include "tsTSPacket.h"
#include "tsTSPacketMetadata.h"

namespace ts {
    //!
    //! Input @c tsp plugin interface.
    //!
    //! @ingroup plugin
    //!
    //! All shared libraries providing input capability shall return
    //! an object implementing this abstract interface.
    //!
    class TSDUCKDLL InputPlugin : public Plugin
    {
        TS_NOBUILD_NOCOPY(InputPlugin);
    public:
        //!
        //! Packet reception interface.
        //!
        //! The main application invokes receive() to get input packets.
        //! This method reads complete 188-byte TS packets in
        //! the buffer (never read partial packets).
        //!
        //! @param [out] buffer Address of the buffer for incoming packets.
        //! @param [in,out] pkt_data Array of metadata for incoming packets.
        //! A packet and its metadata have the same index in their respective arrays.
        //! @param [in] max_packets Size of @a buffer in number of packets.
        //! @return The number of actually received packets (in the range
        //! 1 to @a max_packets). Returning zero means error or end of input.
        //!
        virtual size_t receive(TSPacket* buffer, TSPacketMetadata* pkt_data, size_t max_packets) = 0;

        //!
        //! Set a receive timeout for all input operations.
        //!
        //! This method is typically invoked from tsp before starting the plugin.
        //!
        //! @param [in] timeout Receive timeout in milliseconds. No timeout if zero or negative.
        //! @return True when the timeout is accepted, false if not supported by the plugin.
        //!
        virtual bool setReceiveTimeout(std::chrono::milliseconds timeout);

        //!
        //! Abort the input operation currently in progress.
        //!
        //! This method is typically invoked from another thread when the input
        //! plugin is waiting for input. When this method is invoked, the plugin
        //! shall abort the current input and place the input plugin in some
        //! "error" or "end of input" state. The only acceptable operation
        //! after an abortInput() is a stop().
        //!
        //! @return True when the operation was properly handled. False in case
        //! of fatal error or if not supported by the plugin.
        //!
        virtual bool abortInput();

        // Implementation of inherited interface.
        virtual PluginType type() const override;

    protected:
        //!
        //! Constructor.
        //!
        //! @param [in] tsp_ Associated callback to @c tsp executable.
        //! @param [in] description A short one-line description, eg. "Wonderful File Copier".
        //! @param [in] syntax A short one-line syntax summary, eg. "[options] filename ...".
        //!
        InputPlugin(TSP* tsp_, const UString& description = UString(), const UString& syntax = UString());
    };
}
