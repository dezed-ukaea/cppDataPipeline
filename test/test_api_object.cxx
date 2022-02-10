#ifndef TESTDIR
#define TESTDIR ""
#endif
#include "fdp/objects/api_object.hxx"
#include "gtest/gtest.h"

#include "fdp/utilities/json.hxx"
#include "fdp/utilities/logging.hxx"

using namespace FDP;

class ApiObjectTest : public ::testing::Test {
protected:
  void SetUp() override {
    
  }

  static ApiObject::uptr get_obj() {

      ApiObject::uptr test_object = ApiObject::construct();

      test_object->add(  "url", std::string("http://127.0.0.1:8000/api/object/1") );
      test_object->add( "name", std::string("Test Name") );
      test_object->add( "string_value", std::string("value") );
      test_object->add( "int_value",  5 );

      return test_object;
  }

  ApiObject::uptr test_obj = get_obj();
  
  void TearDown() override {}
};

TEST_F(ApiObjectTest, testGetType) {
  ASSERT_EQ(test_obj->get_type(), std::string("object"));
}

TEST_F(ApiObjectTest, testGetIdFromString) {
  ASSERT_EQ(test_obj->get_id(), 1);
}

TEST_F(ApiObjectTest, testGetUri) {
  ASSERT_EQ(test_obj->get_uri(), std::string("http://127.0.0.1:8000/api/object/1"));
}

TEST_F(ApiObjectTest, testGetValueAsString) {
  ASSERT_EQ(test_obj->get_value_as_string("string_value"), std::string("value"));
}

TEST_F(ApiObjectTest, testGetValueAsInt) {
  ASSERT_EQ(test_obj->get_value_as_int("int_value"), int(5));
}

TEST_F(ApiObjectTest, testIsEmpty){
  //Json::Value empty_;
    ApiObject::uptr test_empty_obj = ApiObject::construct();
  ASSERT_TRUE(test_empty_obj->is_empty());
}

