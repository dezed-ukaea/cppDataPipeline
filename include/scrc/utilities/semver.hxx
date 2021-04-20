#ifndef __SCRC_SEMVER_HXX__
#define __SCRC_SEMVER_HXX__

#include <ostream>
#include <string>
#include <vector>

namespace SCRC {
enum class meta_tag { ALPHA, BETA, RELEASE_CANDIDATE, RELEASE };

class version {
public:
  version() = default;

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

  friend std::ostream &operator<<(std::ostream &os, const version &v) {
    os << std::to_string(v.major_) << ".";
    os << std::to_string(v.minor_) << ".";
    os << std::to_string(v.patch_);

    if (v.meta_tag_ != meta_tag::RELEASE) {
      os << "-" << v.get_tag_str();

      if (v.tag_v_ != 1E9) {
        os << std::to_string(v.tag_v_);
      }
    }
    if (!v.meta_data_.empty()) {
      os << "+" << v.meta_data_;
    }

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
}; // namespace SCRC

#endif