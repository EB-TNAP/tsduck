//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  TSUnit test suite for class ts::TSPacketWindow
//
//----------------------------------------------------------------------------

#include "tsTSPacketWindow.h"
#include "tsunit.h"


//----------------------------------------------------------------------------
// The test fixture
//----------------------------------------------------------------------------

class TSPacketWindowTest: public tsunit::Test
{
    TSUNIT_DECLARE_TEST(All);
};

TSUNIT_REGISTER(TSPacketWindowTest);


//----------------------------------------------------------------------------
// Unitary tests.
//----------------------------------------------------------------------------

TSUNIT_DEFINE_TEST(All)
{
    // Physical buffer of 10 packets, PID 100 to 109.
    ts::TSPacket packets[10];
    for (size_t i = 0; i < 10; ++i) {
        packets[i].init(ts::PID(100 + i));
    }

    // Corresponding metadata.
    ts::TSPacketMetadata mdata[10];

    // Map logical index in packet window to physical index
    // There are 4 segments of contiguous packets.
    const size_t map[10] = {8, 9, 4, 5, 6, 7, 3, 0, 1, 2};

    // Build packet window.
    ts::TSPacketWindow win;
    for (size_t i = 0; i < 10; ++i) {
        win.addPacketsReference(packets + map[i], mdata + map[i], 1);
    }

    TSUNIT_EQUAL(10, win.size());
    TSUNIT_EQUAL(4, win.segmentCount());
    TSUNIT_EQUAL(0, win.nullifyCount());
    TSUNIT_EQUAL(0, win.dropCount());

    // Sequential access.
    TSUNIT_ASSERT(win.packet(0) == &packets[map[0]]);
    TSUNIT_ASSERT(win.metadata(0) == &mdata[map[0]]);
    TSUNIT_ASSERT(win.packet(1) == &packets[map[1]]);
    TSUNIT_ASSERT(win.metadata(1) == &mdata[map[1]]);
    TSUNIT_ASSERT(win.packet(2) == &packets[map[2]]);
    TSUNIT_ASSERT(win.metadata(2) == &mdata[map[2]]);
    TSUNIT_ASSERT(win.packet(3) == &packets[map[3]]);
    TSUNIT_ASSERT(win.metadata(3) == &mdata[map[3]]);
    TSUNIT_ASSERT(win.packet(4) == &packets[map[4]]);
    TSUNIT_ASSERT(win.metadata(4) == &mdata[map[4]]);
    TSUNIT_ASSERT(win.packet(5) == &packets[map[5]]);
    TSUNIT_ASSERT(win.metadata(5) == &mdata[map[5]]);
    TSUNIT_ASSERT(win.packet(6) == &packets[map[6]]);
    TSUNIT_ASSERT(win.metadata(6) == &mdata[map[6]]);
    TSUNIT_ASSERT(win.packet(7) == &packets[map[7]]);
    TSUNIT_ASSERT(win.metadata(7) == &mdata[map[7]]);
    TSUNIT_ASSERT(win.packet(8) == &packets[map[8]]);
    TSUNIT_ASSERT(win.metadata(8) == &mdata[map[8]]);
    TSUNIT_ASSERT(win.packet(9) == &packets[map[9]]);
    TSUNIT_ASSERT(win.metadata(9) == &mdata[map[9]]);

    // Random access.
    TSUNIT_ASSERT(win.packet(2) == &packets[map[2]]);
    TSUNIT_ASSERT(win.metadata(8) == &mdata[map[8]]);
    TSUNIT_ASSERT(win.packet(1) == &packets[map[1]]);
    TSUNIT_ASSERT(win.metadata(9) == &mdata[map[9]]);
    TSUNIT_ASSERT(win.packet(4) == &packets[map[4]]);
    TSUNIT_ASSERT(win.packet(10) == nullptr);
    TSUNIT_ASSERT(win.packet(7) == &packets[map[7]]);
    TSUNIT_ASSERT(win.metadata(1) == &mdata[map[1]]);
    TSUNIT_ASSERT(win.packet(0) == &packets[map[0]]);
    TSUNIT_ASSERT(win.metadata(11) == nullptr);
    TSUNIT_ASSERT(win.metadata(5) == &mdata[map[5]]);

    TSUNIT_EQUAL(100 + map[4], packets[map[4]].getPID());
    win.nullify(4);
    TSUNIT_EQUAL(ts::PID_NULL, packets[map[4]].getPID());
    TSUNIT_EQUAL(1, win.nullifyCount());
    win.nullify(4);
    win.nullify(4);
    TSUNIT_EQUAL(1, win.nullifyCount());

    TSUNIT_EQUAL(100 + map[8], packets[map[8]].getPID());
    win.nullify(8);
    TSUNIT_EQUAL(2, win.nullifyCount());
    TSUNIT_EQUAL(ts::PID_NULL, packets[map[8]].getPID());

    TSUNIT_EQUAL(ts::SYNC_BYTE, packets[map[7]].b[0]);
    win.drop(7);
    TSUNIT_EQUAL(0, packets[map[7]].b[0]);
    TSUNIT_EQUAL(1, win.dropCount());
    win.drop(7);
    win.drop(7);
    win.drop(7);
    TSUNIT_EQUAL(1, win.dropCount());

    TSUNIT_EQUAL(ts::SYNC_BYTE, packets[map[1]].b[0]);
    TSUNIT_EQUAL(ts::SYNC_BYTE, packets[map[9]].b[0]);
    win.drop(1);
    win.drop(9);
    TSUNIT_EQUAL(0, packets[map[1]].b[0]);
    TSUNIT_EQUAL(0, packets[map[9]].b[0]);
    TSUNIT_EQUAL(3, win.dropCount());

    TSUNIT_EQUAL(map[0], win.packetIndexInBuffer(0, packets, 10));
    TSUNIT_EQUAL(ts::NPOS, win.packetIndexInBuffer(0, packets + 10, 10));
    TSUNIT_EQUAL(map[9], win.packetIndexInBuffer(9, packets, 10));
    TSUNIT_EQUAL(map[7], win.packetIndexInBuffer(7, packets, 10));
    TSUNIT_EQUAL(ts::NPOS, win.packetIndexInBuffer(11, packets, 10));
}
