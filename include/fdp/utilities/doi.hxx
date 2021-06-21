/*! **************************************************************************
 * @file fdp/utilities/doi.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File contains methods relating to the Digital Object Identifier class
 *
 * This file contains classes to describe structures received from the RestAPI
 * and recognised by the local configuration file.
 ****************************************************************************/
#ifndef __DOI_FDP_HXX__
#define __DOI_FDP_HXX__

#include <regex>
#include <string>

#include "fdp/exceptions.hxx"
#include "fdp/utilities/logging.hxx"

namespace FDP {

/*! **************************************************************************
 * @class DOI
 * @brief class for handling of Digital Object Identifiers
 * @author K. Zarebski (UKAEA)
 *
 ****************************************************************************/
class DOI {
private:
  const std::string registrant_id_ = "";
  const std::string object_id_ = "";
  bool check_prefix_(std::string &prefix);
  bool check_suffix_(std::string &suffix);

public:
  /*! ***********************************************************************
   * @brief a construct an empty DOI instance
   * @author K. Zarebski (UKAEA)
   *
   *************************************************************************/
  DOI(){};

  /*! ***********************************************************************
   * @brief a construct an empty DOI instance
   * @author K. Zarebski (UKAEA)
   *
   * @param reg_id registrant identifier
   * @param obj_id object identifier
   *
   *************************************************************************/
  DOI(std::string reg_id, std::string obj_id)
      : registrant_id_(reg_id), object_id_(obj_id) {
    check_prefix_(reg_id);
    check_suffix_(obj_id);
  }

  /*! **********************************************************************
   * @brief Construct a new DOI object from an existing one
   * @author K. Zarebski (UKAEA)
   *
   * @param doi DOI instance to clone
   *************************************************************************/
  DOI(const DOI &doi)
      : registrant_id_(doi.registrant_id_), object_id_(doi.object_id_) {}

  /*! **********************************************************************
   * @brief convert a DOI object to string representation
   * @author K. Zarebski (UKAEA)
   *
   * @return DOI as a string
   ************************************************************************/
  std::string to_string() const { return registrant_id_ + "/" + object_id_; }

  /*! **********************************************************************
   * @brief set DOI to be equal to another
   * @author K. Zarebski (UKAEA)
   *
   * @param doi other DOI to copy
   * @return a new DOI matching the specified instance
   ************************************************************************/
  DOI operator=(const DOI &doi) { return DOI(registrant_id_, object_id_); }

  /*! **********************************************************************
   * @brief compare to DOI instances
   * @author K. Zarebski (UKAEA)
   *
   * @param a first instance
   * @param b second instance
   * @return true if objects match
   * @return false if objects do not match
   ************************************************************************/
  friend bool operator==(const DOI &a, const DOI &b) {
    return a.registrant_id_ == b.registrant_id_ && a.object_id_ == b.object_id_;
  }
};

/*! ***********************************************************************
 * @brief Construct a DOI object from a string representation
 * @author K. Zarebski (UKAEA)
 *
 * @param doi_id DOI as a string
 * @return DOI a DOI object representing the given DOI
 *************************************************************************/
DOI doi_from_string(std::string doi_id);
}; // namespace FDP

#endif