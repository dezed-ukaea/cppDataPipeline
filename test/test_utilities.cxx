#ifndef TESTDIR
#define TESTDIR ""
#endif
#include "fdp/exceptions.hxx"
#include "fdp/utilities/json.hxx"
#include "fdp/utilities/semver.hxx"
#include "fdp/objects/metadata.hxx"
#include "gtest/gtest.h"

#include "json/reader.h"

using namespace FairDataPipeline;

TEST(FDPAPITest, TestSemVerComparisons) {
  Versioning::version v1{1, 4, 5};
  Versioning::version v2;
  Versioning::version v3{1, 0, 0};
  Versioning::version v4{5, 2, 1};
  Versioning::version v3a(v3);
  v3a.alpha();

  ASSERT_LT(v2, v1);
  ASSERT_LT(v1, v4);
  ASSERT_LT(v3a, v3);

  v2.bump_major();
  v3.bump_minor();

  ASSERT_EQ(v2, v3);
  ASSERT_EQ(Versioning::version("5.2.1"), v4);
}

//! [TestJSONString]
TEST(FDPAPITest, TestJSONString) {
  Json::Value value_;
  value_["int"] = 5;
  value_["string"] = "hello";
  const std::string expected_ =
      "{\n\t\"int\" : 5,\n\t\"string\" : \"hello\"\n}";
  ASSERT_EQ(json_to_string(value_), expected_);
}
//! [TestJSONString]

TEST(FDAPITest, TestRandomHash) {
  // Use a set to store 1001 random hashes and ensure they are unique
  std::set<std::string> unique_random_hashes;
  for(int i = 0; i <=1000; i++){
    std::string random_hash_ = generate_random_hash();
    unique_random_hashes.insert(random_hash_);
  }
  ASSERT_EQ(unique_random_hashes.size(), 1001);
}

TEST(FDAPITest, TestRemoveLocalFromRoot) {
  ASSERT_EQ(remove_local_from_root(std::string("file://test")), "test");
}