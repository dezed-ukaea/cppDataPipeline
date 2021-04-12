#include "gtest/gtest.h"
#include "scrc/utilities/doi.hxx"
#include "scrc/utilities/semver.hxx"
#include "scrc/exceptions.hxx"

using namespace SCRC;

TEST(SCRCAPITest, TestDOIPass)
{
    ASSERT_NO_THROW(doi_from_string("10.1111/ddi.12887"));
}

TEST(SCRCAPITest, TestDOIFail)
{
    ASSERT_THROW(doi_from_string("10.Asdfa/eya/ddi7"), validation_error);
}

TEST(SCRCAPITest, TestSemVerComparisons)
{
    version v1{1, 4, 5};
    version v2;
    version v3{1, 0, 0};
    version v4{5, 2, 1};
    version v3a(v3);
    v3a.alpha();

    ASSERT_LT(v2, v1);
    ASSERT_LT(v1, v4);
    ASSERT_LT(v3a, v3);

    v2.bump_major();

    ASSERT_EQ(v2, v3);
    ASSERT_EQ(version("5.2.1"), v4);
}