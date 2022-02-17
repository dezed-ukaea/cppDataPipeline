/*! **************************************************************************
 * @file FairDataPipeline/utilities/json.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File contains methods relating to handling of JSON based data
 *
 * This file contains methods which modify/change the form of data which is
 * held as a JSON string
 ****************************************************************************/
#ifndef __FDP_JSON_HXX__
#define __FDP_JSON_HXX__

#include <json/writer.h>

namespace FairDataPipeline {
/*! **************************************************************************
 * @brief convert a JSON object to string form
 * @author K. Zarebski (UKAEA)
 *
 * @param json_data JSON data held in a Json::Value object
 * @return the data as a JSON string
 *
 * @paragraph testcases Test Case
 *    `test/test_utilities.cxx`: TestJSONString
 *
 *    This unit test checks that the JSON string constructed from a
 *    Json::Value object matches expectation
 *    @snippet `test/test_utilities.cxx TestJSONString
 *
 ****************************************************************************/
std::string json_to_string(Json::Value &json_data);
}; // namespace FairDataPipeline

#endif