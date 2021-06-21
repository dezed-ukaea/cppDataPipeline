/*! **************************************************************************
 * @file fdp/utilities/semver.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File contains classes and types for validating semantic versioning
 *
 * This file contains all classes and types required to ensure that data
 * objects follow strict semantic versioning. This is important for tracking
 * which values are used during code runs.
 ****************************************************************************/

#ifndef __FDP_SEMVER_HXX__
#define __FDP_SEMVER_HXX__

#include <ostream>
#include <string>
#include <vector>

namespace FDP {
/*! **************************************************************************
 * @namespace FDP::Versioning
 * @brief namespace containing all semantic versioning types and classes
 * @author K. Zarebski (UKAEA)
 *
 ****************************************************************************/
namespace Versioning {

/*! **************************************************************************
 * @enum meta_tag
 * @brief the allowed tags for versions describing version state
 * @author K. Zarebski (UKAEA)
 *
 * This enum class contains all permitted release state classifiers which
 * describe whether the code/data object is a full release or still under
 * review by the authors
 *
 ****************************************************************************/
enum class meta_tag {
  ALPHA,             /*!< Release is an Alpha release */
  BETA,              /*!< Release is an Beta release */
  RELEASE_CANDIDATE, /*!< Release is a candidate for full release */
  RELEASE            /*!< Release is a full release */
};

/*! **************************************************************************
 * @brief class describing the semantic version of an object
 * @author K. Zarebski (UKAEA)
 *
 * This class allows the comparison and validation of semantic version numbers
 * and ensures that all data objects handled follow the same protocol
 *
 ****************************************************************************/
class version {
public:
  /*! ************************************************************************
   * @brief Construct a new version with the default start value of `0.1.0`
   * @author K. Zarebski (UKAEA)
   *
   **************************************************************************/
  version() = default;

  /*! ************************************************************************
   * @brief Construct a new version from integer representation
   *
   * Constructs a version object with the forms:
   *
   * `MAJOR.MINOR.PATCH-<tag>.<tag-version>+<meta-data>`
   *
   * where parts in `<>` are optional. The default for `tag_v` of `1E9` is
   * used to, as a default, represent a version that is a full release
   *
   * @param major the MAJOR release number
   * @param minor the MINOR release number
   * @param patch the PATCH release number
   * @param tag if applicable, whether release is alpha, beta etc.
   * @param tag_v if applicable, the version of the tag, e.g. `-alpha.2`
   * @param meta_data if applicable, additional metadata, e.g. `+<build_num>`
   *
   **************************************************************************/
  version(unsigned int major, unsigned int minor, unsigned int patch,
          meta_tag tag = meta_tag::RELEASE, unsigned int tag_v = 1E9,
          std::string meta_data = "")
      : major_(major), minor_(minor), patch_(patch), meta_tag_(tag),
        tag_v_(tag_v), meta_data_(meta_data) {}

  version(const std::string version_str);

  version(const version &v)
      : major_(v.major_), minor_(v.minor_), patch_(v.patch_),
        meta_tag_(v.meta_tag_), tag_v_(v.tag_v_), meta_data_(v.meta_data_) {}

  void bump_major() { major_ += 1; }
  void bump_minor() { minor_ += 1; }
  void bump_patch() { patch_ += 1; }

  void alpha() {
    tag_v_ = (meta_tag_ == meta_tag::ALPHA) ? tag_v_ + 1 : 0;
    meta_tag_ = meta_tag::ALPHA;
  }

  void beta() {
    tag_v_ = (meta_tag_ == meta_tag::BETA) ? tag_v_ + 1 : 0;
    meta_tag_ = meta_tag::BETA;
  }

  void prerelease() {
    tag_v_ = (meta_tag_ == meta_tag::RELEASE_CANDIDATE) ? tag_v_ + 1 : 0;
    meta_tag_ = meta_tag::RELEASE_CANDIDATE;
  }

  void release() {
    tag_v_ = 1E9;
    meta_tag_ = meta_tag::RELEASE;
  }

  std::string to_string() const {
    std::string out_str_;

    out_str_ += std::to_string(major_) + ".";
    out_str_ += std::to_string(minor_) + ".";
    out_str_ += std::to_string(patch_);

    if (meta_tag_ != meta_tag::RELEASE) {
      out_str_ += "-" + get_tag_str();

      if (tag_v_ != 1E9) {
        out_str_ += std::to_string(tag_v_);
      }
    }
    if (!meta_data_.empty()) {
      out_str_ += "+" + meta_data_;
    }

    return out_str_;
  }

  friend std::ostream &operator<<(std::ostream &os, const version &v) {
    os << v.to_string();
    return os;
  }

  friend bool operator<(const version &v1, const version &v2) {
    if (v1.major_ < v2.major_)
      return true;
    else if (v2.major_ < v1.major_)
      return false;
    if (v1.minor_ < v2.minor_)
      return true;
    else if (v2.minor_ < v1.minor_)
      return false;
    if (v1.patch_ < v2.patch_)
      return true;
    else if (v2.patch_ < v1.patch_)
      return false;
    if (v1.meta_tag_ < v2.meta_tag_)
      return true;
    else if (v2.meta_tag_ < v1.meta_tag_)
      return false;
    if (v1.tag_v_ < v2.tag_v_)
      return true;
    else if (v2.tag_v_ < v1.tag_v_)
      return false;
    return false;
  }

  friend bool operator>(const version &v1, const version &v2) {
    return v2 < v1;
  }

  friend bool operator==(const version &v1, const version &v2) {
    bool same_ = v1.major_ == v2.major_;
    same_ += v1.minor_ == v2.minor_;
    same_ += v1.patch_ == v2.patch_;
    same_ += v1.meta_tag_ == v2.meta_tag_;
    same_ += v1.tag_v_ == v2.tag_v_;
    same_ += v1.meta_data_ == v2.meta_data_;
    return same_;
  }

  friend bool operator!=(const version &v1, const version &v2) {
    return !(v1 == v2);
  }

  friend bool operator<=(const version &v1, const version &v2) {
    return (v1 == v2) || v1 < v2;
  }

  friend bool operator>=(const version &v1, const version &v2) {
    return (v1 == v2) || v1 > v2;
  }

private:
  unsigned int major_ = 0;
  unsigned int minor_ = 1;
  unsigned int patch_ = 0;
  meta_tag meta_tag_ = meta_tag::RELEASE;
  unsigned int tag_v_ = 1E9;
  std::string meta_data_ = "";
  std::string get_tag_str() const;
};
}; // namespace Versioning
}; // namespace FDP

#endif