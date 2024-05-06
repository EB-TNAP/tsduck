//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  TSUnit test suite for singletons.
//
//----------------------------------------------------------------------------

#include "tsSingleton.h"
#include "tsunit.h"


//----------------------------------------------------------------------------
// The test fixture
//----------------------------------------------------------------------------

class SingletonTest: public tsunit::Test
{
    TSUNIT_DECLARE_TEST(Singleton);
    TSUNIT_DECLARE_TEST(NoInitializer);
    TSUNIT_DECLARE_TEST(InitializerTwoArgs);
};

TSUNIT_REGISTER(SingletonTest);


//----------------------------------------------------------------------------
// Singleton class
//----------------------------------------------------------------------------

namespace {
    class Singleton
    {
        TS_DECLARE_SINGLETON(Singleton);
    };

    TS_DEFINE_SINGLETON(Singleton);

    Singleton::Singleton()
    {
        tsunit::Test::debug() << "SingletonTest: constructor of the singleton" << std::endl;
    }
}

//----------------------------------------------------------------------------
// Unitary tests.
//----------------------------------------------------------------------------

TSUNIT_DEFINE_TEST(Singleton)
{
    // Check that this is a singleton
    Singleton& p1(Singleton::Instance());
    Singleton& p2(Singleton::Instance());
    TSUNIT_ASSERT(&p1 == &p2);
}

// Static instance, no initializer
TS_STATIC_INSTANCE(std::string, (), Foo1);

TSUNIT_DEFINE_TEST(NoInitializer)
{
    debug() << "SingletonTest: Foo1::Instance() = \"" << Foo1::Instance() << "\"" << std::endl;

    // Check the value
    TSUNIT_ASSERT(Foo1::Instance().empty());

    // Check that this is a singleton
    std::string* p1(&Foo1::Instance());
    std::string* p2(&Foo1::Instance());
    TSUNIT_ASSERT(p1 == p2);
}

// Static instance, initializer with two parameters
TS_STATIC_INSTANCE(std::string, (4, '='), Foo2);

TSUNIT_DEFINE_TEST(InitializerTwoArgs)
{
    debug() << "StaticInstanceTest: Foo2::Instance() = \"" << Foo2::Instance() << "\"" << std::endl;

    // Check the value
    TSUNIT_EQUAL("====", Foo2::Instance());

    // Check that this is a singleton
    std::string* p1(&Foo2::Instance());
    std::string* p2(&Foo2::Instance());
    TSUNIT_ASSERT(p1 == p2);
}
