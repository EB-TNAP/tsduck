//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  List DVB devices characteristics.
//
//----------------------------------------------------------------------------

#include "tsMain.h"
#include "tsDuckContext.h"
#include "tsTuner.h"
#include "tsTunerArgs.h"
TS_MAIN(MainCode);

#if defined(TS_WINDOWS)
    #include "tsDirectShowTest.h"
#endif


//----------------------------------------------------------------------------
//  Command line options
//----------------------------------------------------------------------------

namespace {
    class Options: public ts::Args
    {
        TS_NOBUILD_NOCOPY(Options);
    public:
        Options(int argc, char *argv[]);

#if defined(TS_WINDOWS)
        ts::DirectShowTest::TestType test_type = ts::DirectShowTest::NONE;  // DirectShow test (Windows only).
#endif
        ts::DuckContext duck {this};
        ts::TunerArgs   tuner_args {true};  // true = info_only
        bool            extended = false;
    };
}

Options::Options(int argc, char *argv[]) :
    ts::Args(u"List DVB tuner devices", u"[options]")
{
    // Common tuner options.
    tuner_args.defineArgs(*this, true);

    option(u"extended-info", 'e');
    help(u"extended-info", u"Display extended information.");

#if defined(TS_WINDOWS)

    option(u"enumerate-devices");
    help(u"enumerate-devices", u"Legacy option, equivalent to --test enumerate-devices.");

    option(u"list-devices", 'l');
    help(u"list-devices", u"Get a list of all tuner and receiver devices, equivalent to --test list-devices.");

    option(u"test", 't', ts::DirectShowTest::TestNames());
    help(u"test", u"name",
         u"Run a specific DirectShow test. Very verbose output, for debug only. "
         u"The default is none.");

#endif

    // Analyze command line options.
    analyze(argc, argv);
    tuner_args.loadArgs(duck, *this);
    extended = present(u"extended-info");

#if defined(TS_WINDOWS)
    // Test options on Windows. The legacy option "--enumerate-devices" means "--test enumerate-devices".
    if (present(u"list-devices")) {
        test_type = ts::DirectShowTest::LIST_DEVICES;
    }
    else if (present(u"enumerate-devices")) {
        test_type = ts::DirectShowTest::ENUMERATE_DEVICES;
    }
    else {
        getIntValue(test_type, u"test", ts::DirectShowTest::NONE);
    }
#endif

    exitOnError();
}


//----------------------------------------------------------------------------
//  This routine lists one tuner device.
//  If tuner_index >= 0, print it (Windows only).
//----------------------------------------------------------------------------

namespace {
    void ListTuner(ts::DuckContext& duck, ts::TunerBase& tuner, int tuner_index, Options& opt)
    {
        // If not opened, nothing to display.
        if (!tuner.isOpen()) {
            return;
        }

        // On Windows, since names are weird, always use quotes around tuner name.
        const char* quote = "";
#if defined(TS_WINDOWS)
        quote = "\"";
#endif

        // Display name.
        ts::UString margin;
        if (tuner_index >= 0) {
            margin = u"   ";
            std::cout << tuner_index << ": ";
        }
        std::cout << quote << tuner.deviceName() << quote;

        // Display tuner information.
        const ts::UString info(tuner.deviceInfo());
        std::cout << " (";
        if (!info.empty()) {
            std::cout << "\"" << info << "\", ";
        }
        std::cout << tuner.deliverySystems().toString() << ")" << std::endl;

        // Display verbose information
        if (opt.verbose() || opt.extended) {

            // Display device path.
            const ts::UString path(tuner.devicePath());
            if (!path.empty()) {
                std::cout << margin << "Device: " << path << std::endl;
            }

            // Display system-specific status (very verbose).
            std::cout << std::endl;
            tuner.displayStatus(std::cout, margin + u"  ", opt.extended);
            std::cout << std::endl;
        }
    }
}

//----------------------------------------------------------------------------
//  Program entry point
//----------------------------------------------------------------------------

int MainCode(int argc, char *argv[])
{
    Options opt(argc, argv);

#if defined(TS_WINDOWS)
    // Specific DirectShow tests on Windows.
    if (opt.test_type != ts::DirectShowTest::NONE) {
        ts::DirectShowTest ds(std::cout, opt);
        ds.runTest(opt.test_type);
        return EXIT_SUCCESS;
    }
#endif

    // List DVB tuner devices
    if (!opt.tuner_args.device_name.empty()) {
        // One device name specified.
        ts::Tuner tuner(opt.duck);
        if (opt.tuner_args.configureTuner(tuner)) {
            ListTuner(opt.duck, tuner, -1, opt);
        }
    }
    else {
        // List all tuners.
        ts::TunerPtrVector tuners;
        if (!ts::Tuner::GetAllTuners(opt.duck, tuners)) {
            return EXIT_FAILURE;
        }
        else if (tuners.empty()) {
            opt.error(u"no DVB device found");
        }
        else {
            if (opt.verbose()) {
                std::cout << std::endl;
            }
            for (size_t i = 0; i < tuners.size(); ++i) {
                ListTuner(opt.duck, *tuners[i], int(i), opt);
            }
        }
    }

    return opt.valid() ? EXIT_SUCCESS : EXIT_FAILURE;
}
