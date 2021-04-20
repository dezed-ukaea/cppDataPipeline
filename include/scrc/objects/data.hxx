#ifndef __SCRC_DATATABLE_HXX__
#define __SCRC_DATATABLE_HXX__

#include <stdexcept>
#include <string>
#include <vector>

namespace SCRC {
template <typename T> class DataTableColumn {
private:
  const std::string name_;
  const std::string unit_;
  const std::vector<std::string> row_names_;
  const std::vector<T> values_;

public:
  DataTableColumn() {}
  DataTableColumn(std::string name, std::string unit,
                  const std::vector<std::string> &row_names,
                  const std::vector<T> &values)
      : name_(name), unit_(unit), row_names_(row_names), values_(values) {}

  const std::vector<T> values() const { return values_; }

  T operator[](const char *key) {
    if (row_names_.empty()) {
      throw std::runtime_error(
          "Cannot access label-less data table column by label");
    }

    const auto iterator_ =
        std::find(row_names_.begin(), row_names_.end(), std::string(key));
    if (iterator_ == row_names_.end()) {
      throw std::invalid_argument("No row name '" + std::string(key) +
                                  "' in column '" + name_ + "'");
    }

    const int distance_ = std::distance(row_names_.begin(), iterator_);

    return values_.operator[](distance_);
  }
};

template <typename T> class ArrayObject {
private:
  const std::vector<std::string> dimension_titles_;
  const std::vector<std::vector<std::string>> dimension_names_;
  const std::vector<int> dimensions_ = {-1};
  const std::vector<T> array_;

public:
  ArrayObject() {}
  ArrayObject(std::vector<std::string> dim_titles,
              std::vector<std::vector<std::string>> &dim_names,
              std::vector<int> &size, std::vector<T> &values)
      : dimension_titles_(dim_titles), dimension_names_(dim_names),
        dimensions_(size), array_(values) {}
  T get(std::vector<int> co_ords) const {
    int flat_index_ = 0;

    if (dimensions_.size() != co_ords.size()) {
      throw std::invalid_argument(
          "Number of coordinates must match number of dimensions");
    }

    for (int i{0}; i < dimensions_.size(); ++i) {
      if (co_ords[i] > dimensions_[i] - 1) {
        throw std::invalid_argument(
            "Coordinate[" + std::to_string(i) +
            "] exceeds dimension index limit: " + std::to_string(co_ords[i]) +
            " > " + std::to_string(dimensions_[i] - 1));
      }

      flat_index_ += dimensions_[i] * co_ords[i];
    }

    return array_[flat_index_];
  }

  std::string get_title(unsigned int dimension) const {
    if (dimension >= dimension_titles_.size()) {
      throw std::invalid_argument("Dimension " + std::to_string(dimension) +
                                  " exceeds array rank " +
                                  std::to_string(rank()));
    }

    return dimension_titles_[dimension];
  }

  std::vector<std::string> get_dimension_names(unsigned int dimension) const {
    if (dimension >= dimension_titles_.size()) {
      throw std::invalid_argument("Dimension " + std::to_string(dimension) +
                                  " exceeds array rank " +
                                  std::to_string(rank()));
    }

    return dimension_names_[dimension];
  }

  int rank() const { return dimensions_.size(); }
};
}; // namespace SCRC

#endif