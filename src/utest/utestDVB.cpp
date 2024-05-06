//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  TSUnit test suite for DVB classes
//
//----------------------------------------------------------------------------

#include "tsTunerArgs.h"
#include "tsDuckContext.h"
#include "tsCerrReport.h"
#include "tsNullReport.h"
#include "tsArgs.h"
#include "tsunit.h"


//----------------------------------------------------------------------------
// The test fixture
//----------------------------------------------------------------------------

class DVBTest: public tsunit::Test
{
    TSUNIT_DECLARE_TEST(TunerArgs);
    TSUNIT_DECLARE_TEST(TunerParams);
    TSUNIT_DECLARE_TEST(AllLNB);
    TSUNIT_DECLARE_TEST(LNB);
    TSUNIT_DECLARE_TEST(UniversalLNB);
    TSUNIT_DECLARE_TEST(JapanLNB);

private:
    ts::Report& report();
    static void displayLNB(const ts::LNB& lnb);
    static void testParameters(ts::DeliverySystem delsys);
};

TSUNIT_REGISTER(DVBTest);


//----------------------------------------------------------------------------
// Common tools.
//----------------------------------------------------------------------------

ts::Report& DVBTest::report()
{
    if (tsunit::Test::debugMode()) {
        return CERR;
    }
    else {
        return NULLREP;
    }
}


//----------------------------------------------------------------------------
// Unitary tests.
//----------------------------------------------------------------------------

TSUNIT_DEFINE_TEST(TunerArgs)
{
    ts::Args args(u"Test tuner", u"[options]");
    ts::TunerArgs tuner_args;
    tuner_args.defineArgs(args, true);
    debug() << "DVBTest:: TunerArgs: " << std::endl << args.getHelpText(ts::Args::HELP_FULL) << std::endl;
}

void DVBTest::testParameters(ts::DeliverySystem delsys)
{
    debug() << "DVBTest: Default TunerParameters, type: " << ts::DeliverySystemEnum.name(delsys) << std::endl;

    ts::ModulationArgs params;
    params.delivery_system = delsys;
    params.frequency = 1000000;
    params.setDefaultValues();
    TSUNIT_ASSERT(params.hasModulationArgs());

    const ts::UString opts(params.toPluginOptions());
    debug() << "DVBTest: Options: \"" << opts << "\"" << std::endl;

    ts::Args args;
    ts::TunerArgs targs1;
    targs1.defineArgs(args, true);

    ts::UStringVector args_vec;
    opts.split(args_vec, u' ');
    TSUNIT_ASSERT(args.analyze(u"", args_vec));

    ts::DuckContext duck;
    TSUNIT_ASSERT(targs1.loadArgs(duck, args));
    TSUNIT_ASSERT(targs1.toPluginOptions() == opts);
}

TSUNIT_DEFINE_TEST(TunerParams)
{
    testParameters(ts::DS_DVB_S);
    testParameters(ts::DS_DVB_C);
    testParameters(ts::DS_DVB_T);
    testParameters(ts::DS_ATSC);
}

TSUNIT_DEFINE_TEST(AllLNB)
{
    ts::UStringList names(ts::LNB::GetAllNames(report()));
    debug() << "DVBTest::testAllLNB: " << ts::UString::Join(names, u" | ") << std::endl;
    TSUNIT_ASSERT(!names.empty());
}

void DVBTest::displayLNB(const ts::LNB& lnb)
{
    debug() << "DVBTest: Test LNB: name: \"" << lnb.name() << "\"" << std::endl
            << "    convert to string: \"" << lnb << "\"" << std::endl
            << "    valid: " << ts::UString::TrueFalse(lnb.isValid()) << std::endl
            << "    number of bands: " << lnb.bandsCount() << std::endl
            << "    polarization-controlled: " << ts::UString::TrueFalse(lnb.isPolarizationControlled()) << std::endl
            << "    legacyLowOscillatorFrequency: " << lnb.legacyLowOscillatorFrequency() << std::endl
            << "    legacyHighOscillatorFrequency: " << lnb.legacyHighOscillatorFrequency() << std::endl
            << "    legacySwitchFrequency: " << lnb.legacySwitchFrequency() << std::endl;
}

TSUNIT_DEFINE_TEST(LNB)
{
    ts::LNB lnb1(u"", report());
    debug() << "DVBTest::testLNB(): default LNB:" << std::endl;
    displayLNB(lnb1);
    TSUNIT_ASSERT(lnb1.isValid());

    ts::LNB lnb2(u"9000,10000,11000", report());
    displayLNB(lnb2);
    TSUNIT_ASSERT(lnb2.isValid());

    ts::LNB lnb3(u"9500", report());
    displayLNB(lnb3);
    TSUNIT_ASSERT(lnb3.isValid());

    ts::LNB lnb4(u"9500,10000", report());
    displayLNB(lnb4);
    TSUNIT_ASSERT(!lnb4.isValid());

    ts::LNB lnb5(u"azerty", report());
    displayLNB(lnb5);
    TSUNIT_ASSERT(!lnb5.isValid());
}

TSUNIT_DEFINE_TEST(UniversalLNB)
{
    ts::LNB lnb(u"universal", report());
    displayLNB(lnb);

    TSUNIT_ASSERT(lnb.isValid());
    TSUNIT_ASSERT(!lnb.isPolarizationControlled());
    TSUNIT_EQUAL(2, lnb.bandsCount());
    TSUNIT_EQUAL(9750000000, lnb.legacyLowOscillatorFrequency());
    TSUNIT_EQUAL(10600000000, lnb.legacyHighOscillatorFrequency());
    TSUNIT_EQUAL(11700000000, lnb.legacySwitchFrequency());

    ts::LNB::Transposition tr;

    TSUNIT_ASSERT(lnb.transpose(tr, 11000000000, ts::POL_AUTO, report()));
    TSUNIT_EQUAL(11000000000, tr.satellite_frequency);
    TSUNIT_EQUAL(1250000000, tr.intermediate_frequency);
    TSUNIT_EQUAL(9750000000, tr.oscillator_frequency);
    TSUNIT_ASSERT(!tr.stacked);
    TSUNIT_EQUAL(0, tr.band_index);

    TSUNIT_ASSERT(lnb.transpose(tr, 12000000000, ts::POL_AUTO, report()));
    TSUNIT_EQUAL(12000000000, tr.satellite_frequency);
    TSUNIT_EQUAL(1400000000, tr.intermediate_frequency);
    TSUNIT_EQUAL(10600000000, tr.oscillator_frequency);
    TSUNIT_ASSERT(!tr.stacked);
    TSUNIT_EQUAL(1, tr.band_index);

    // Outside bands
    TSUNIT_ASSERT(!lnb.transpose(tr, 8000000000, ts::POL_AUTO, report()));
}

TSUNIT_DEFINE_TEST(JapanLNB)
{
    ts::LNB lnb(u"japan", report());
    displayLNB(lnb);

    TSUNIT_ASSERT(lnb.isValid());
    TSUNIT_ASSERT(lnb.isPolarizationControlled());
    TSUNIT_EQUAL(2, lnb.bandsCount());
    TSUNIT_EQUAL(0, lnb.legacyLowOscillatorFrequency());
    TSUNIT_EQUAL(0, lnb.legacyHighOscillatorFrequency());
    TSUNIT_EQUAL(0, lnb.legacySwitchFrequency());

    ts::LNB::Transposition tr;

    // Channel BS-15
    TSUNIT_ASSERT(lnb.transpose(tr, 11996000000, ts::POL_RIGHT, report()));
    TSUNIT_EQUAL(11996000000, tr.satellite_frequency);
    TSUNIT_EQUAL(1318000000, tr.intermediate_frequency);
    TSUNIT_EQUAL(10678000000, tr.oscillator_frequency);
    TSUNIT_ASSERT(tr.stacked);

    // Channel ND-15
    TSUNIT_ASSERT(lnb.transpose(tr, 12551000000, ts::POL_LEFT, report()));
    TSUNIT_EQUAL(12551000000, tr.satellite_frequency);
    TSUNIT_EQUAL(3046000000, tr.intermediate_frequency);
    TSUNIT_EQUAL(9505000000, tr.oscillator_frequency);
    TSUNIT_ASSERT(tr.stacked);

    // Need polarization.
    TSUNIT_ASSERT(!lnb.transpose(tr, 12551000000, ts::POL_NONE, report()));

    // Outside bands
    TSUNIT_ASSERT(!lnb.transpose(tr, 11000000000, ts::POL_RIGHT, report()));
}
