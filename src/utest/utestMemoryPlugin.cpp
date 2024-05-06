//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  TSUnit test suite for input and output memory plugins
//
//----------------------------------------------------------------------------

#include "tsPluginEventHandlerInterface.h"
#include "tsPluginEventData.h"
#include "tsTSProcessor.h"
#include "tsAsyncReport.h"
#include "tsunit.h"


//----------------------------------------------------------------------------
// The test fixture
//----------------------------------------------------------------------------

class MemoryPluginTest: public tsunit::Test
{
    TSUNIT_DECLARE_TEST(All);
};

TSUNIT_REGISTER(MemoryPluginTest);


//----------------------------------------------------------------------------
// An asynchronous report class which fills a string buffer.
//----------------------------------------------------------------------------

namespace {
    class TestReport : public ts::AsyncReport
    {
        TS_NOBUILD_NOCOPY(TestReport);
    public:
        TestReport(ts::UString& buffer, int max_severity = ts::Severity::Info, const ts::AsyncReportArgs& args = ts::AsyncReportArgs());
    private:
        ts::UString& _buffer;
        virtual void asyncThreadLog(int severity, const ts::UString& message) override;
    };

    TestReport::TestReport(ts::UString& buffer, int max_severity, const ts::AsyncReportArgs& args) :
        ts::AsyncReport(max_severity, args),
        _buffer(buffer)
    {
    }

    void TestReport::asyncThreadLog(int severity, const ts::UString& message)
    {
        if (!_buffer.empty()) {
            _buffer.append(ts::LINE_FEED);
        }
        _buffer.append(message);
        tsunit::Test::debug() << message << std::endl;
    }
}


//----------------------------------------------------------------------------
// An event handler for memory input plugin: send packets one by one.
//----------------------------------------------------------------------------

namespace {
    class Input : public ts::PluginEventHandlerInterface
    {
        TS_NOBUILD_NOCOPY(Input);
    public:
        Input(const ts::TSPacket* packets, size_t count);
        virtual void handlePluginEvent(const ts::PluginEventContext& context) override;
    private:
        const ts::TSPacket* _packets;
        size_t _packets_count;
    };

    Input::Input(const ts::TSPacket* packets, size_t count) :
        _packets(packets),
        _packets_count(count)
    {
    }

    void Input::handlePluginEvent(const ts::PluginEventContext& context)
    {
        ts::PluginEventData* data = dynamic_cast<ts::PluginEventData*>(context.pluginData());
        if (data != nullptr && _packets_count > 0) {
            data->append(_packets, ts::PKT_SIZE);
            _packets++;
            _packets_count--;
        }
    }
}


//----------------------------------------------------------------------------
// An event handler for memory output plugin: fill a vector of packets.
//----------------------------------------------------------------------------

namespace {
    class Output : public ts::PluginEventHandlerInterface
    {
        TS_NOBUILD_NOCOPY(Output);
    public:
        Output(ts::TSPacketVector& output);
        virtual void handlePluginEvent(const ts::PluginEventContext& context) override;
    private:
        ts::TSPacketVector& _output;
    };

    Output::Output(ts::TSPacketVector& output) :
        _output(output)
    {
    }

    void Output::handlePluginEvent(const ts::PluginEventContext& context)
    {
        ts::PluginEventData* data = dynamic_cast<ts::PluginEventData*>(context.pluginData());
        if (data != nullptr) {
            const size_t packets_count = data->size() / ts::PKT_SIZE;
            const size_t index = _output.size();
            _output.resize(index + packets_count);
            ts::TSPacket::Copy(&_output[index], data->data(), packets_count);
        }
    }
}


//----------------------------------------------------------------------------
// Reference TS packets, all on PID 100.
//----------------------------------------------------------------------------

namespace {

    // Sample command to generates one randome packet:
    // tsp -I craft --pid 100 --count 1 --payload-pattern $(head -c 184 /dev/random | xxd -p | tr '\n' ' ' | sed 's/ //g') | tsdump -rc

    constexpr size_t REF_PACKETS_COUNT = 3;

    const ts::TSPacket REF_PACKETS[REF_PACKETS_COUNT] = {
        {{0x47, 0x00, 0x64, 0x10, 0x27, 0xCD, 0x88, 0x8B, 0x72, 0x99, 0xB9, 0x7E, 0x9F, 0xC8, 0x11, 0xD8,
          0x34, 0xEC, 0xFE, 0x57, 0xFF, 0xF9, 0x80, 0x2F, 0xF5, 0xCD, 0x0B, 0x05, 0xD0, 0x9F, 0x21, 0x23,
          0x60, 0x47, 0x28, 0x80, 0x4E, 0xE3, 0xAD, 0x89, 0xFF, 0x92, 0x85, 0x93, 0x5E, 0x41, 0xB8, 0xCF,
          0xED, 0xA8, 0x41, 0xEC, 0x13, 0x22, 0x7B, 0x5C, 0x37, 0xD2, 0xDB, 0x06, 0xB1, 0xB5, 0xF5, 0x62,
          0x9D, 0x3C, 0x48, 0x99, 0x5B, 0xDD, 0xF9, 0x66, 0x1A, 0xD1, 0xE9, 0xE1, 0x43, 0x6F, 0xDF, 0x17,
          0x0E, 0x85, 0xD1, 0x98, 0x97, 0x64, 0xFB, 0x18, 0xF9, 0x91, 0xB6, 0x7D, 0x5A, 0xBF, 0x2D, 0xAD,
          0xCD, 0x28, 0xC2, 0xDE, 0xB2, 0x47, 0x92, 0x57, 0xF5, 0x5B, 0x1C, 0x71, 0xC8, 0xF0, 0xC9, 0x21,
          0x3E, 0x0D, 0xC3, 0xF8, 0x32, 0x15, 0xEF, 0x55, 0x81, 0xF6, 0x05, 0xA8, 0x0D, 0x83, 0xD2, 0x8D,
          0x34, 0x29, 0x1C, 0x2A, 0x01, 0x07, 0xE4, 0x50, 0x48, 0x55, 0xD4, 0xA8, 0xDF, 0x94, 0x47, 0x4C,
          0x2A, 0x0A, 0x84, 0xB5, 0x6F, 0xD4, 0xF3, 0x87, 0x8C, 0xB1, 0x2B, 0x47, 0x85, 0x0A, 0x2D, 0x1D,
          0x9A, 0x2B, 0x18, 0x45, 0x99, 0x01, 0xC1, 0x93, 0x79, 0xFB, 0x94, 0xF3, 0x52, 0x24, 0x1D, 0xB4,
          0xD9, 0x2F, 0x2D, 0x95, 0xD6, 0xFA, 0xA8, 0xA5, 0x46, 0x62, 0x54, 0x01}},

        {{0x47, 0x00, 0x64, 0x10, 0x9E, 0x9E, 0xD4, 0x70, 0x06, 0xDB, 0x11, 0xD4, 0x7A, 0xA5, 0x97, 0x11,
          0x2E, 0xBC, 0x7D, 0x62, 0x7C, 0xA6, 0x90, 0xB8, 0x49, 0x72, 0x1A, 0xBD, 0xA0, 0x04, 0x0D, 0x59,
          0x42, 0xDD, 0x91, 0x29, 0xA4, 0xD4, 0x21, 0xD1, 0x63, 0xD8, 0x36, 0xC6, 0xAE, 0xD2, 0x65, 0xE0,
          0xA5, 0x85, 0xE9, 0xF9, 0xD9, 0x7E, 0x44, 0xF1, 0x7B, 0xF2, 0x49, 0x19, 0x22, 0xEC, 0x8D, 0x9B,
          0xBC, 0xAB, 0xE3, 0xC9, 0x0A, 0x98, 0x3F, 0xF3, 0xAF, 0x3B, 0x1B, 0xE9, 0x52, 0xCF, 0x70, 0x8A,
          0x2C, 0x22, 0xF9, 0xF8, 0x93, 0x7B, 0xAF, 0xD8, 0x99, 0x97, 0x4F, 0x54, 0x59, 0x3F, 0x69, 0xC4,
          0xFB, 0xD6, 0x93, 0xC8, 0x00, 0x80, 0x08, 0x8D, 0x1B, 0x97, 0xB2, 0xDD, 0x9D, 0x39, 0x2B, 0x55,
          0xA8, 0xAA, 0x2D, 0x15, 0xDB, 0xF4, 0x3E, 0xC6, 0x4C, 0x38, 0x44, 0x3E, 0x04, 0x05, 0x83, 0xAA,
          0x52, 0x8E, 0x81, 0xD6, 0x69, 0x20, 0x33, 0xAA, 0xB6, 0x5C, 0xFF, 0xB8, 0xCB, 0xE5, 0xCC, 0xAB,
          0xB8, 0x73, 0xE8, 0xC3, 0xDE, 0x25, 0x38, 0xCA, 0x8F, 0x38, 0xF4, 0x33, 0xEE, 0xA4, 0xA6, 0x57,
          0x17, 0x2F, 0xC2, 0xCB, 0xE9, 0xD7, 0xCC, 0x42, 0xCA, 0x27, 0xFF, 0xDA, 0xF6, 0x5D, 0x53, 0x86,
          0xBB, 0x7E, 0x0A, 0x62, 0xA9, 0x83, 0xD0, 0x54, 0x2D, 0x50, 0x1F, 0xBC}},

        {{0x47, 0x00, 0x64, 0x10, 0x37, 0x71, 0x69, 0xA4, 0x09, 0xF0, 0xE4, 0x7F, 0xCC, 0x4C, 0xDA, 0x9F,
          0x21, 0x90, 0x12, 0x7A, 0xEC, 0x04, 0x48, 0xC8, 0x6C, 0x9D, 0x0C, 0xF2, 0x3E, 0x3E, 0x5C, 0xCA,
          0xF3, 0x98, 0xF3, 0xCC, 0x09, 0x44, 0x61, 0x30, 0x91, 0x1B, 0xAA, 0x66, 0x8E, 0x8A, 0x2D, 0xDC,
          0x23, 0x9B, 0xF9, 0xE0, 0x8F, 0x67, 0x79, 0xDD, 0x7C, 0x6C, 0xF8, 0x35, 0x82, 0x13, 0x8B, 0x23,
          0xFF, 0x1C, 0xE4, 0x5B, 0x53, 0x17, 0x95, 0xD0, 0x08, 0x40, 0xAE, 0x6B, 0xFF, 0xF9, 0x80, 0x27,
          0x6F, 0xE1, 0xF2, 0x82, 0x85, 0xB6, 0xF5, 0x00, 0x5F, 0x49, 0xF4, 0x67, 0x45, 0x84, 0xE8, 0x76,
          0xE8, 0xB9, 0xE0, 0x06, 0x12, 0x1A, 0x6A, 0x5F, 0x16, 0x7D, 0x59, 0x5A, 0x4D, 0xA2, 0x7F, 0x4D,
          0x93, 0x85, 0x93, 0xEA, 0x79, 0x2D, 0x8C, 0xAB, 0xA1, 0x6C, 0xE4, 0x04, 0x80, 0xB1, 0x87, 0x40,
          0xCB, 0x07, 0x93, 0xED, 0xAA, 0x8C, 0x82, 0xDD, 0xA6, 0x4A, 0xFB, 0x1F, 0x95, 0x6A, 0x2C, 0xEF,
          0x61, 0x09, 0x73, 0x07, 0x9C, 0xDA, 0x03, 0xE0, 0x49, 0x94, 0x19, 0x4C, 0xE1, 0xCF, 0x99, 0xE9,
          0x42, 0xDE, 0xCD, 0x4F, 0xEC, 0x79, 0xD8, 0xF6, 0xE2, 0xE2, 0xEB, 0x2A, 0x6C, 0x66, 0x2E, 0xBE,
          0x44, 0x5C, 0xC7, 0x97, 0xE2, 0x3A, 0xC9, 0xB5, 0x52, 0xA8, 0xEF, 0x3F}}
    };
}


//----------------------------------------------------------------------------
// Unitary tests.
//----------------------------------------------------------------------------

TSUNIT_DEFINE_TEST(All)
{
    ts::UString log_buffer;
    TestReport log(log_buffer);

    ts::TSPacketVector output_packets;
    Input input(REF_PACKETS, REF_PACKETS_COUNT);
    Output output(output_packets);

    ts::TSProcessorArgs opt;
    opt.input = {u"memory", {}};
    opt.output = {u"memory", {}};

    ts::TSProcessor tsp(log);
    tsp.registerEventHandler(&input, ts::PluginType::INPUT);
    tsp.registerEventHandler(&output, ts::PluginType::OUTPUT);

    TSUNIT_ASSERT(tsp.start(opt));
    tsp.waitForTermination();

    TSUNIT_EQUAL(REF_PACKETS_COUNT, output_packets.size());
    TSUNIT_EQUAL(0, ts::MemCompare(&output_packets[0], REF_PACKETS, ts::PKT_SIZE * REF_PACKETS_COUNT));
    TSUNIT_EQUAL(u"", log_buffer);
}
