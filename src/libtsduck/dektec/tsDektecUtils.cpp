//-----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//-----------------------------------------------------------------------------

#include "tsDektecUtils.h"
#include "tsDektec.h"
#include "tsVersionInfo.h"


//----------------------------------------------------------------------------
// Register for options --version and --support.
//----------------------------------------------------------------------------

#if defined(TS_NO_DTAPI)
    #define SUPPORT UNSUPPORTED
#else
    #define SUPPORT SUPPORTED
#endif

TS_REGISTER_FEATURE(u"dektec", u"Dektec", SUPPORT, ts::GetDektecVersions);


//-----------------------------------------------------------------------------
// Enumeration for various Dektec constants, names for values
//-----------------------------------------------------------------------------

const ts::Names& ts::DektecModulationTypes()
{
    static const Names data {
    #if !defined(TS_NO_DTAPI)
        {u"DVBS-QPSK",    DTAPI_MOD_DVBS_QPSK},
        {u"DVBS-BPSK",    DTAPI_MOD_DVBS_BPSK},
        {u"4-QAM",        DTAPI_MOD_QAM4},
        {u"16-QAM",       DTAPI_MOD_QAM16},
        {u"32-QAM",       DTAPI_MOD_QAM32},
        {u"64-QAM",       DTAPI_MOD_QAM64},
        {u"128-QAM",      DTAPI_MOD_QAM128},
        {u"256-QAM",      DTAPI_MOD_QAM256},
        {u"DVBT",         DTAPI_MOD_DVBT},
        {u"ATSC",         DTAPI_MOD_ATSC},
        {u"DVB-T2",       DTAPI_MOD_DVBT2},
        {u"ISDB-T",       DTAPI_MOD_ISDBT},
        {u"IQDIRECT",     DTAPI_MOD_IQDIRECT},
        {u"DVBS2-QPSK",   DTAPI_MOD_DVBS2_QPSK},
        {u"DVBS2-8PSK",   DTAPI_MOD_DVBS2_8PSK},
        {u"DVBS2-16APSK", DTAPI_MOD_DVBS2_16APSK},
        {u"DVBS2-32APSK", DTAPI_MOD_DVBS2_32APSK},
        {u"DMB-TH",       DTAPI_MOD_DMBTH},
        {u"ADTB-T",       DTAPI_MOD_ADTBT},
        {u"CMMB",         DTAPI_MOD_CMMB},
        {u"T2MI",         DTAPI_MOD_T2MI},
        {u"DVBC2",        DTAPI_MOD_DVBC2}
    #endif
    };
    return data;
}

const ts::Names& ts::DektecVSB()
{
    static const Names data {
    #if !defined(TS_NO_DTAPI)
        {u"8-VSB",  DTAPI_MOD_ATSC_VSB8},
        {u"16-VSB", DTAPI_MOD_ATSC_VSB16},
    #endif
    };
    return data;
}

const ts::Names& ts::DektecFEC()
{
    static const Names data {
    #if !defined(TS_NO_DTAPI)
        {u"1/2",         DTAPI_MOD_1_2},
        {u"2/3",         DTAPI_MOD_2_3},
        {u"3/4",         DTAPI_MOD_3_4},
        {u"4/5",         DTAPI_MOD_4_5},
        {u"5/6",         DTAPI_MOD_5_6},
        {u"6/7",         DTAPI_MOD_6_7},
        {u"7/8",         DTAPI_MOD_7_8},
        {u"1/4",         DTAPI_MOD_1_4},
        {u"1/3",         DTAPI_MOD_1_3},
        {u"2/5",         DTAPI_MOD_2_5},
        {u"3/5",         DTAPI_MOD_3_5},
        {u"8/9",         DTAPI_MOD_8_9},
        {u"9/10",        DTAPI_MOD_9_10},
        {u"unknown-FEC", DTAPI_MOD_CR_UNK},
    #endif
    };
    return data;
}

const ts::Names& ts::DektecInversion()
{
    static const Names data {
    #if !defined(TS_NO_DTAPI)
        {u"non-inverted", DTAPI_MOD_S_S2_SPECNONINV},
        {u"inverted",     DTAPI_MOD_S_S2_SPECINV},
    #endif
    };
    return data;
}

const ts::Names& ts::DektecDVBTProperty()
{
    static const Names data {
    #if !defined(TS_NO_DTAPI)
        {u"5-MHz",                     DTAPI_MOD_DVBT_5MHZ},
        {u"6-MHz",                     DTAPI_MOD_DVBT_6MHZ},
        {u"7-MHz",                     DTAPI_MOD_DVBT_7MHZ},
        {u"8-MHz",                     DTAPI_MOD_DVBT_8MHZ},
        {u"unknown-bandwidth",         DTAPI_MOD_DVBT_BW_UNK},
        {u"QPSK",                      DTAPI_MOD_DVBT_QPSK},
        {u"16-QAM",                    DTAPI_MOD_DVBT_QAM16},
        {u"64-QAM",                    DTAPI_MOD_DVBT_QAM64},
        {u"unknown-constellation",     DTAPI_MOD_DVBT_CO_UNK},
        {u"1/32",                      DTAPI_MOD_DVBT_G_1_32},
        {u"1/16",                      DTAPI_MOD_DVBT_G_1_16},
        {u"1/8",                       DTAPI_MOD_DVBT_G_1_8},
        {u"1/4",                       DTAPI_MOD_DVBT_G_1_4},
        {u"unknown-guard-interval",    DTAPI_MOD_DVBT_GU_UNK},
        {u"indepth-interleave",        DTAPI_MOD_DVBT_INDEPTH},
        {u"native-interleave",         DTAPI_MOD_DVBT_NATIVE},
        {u"2K",                        DTAPI_MOD_DVBT_2K},
        {u"4K",                        DTAPI_MOD_DVBT_4K},
        {u"8K",                        DTAPI_MOD_DVBT_8K},
        {u"unknown-transmission-mode", DTAPI_MOD_DVBT_MD_UNK},
    #endif
    };
    return data;
}

const ts::Names& ts::DektecPowerMode()
{
    static const Names data {
    #if !defined(TS_NO_DTAPI)
        {u"high-quality", DTAPI_IOCONFIG_MODHQ},
        {u"low-power",    DTAPI_IOCONFIG_LOWPWR},
    #endif
    };
    return data;
}


//-----------------------------------------------------------------------------
// Check if this version of TSDuck was build with Dektec support.
//-----------------------------------------------------------------------------

bool ts::HasDektecSupport()
{
#if defined(TS_NO_DTAPI)
    return false;
#else
    return true;
#endif
}


//-----------------------------------------------------------------------------
// Get the versions of Dektec API and drivers in one single string.
//-----------------------------------------------------------------------------

ts::UString ts::GetDektecVersions()
{
#if defined(TS_NO_DTAPI)
    return TS_NO_DTAPI_MESSAGE;
#else
    std::map<UString,UString> versions;
    GetDektecVersions(versions);

    UString result;
    for (const auto& it : versions) {
        if (!result.empty()) {
            result.append(u", ");
        }
        result.append(it.first);
        result.append(u": ");
        result.append(it.second);
    }
    return result;
#endif
}


//-----------------------------------------------------------------------------
// Get the versions of Dektec API and drivers.
//-----------------------------------------------------------------------------

void ts::GetDektecVersions(std::map<UString,UString>& versions)
{
    versions.clear();

#if !defined(TS_NO_DTAPI)

    int major = 0;
    int minor = 0;
    int bugfix = 0;
    int build = 0;

    // DTAPI version is always available.
    Dtapi::DtapiGetVersion(major, minor, bugfix, build);
    versions[u"DTAPI"].format(u"%d.%d.%d.%d", major, minor, bugfix, build);

    // DTAPI service is optional.
    major = minor = bugfix = build = 0;
    if (Dtapi::DtapiGetDtapiServiceVersion(major, minor, bugfix, build) == DTAPI_OK) {
        versions[u"Service"].format(u"%d.%d.%d.%d", major, minor, bugfix, build);
    }

    // Get all Dektec drivers versions.
    std::vector<Dtapi::DtDriverVersionInfo> drv;
    if (Dtapi::DtapiGetDeviceDriverVersion(DTAPI_CAT_ALL, drv) == DTAPI_OK) {
        for (const auto& it : drv) {
            versions[UString::FromWChar(it.m_Name)].format(u"%d.%d.%d.%d", it.m_Major, it.m_Minor, it.m_BugFix, it.m_Build);
        }
    }
#endif
}
