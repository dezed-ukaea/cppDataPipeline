/*! **************************************************************************
 * @file fdp/objects/distributions.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-05
 * @brief File containing distribution object classes for storing API objects
 *
 * The classes within this file are used to store objects read from the FDP
 * API, holding distribution data
 ****************************************************************************/
#ifndef __FDP_DISTRIBUTION_HXX__
#define __FDP_DISTRIBUTION_HXX__

#include <map>
#include <ostream>
#include <random>
#include <string>

namespace FDP {
/*! **************************************************************************
 * @class Distribution
 * @brief represents a distribution with defined parameters which can be
 * used to generate values.
 * @author K. Zarebski (UKAEA)
 *
 * The Distribution class can be used to generate a random value from a
 * distribution contained within the standard library. It provides a container
 * into which a distribution object retrieved from the FDP pipeline
 * can be stored. This class forms the base class for the various types.
 ****************************************************************************/
class Distribution {
public:
  /*! ************************************************************************
   * @brief construct a distribution from a given set of parameters
   * @author K. Zarebski (UKAEA)
   *
   * @param name label for the distribution
   * @param params a map containing all parameters associated with the
   * distribution as defined by the relevant FDP pipeline schema
   **************************************************************************/
  Distribution(std::string name, std::map<std::string, double> params)
      : name_(name), parameters_(params) {}

  /*! ************************************************************************
   * @brief generate a value using the distribution
   * @author K. Zarebski (UKAEA)
   *
   * @param gen generator instance to used for random number generation
   * @return value of 1, as this is a base class
   **************************************************************************/
  double generate(std::mt19937 *gen) { return 1; }

  /*! ************************************************************************
   * @brief retrieve the value for the given distribution parameter
   * @author K. Zarebski (UKAEA)
   *
   * @param param_name name of the parameter
   * @return the parameter value
   **************************************************************************/
  double get_param(std::string param_name) const {
    return parameters_.at(param_name);
  }

  /*! ************************************************************************
   * @brief retrieve the name of the distribution
   * @author K. Zarebski (UKAEA)
   *
   * @return distribution name
   **************************************************************************/
  std::string get_name() const { return name_; }

  /*! ************************************************************************
   * @brief Retrieve the parameters of the distribution
   * @author K. Zarebski (UKAEA)
   *
   * @return map containing parameter values
   **************************************************************************/
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

/*! **************************************************************************
 * @class Normal
 * @brief normal (or gaussian) distribution
 * @author K. Zarebski (UKAEA)
 *
 * A subclass of Distribution that represents a normal distribution
 * defined as:
 * @f[
 * f(x;\mu, \sigma) =
 *\frac{1}{\sigma\sqrt{2\pi}}\exp{\left(-\frac{1}{2}\left(\frac{x-\mu}{\sigma}\right)^2\right)}
 * @f]
 * within the standard library
 * <a
 *href="https://en.cppreference.com/w/cpp/numeric/random/normal_distribution">normal_distribution</a>.
 *
 ****************************************************************************/
class Normal : public Distribution {
public:
  /*! ************************************************************************
   * @brief Construct a Normal distribution with the given parameters
   * @author K. Zarebski (UKAEA)
   *
   * @param mean the mean @f$\mu@f$ of the distribution
   * @param sd the standard deviation @f$\sigma@f$ of the distribution
   **************************************************************************/
  Normal(double mean, double sd)
      : Distribution("normal", {{"mu", mean}, {"sigma", sd}}) {}

  /*! ************************************************************************
   * @brief Construct a Normal distribution from a given Distribution object
   * @author K. Zarebski (UKAEA)
   *
   * @param dis distribution to extract parameters from
   **************************************************************************/
  Normal(Distribution &dis) : Distribution("normal", dis.get_params()) {}

  /*! ************************************************************************
   * @brief retrieve the mean, @f$\mu@f$ for the distribution
   * @author K. Zarebski (UKAEA)
   *
   * @return the mean of the distribution
   **************************************************************************/
  double mean() const { return parameters_.at("mu"); }

  /*! ************************************************************************
   * @brief retrieve the standard deviation, @f$\sigma@f$ for the distribution
   * @author K. Zarebski (UKAEA)
   *
   * @return the standard deviation of the distribution
   **************************************************************************/
  double standard_deviation() const { return parameters_.at("sigma"); }

  /*! ************************************************************************
   * @brief generate a random value from the distribution
   * @author K. Zarebski (UKAEA)
   *
   * @param gen generator instance to use for seeding
   * @return double a randomly generated value
   **************************************************************************/
  double generate(std::mt19937 *gen) {
    std::normal_distribution<> dis_{mean(), standard_deviation()};
    return dis_(*gen);
  };
};

/*! **************************************************************************
 * @class Gamma
 * @brief gamma distribution
 * @author K. Zarebski (UKAEA)
 *
 * A subclass of Distribution that represents a gamma distribution
 * defined as:
 * @f[
 * p(x | k, \theta) = \frac{\theta^{k}e^{-x\theta}}{\Gamma(k)}x^{k-1}
 * @f]
 * within the standard library
 * <a
 *href="https://en.cppreference.com/w/cpp/numeric/random/gamma_distribution">gamma_distribution</a>.
 * Where the parameterisation is:
 * @f[
 *  k = \alpha,\quad\theta = 1/\beta
 * @f]
 *
 ****************************************************************************/
class Gamma : public Distribution {
public:
  /*! ************************************************************************
   * @brief Construct a new Gamma distribution with the given parameters
   * @author K. Zarebski (UKAEA)
   *
   * @param k shape parameter @f$k@f$
   * @param theta scale parameter, @f$\theta@f$
   **************************************************************************/
  Gamma(double k, double theta)
      : Distribution("gamma", {{"k", k}, {"theta", theta}}) {}

  /*! ************************************************************************
   * @brief Construct a Gamma distribution from a given Distribution object
   * @author K. Zarebski (UKAEA)
   *
   * @param dis distribution to extract parameters from
   **************************************************************************/
  Gamma(Distribution &dis) : Distribution("gamma", dis.get_params()) {}

  /*! ************************************************************************
   * @brief retrieve the value of the shape parameter, @f$k@f$.
   * @author K. Zarebski (UKAEA)
   *
   * @return the shape parameter value
   **************************************************************************/
  double k() const { return parameters_.at("k"); }

  /*! ************************************************************************
   * @brief retrieve the value of the scale parameter, @f$\theta@f$.
   * @author K. Zarebski (UKAEA)
   *
   * @return the scale parameter value
   **************************************************************************/
  double theta() const { return parameters_.at("theta"); }

  /*! ************************************************************************
   * @brief generate a random value from the distribution
   * @author K. Zarebski (UKAEA)
   *
   * @param gen generator instance to use for seeding
   * @return double a randomly generated value
   **************************************************************************/
  double generate(std::mt19937 *gen) {
    std::gamma_distribution<> dis_{k(), 1. / theta()};
    return dis_(*gen);
  }
};
}; // namespace FDP

#endif