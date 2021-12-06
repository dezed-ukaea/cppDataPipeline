#ifndef TESTDIR
#define TESTDIR ""
#endif
#include "fdp/objects/api_object.hxx"
#include "gtest/gtest.h"

#include "fdp/utilities/logging.hxx"

using namespace FDP;

//! [TestGetId]
TEST(FDPAPITest, TestGetId) {
  ApiObject testObj = ApiObject("http://127.0.0.1/object/1");
  ASSERT_EQ(testObj.get_id(), 1);
}
//! [TestGetId]

//! [TestGetType]
TEST(FDPAPITest, TestGetType) {
  ApiObject testObj = ApiObject("http://127.0.0.1/object/1");
  ASSERT_EQ(testObj.get_type(), "object");
}
//! [TestGetType]