//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2017, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//
//  CppUnit test suite for class ts::Guard
//
//----------------------------------------------------------------------------

#include "tsGuard.h"
#include "tsSysUtils.h"
#include "utestCppUnitTest.h"
TSDUCK_SOURCE;


//----------------------------------------------------------------------------
// The test fixture
//----------------------------------------------------------------------------

class GuardTest: public CppUnit::TestFixture
{
public:
    virtual void setUp() override;
    virtual void tearDown() override;

    void testGuard();
    void testAcquireFailed();
    void testReleaseFailed();

    CPPUNIT_TEST_SUITE(GuardTest);
    CPPUNIT_TEST(testGuard);
    CPPUNIT_TEST_EXCEPTION(testAcquireFailed, ts::Guard::GuardError);
    CPPUNIT_TEST(testReleaseFailed);
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GuardTest);


//----------------------------------------------------------------------------
// A mutex class which counts acquire & release.
// Can also voluntarily fail on acquire and/or release.
//----------------------------------------------------------------------------

namespace {
    class MutexTest: public ts::MutexInterface
    {
    private:
        int _count;
        const bool _acquireResult;
        const bool _failResult;
    public:
        // Constructor
        MutexTest(bool acquireResult = true, bool failResult = true) :
            _count(0),
            _acquireResult(acquireResult),
            _failResult(failResult)
        {
        }

        // Get the count
        int count() const
        {
            return _count;
        }

        // Acquire the mutex. Block until granted.
        // Return true on success and false on error.
        virtual bool acquire(ts::MilliSecond timeout = ts::Infinite) override
        {
            _count++;
            return _acquireResult;
        }

        // Release the mutex.
        // Return true on success and false on error.
        virtual bool release() override
        {
            _count--;
            return _failResult;
        }
    private:
        // Inaccessible operations
        MutexTest(const MutexTest&) = delete;
        MutexTest& operator=(const MutexTest&) = delete;
    };
}


//----------------------------------------------------------------------------
// Initialization.
//----------------------------------------------------------------------------

// Test suite initialization method.
void GuardTest::setUp()
{
}

// Test suite cleanup method.
void GuardTest::tearDown()
{
}


//----------------------------------------------------------------------------
// Unitary tests.
//----------------------------------------------------------------------------

// Test case: basic locking
void GuardTest::testGuard()
{
    MutexTest mutex;
    CPPUNIT_ASSERT(mutex.count() == 0);
    {
        ts::Guard gard1(mutex);
        CPPUNIT_ASSERT(mutex.count() == 1);
        {
            ts::Guard gard2(mutex);
            CPPUNIT_ASSERT(mutex.count() == 2);
        }
        CPPUNIT_ASSERT(mutex.count() == 1);
    }
    CPPUNIT_ASSERT(mutex.count() == 0);
}

// Test case: acquire() error is properly handled.
void GuardTest::testAcquireFailed()
{
    MutexTest mutex(false, true);
    CPPUNIT_ASSERT(mutex.count() == 0);
    {
        ts::Guard gard(mutex);
        CPPUNIT_FAIL("mutex.acquire() passed, should not get there");
    }
}

// Test case: release() error is properly handled.
void GuardTest::testReleaseFailed()
{
    if (ts::EnvironmentExists("UTEST_FATAL_CRASH_ALLOWED")) {
        std::cerr << "FatalTest: Guard destructor should fail !" << std::endl
                  << "Unset UTEST_FATAL_CRASH_ALLOWED to skip the crash test" << std::endl;
        MutexTest mutex(true, false);
        CPPUNIT_ASSERT(mutex.count() == 0);
        {
            ts::Guard gard(mutex);
            CPPUNIT_ASSERT(mutex.count() == 1);
        }
        CPPUNIT_FAIL("mutex.release() passed, should not get there");
    }
    else {
        utest::Out() << "FatalTest: crash test for failing Guard destructor skipped, define UTEST_FATAL_CRASH_ALLOWED to force it" << std::endl;
    }
}
