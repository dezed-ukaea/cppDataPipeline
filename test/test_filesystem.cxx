#include "gtest/gtest.h"
#include "scrc/registry/file_system.hxx"
#include "scrc/utilities/logging.hxx"
#include "scrc/objects/distributions.hxx"

using namespace SCRC;

TEST(SCRCAPITest, TestTOMLPERead)
{
    APILogger->set_level(spdlog::level::debug);
    const std::filesystem::path test_file = std::filesystem::path(TESTDIR) / std::filesystem::path("test_pe.toml");
    ASSERT_EQ(read_point_estimate(test_file), 11);
}

TEST(SCRCAPITest, TestTOMLDisRead)
{
    APILogger->set_level(spdlog::level::debug);
    const std::filesystem::path test_file = std::filesystem::path(TESTDIR) / std::filesystem::path("test_dis.toml");
    Normal dis(*read_distribution(test_file));
    ASSERT_EQ(dis.mean(), 2.675739);
    ASSERT_EQ(dis.standard_deviation(), 0.5719293);
}

TEST(SCRCAPITest, TestArrayRead)
{
    APILogger->set_level(spdlog::level::debug);
    const std::filesystem::path test_file = std::filesystem::path(TESTDIR) / std::filesystem::path("test_array.hdf5");
    const std::filesystem::path key = "/contact_matrices/home";

    ArrayObject<double> array_ = read_array<double>(test_file, key);

    ASSERT_EQ(array_.get({0, 0}), 0.4788127996331724);
}

TEST(SCRCAPITest, TestTableReadColumn)
{
    APILogger->set_level(spdlog::level::debug);
    const std::filesystem::path test_file = std::filesystem::path(TESTDIR) / std::filesystem::path("test_table.hdf5");
    const std::filesystem::path key = "/conversiontable/scotland/URcode";

    read_table_column<double>(test_file, key);

}