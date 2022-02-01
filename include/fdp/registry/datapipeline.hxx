/*! **************************************************************************
 * @file fdp/registry/datapipeline.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File containing classes for interaction with the FDP data pipeline
 *
 * The classes within this file form the core objects for handling interaction
 * with the FDP FAIR Data pipeline. They contain methods which parse
 * configurations and control calls to the RestAPI itself.
 ****************************************************************************/
#ifndef __FDP_DATAPIPELINE_IMP_HXX__
#define __FDP_DATAPIPELINE_IMP_HXX__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "json/json.h"

#include "fdp/fdp.hxx"

#include "fdp/objects/config.hxx"
#include "fdp/objects/metadata.hxx"
#include "fdp/registry/api.hxx"
#include "fdp/registry/data_object.hxx"
#include "fdp/registry/data_io.hxx"
#include "fdp/utilities/logging.hxx"

namespace FDP {

/*! **************************************************************************
 * @class DataPipelineImpl_
 * @brief private pointer-to-implementation class containing all backend methods
 * @author K. Zarebski (UKAEA)
 *
 * This class performs all behind the scenes operations acting as the backend to
 * the user called FDP::DataPipeline class.
 * This structure has been chosen to allow for unit tests to be created to test
 * all methods including private ones.
 *
 * @warning The class should not be used directly during implementation,
 * but rather via an FDP::DataPipeline instance.
 *
 *****************************************************************************/
}; // namespace FDP

#endif
