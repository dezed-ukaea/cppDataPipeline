#include "gtest/gtest.h"
#include "scrc/registry/file_system.hxx"
#include "scrc/objects/distributions.hxx"

using namespace SCRC;

TEST(SCRCAPITest, TestTOMLPERead)
{
    const std::filesystem::path test_file = std::filesystem::path(TESTDIR) / std::filesystem::path("test_pe.toml");
    ASSERT_EQ(read_point_estimate(test_file), 11);
}

TEST(SCRCAPITest, TestTOMLDisRead)
{
    const std::filesystem::path test_file = std::filesystem::path(TESTDIR) / std::filesystem::path("test_dis.toml");
    Normal dis(*read_distribution(test_file));
    ASSERT_EQ(dis.mean(), 2.675739);
    ASSERT_EQ(dis.standard_deviation(), 0.5719293);
}