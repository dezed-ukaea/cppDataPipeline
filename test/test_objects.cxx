#include "scrc/objects/data.hxx"
#include "gtest/gtest.h"

using namespace SCRC;

TEST(SCRCAPITest, TestDataTableCreate) {
  DataTable *data_table_ = new DataTable;
  data_table_->add_column<std::string>("Name",
                                       {"John", "James", "David", "George"});
  data_table_->add_column<int>("Age", {34, 56, 54, 23});
  data_table_->add_column<float>("Height", {182.3, 183.4, 179.1, 175.9}, "cm");
  ASSERT_EQ((*data_table_->get_string_column("Name"))[2], "David");
  std::cout << data_table_->print() << std::endl;
}