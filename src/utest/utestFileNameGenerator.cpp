//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  TSUnit test suite for FileNameGenerator class.
//
//----------------------------------------------------------------------------

#include "tsFileNameGenerator.h"
#include "tsunit.h"


//----------------------------------------------------------------------------
// The test fixture
//----------------------------------------------------------------------------

class FileNameGeneratorTest: public tsunit::Test
{
public:
    virtual void beforeTest() override;
    virtual void afterTest() override;

    void testDefault();
    void testCounter();
    void testDateTime();

    TSUNIT_TEST_BEGIN(FileNameGeneratorTest);
    TSUNIT_TEST(testDefault);
    TSUNIT_TEST(testCounter);
    TSUNIT_TEST(testDateTime);
    TSUNIT_TEST_END();
};

TSUNIT_REGISTER(FileNameGeneratorTest);


//----------------------------------------------------------------------------
// Initialization.
//----------------------------------------------------------------------------

// Test suite initialization method.
void FileNameGeneratorTest::beforeTest()
{
}

// Test suite cleanup method.
void FileNameGeneratorTest::afterTest()
{
}


//----------------------------------------------------------------------------
// Unitary tests.
//----------------------------------------------------------------------------

void FileNameGeneratorTest::testDefault()
{
    ts::FileNameGenerator gen;
    TSUNIT_EQUAL(u"000000", gen.newFileName());
    TSUNIT_EQUAL(u"000001", gen.newFileName());
    TSUNIT_EQUAL(u"000002", gen.newFileName());
    TSUNIT_EQUAL(u"000003", gen.newFileName(ts::Time::CurrentLocalTime()));
}

void FileNameGeneratorTest::testCounter()
{
    ts::FileNameGenerator gen;

    gen.initCounter(u"base.ext", 1234, 7);
    TSUNIT_EQUAL(u"base-0001234.ext", gen.newFileName());
    TSUNIT_EQUAL(u"base-0001235.ext", gen.newFileName());
    TSUNIT_EQUAL(u"base-0001236.ext", gen.newFileName(ts::Time::CurrentLocalTime()));

    gen.initCounter(u"foo/bar/base.ext", 1234, 7);
    TSUNIT_EQUAL(u"foo/bar/base-0001234.ext", gen.newFileName());
    TSUNIT_EQUAL(u"foo/bar/base-0001235.ext", gen.newFileName());
    TSUNIT_EQUAL(u"foo/bar/base-0001236.ext", gen.newFileName(ts::Time::CurrentLocalTime()));

    gen.initCounter(u"foo056.bar", 3, 7);
    TSUNIT_EQUAL(u"foo056.bar", gen.newFileName());
    TSUNIT_EQUAL(u"foo057.bar", gen.newFileName());
    TSUNIT_EQUAL(u"foo058.bar", gen.newFileName());
    TSUNIT_EQUAL(u"foo059.bar", gen.newFileName(ts::Time::CurrentLocalTime()));

    gen.initCounter(u"base..ext", 12, 4);
    TSUNIT_EQUAL(u"base.0012.ext", gen.newFileName());
    TSUNIT_EQUAL(u"base.0013.ext", gen.newFileName());
    TSUNIT_EQUAL(u"base.0014.ext", gen.newFileName());
}

void FileNameGeneratorTest::testDateTime()
{
    ts::FileNameGenerator gen;

    gen.initDateTime(u"base.ext");
    TSUNIT_EQUAL(u"base-20210321-121314.ext", gen.newFileName(ts::Time(2021, 03, 21, 12, 13, 14, 521)));

    gen.initDateTime(u"foo/bar/base.ext");
    TSUNIT_EQUAL(u"foo/bar/base-20210321-121314.ext", gen.newFileName(ts::Time(2021, 03, 21, 12, 13, 14, 521)));

    gen.initDateTime(u"base.ext", ts::Time::ALL);
    TSUNIT_EQUAL(u"base-20210321-121314521.ext", gen.newFileName(ts::Time(2021, 03, 21, 12, 13, 14, 521)));

    gen.initDateTime(u"foo.202101-1812.bar");
    TSUNIT_EQUAL(u"foo.202103-1213.bar", gen.newFileName(ts::Time(2021, 03, 21, 12, 13, 14, 521)));
}
