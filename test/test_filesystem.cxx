#include "scrc/objects/distributions.hxx"
#include "scrc/registry/file_system.hxx"
#include "scrc/utilities/logging.hxx"
#include "scrc/objects/data.hxx"
#include "gtest/gtest.h"
#include "H5Cpp.h"

#include <vector>
#include <stdio.h>
#include <stdlib.h>


using namespace SCRC;

TEST(SCRCAPITest, TestTOMLPERead) {
  APILogger->set_level(spdlog::level::debug);
  const std::filesystem::path test_file =
      std::filesystem::path(TESTDIR) / std::filesystem::path("test_pe.toml");
  ASSERT_EQ(read_point_estimate_from_toml(test_file), 11);
}

TEST(SCRCAPITest, TestTOMLDisRead) {
  APILogger->set_level(spdlog::level::debug);
  const std::filesystem::path test_file =
      std::filesystem::path(TESTDIR) / std::filesystem::path("test_dis.toml");
  Normal dis(*read_distribution_from_toml(test_file));
  ASSERT_EQ(dis.mean(), 2.675739);
  ASSERT_EQ(dis.standard_deviation(), 0.5719293);
}

TEST(SCRCAPITest, TestArrayRead) {
  APILogger->set_level(spdlog::level::debug);
  const std::filesystem::path test_file =
      std::filesystem::path(TESTDIR) / std::filesystem::path("test_array.h5");
  const std::filesystem::path key = "/contact_matrices/home";

  ArrayObject<double> *array_ = read_array<double>(test_file, key);

  ASSERT_EQ(array_->get_title(0), "rowvalue");
  ASSERT_EQ(array_->get_dimension_names(0)[2], "10-14");
  ASSERT_EQ(array_->get({0, 0}), 0.4788127996331724);
}

TEST(SCRCAPITest, TestTableReadColumn) {
  APILogger->set_level(spdlog::level::debug);
  const std::filesystem::path test_file =
      std::filesystem::path(TESTDIR) / std::filesystem::path("test_table.h5");
  const std::filesystem::path key = "/conversiontable/scotland";
  const std::string column = "URcode";

  DataTableColumn<double> *column_ =
      read_table_column<double>(test_file, key, column);

  ASSERT_EQ(column_->operator[]("2"), 5.0);
}

TEST(SCRCAPITest, TestWriteArray) {
  APILogger->set_level(spdlog::level::debug);
  std::filesystem::path config_path_ =
      std::filesystem::path(TESTDIR) / "config.yaml";
  LocalFileSystem* file_system_ = new LocalFileSystem(config_path_);
  std::filesystem::path data_product_ = "test_writeout";
  std::filesystem::path component_ = "nd_array";
  std::vector<std::string> titles_ = {"dim_1", "dim_2", "dim_3"};
  std::vector<std::vector<std::string>> dim_names_ = {
    {"a1", "a2", "a3"}, {"b1", "b2", "b3"},
    {"c1", "c2", "c3"}
  };

  std::vector<int> elements_ = {
    1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27
  };

  std::vector<int> dimensions_ = {3, 3, 3};

  ArrayObject<int>* arr_ = new ArrayObject<int>(titles_, dim_names_, dimensions_, elements_);

  const std::filesystem::path output_file_ = write_array(arr_, data_product_, component_, file_system_);

  ASSERT_TRUE(std::filesystem::exists(output_file_));
  const std::string cmd_ = "h5dump -n "+output_file_.string();
  system(cmd_.c_str());
  ASSERT_NO_THROW(read_array<int>(output_file_, ""));
}