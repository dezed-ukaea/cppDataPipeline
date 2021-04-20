#ifndef __DOI_SCRC_HXX__
#define __DOI_SCRC_HXX__

#include <regex>
#include <string>

#include "scrc/exceptions.hxx"

namespace SCRC {
// Digital Object Identifier
class DOI {
private:
  const std::string registrant_id_ = "";
  const std::string object_id_ = "";
  bool check_prefix_(std::string &prefix);
  bool check_suffix_(std::string &suffix);

public:
  DOI(){};
  DOI(std::string reg_id, std::string obj_id)
      : registrant_id_(reg_id), object_id_(obj_id) {
    check_prefix_(reg_id);
    check_suffix_(obj_id);
  }

  DOI(const DOI &doi)
      : registrant_id_(doi.registrant_id_), object_id_(doi.object_id_) {}

  std::string to_string() const { return registrant_id_ + "/" + object_id_; }

  explicit operator bool() const {
    return !registrant_id_.empty() && !object_id_.empty();
  }

  DOI operator=(const DOI &doi) { return DOI(registrant_id_, object_id_); }

  friend bool operator==(const DOI &a, const DOI &b) {
    return a.registrant_id_ == b.registrant_id_ && a.object_id_ == b.object_id_;
  }
};

DOI doi_from_string(std::string doi_id);
}; // namespace SCRC

#endif