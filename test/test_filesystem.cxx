#include "gtest/gtest.h"
#include "scrc/registry/file_system.hxx"

using namespace SCRC;

TEST(SCRCAPITest, TestTOMLPERead)
{
    const std::filesystem::path test_file = std::filesystem::path(TESTDIR) / std::filesystem::path("test_pe.toml");
    ASSERT_EQ(read_point_estimate(test_file), 11);
}