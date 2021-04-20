#ifndef __SCRC_DISTRIBUTION_HXX__
#define __SCRC_DISTRIBUTION_HXX__

#include <map>
#include <ostream>
#include <random>
#include <string>

namespace SCRC {
class Distribution {
public:
  Distribution(std::string name, std::map<std::string, double> params)
      : name_(name), parameters_(params) {}
  double generate(std::mt19937 *gen) { return 1; }
  double get_param(std::string param_name) const {
    return parameters_.at(param_name);
  }
  std::map<std::string, double> get_params() const { return parameters_; }

protected:
  const std::map<std::string, double> parameters_;
  const std::string name_;
  friend std::ostream &operator<<(std::ostream &os, const Distribution &d) {
    os << d.name_ << "(";
    for (const auto &[k, v] : d.parameters_) {
      os << k << "=" << v << ",";
    }

    os << ")";

    return os;
  }

  friend bool operator==(Distribution &d1, Distribution &d2) {
    bool same_ = d1.name_ == d2.name_;
    same_ += d1.parameters_ == d2.parameters_;
    return same_;
  }
};

class Normal : public Distribution {
public:
  Normal(double mean, double sd)
      : Distribution("normal", {{"mean", mean}, {"sd", sd}}) {}
  Normal(Distribution &dis) : Distribution("normal", dis.get_params()) {}
  double mean() const { return parameters_.at("mean"); }
  double standard_deviation() const { return parameters_.at("sd"); }
  double generate(std::mt19937 *gen) {
    std::normal_distribution<> dis_{mean(), standard_deviation()};
    return dis_(*gen);
  };
};

class Gamma : public Distribution {
public:
  Gamma(double k, double theta)
      : Distribution("gamma", {{"k", k}, {"theta", theta}}) {}
  Gamma(Distribution &dis) : Distribution("gamma", dis.get_params()) {}
  double k() const { return parameters_.at("k"); }
  double theta() const { return parameters_.at("theta"); }
  double generate(std::mt19937 *gen) {
    std::gamma_distribution<> dis_{k(), 1. / theta()};
    return dis_(*gen);
  }
};
}; // namespace SCRC

#endif