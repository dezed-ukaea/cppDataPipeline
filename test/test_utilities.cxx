#include "gtest/gtest.h"
#include "scrc/utilities/doi.hxx"
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