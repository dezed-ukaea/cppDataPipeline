#ifndef __SCRC_DATATABLE_HXX__
#define __SCRC_DATATABLE_HXX__

#include <algorithm>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "scrc/utilities/logging.hxx"

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
  const std::string name() const { return name_; }
  int size() const { return values_.size(); }

  T operator[](int index) {
    if (index > size()) {
      APILogger->error("The requested index exceeds the column size: {0} > {1}",
                       index, size());
      throw std::invalid_argument("Index exceeds column size");
    }
    return values_.operator[](index);
  }

  T operator[](const std::string &key) {
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

class DataTable {
private:
  std::map<std::string, DataTableColumn<int> *> int_cols_;
  std::map<std::string, DataTableColumn<std::string> *> str_cols_;
  std::map<std::string, DataTableColumn<float> *> float_cols_;
  std::vector<std::string> row_names_;
  int size_ = 0;
  int ncols_ = 0;

public:
  DataTable() {}

  unsigned int const size() { return size_; }
  std::vector<std::string> get_row_names() const { return row_names_; }

  void add_column(DataTableColumn<int> *column) {
    if (ncols_ > 0 && column->size() != size_) {
      APILogger->error(
          "Column '{0}' does not match size of data table: {1} != {2}",
          column->name(), column->size(), size_);
      throw std::invalid_argument("Invalid column size for data table");
    }
    int_cols_[column->name()] = column;

    if (ncols_ == 0 && row_names_.empty()) {
      for (int i{0}; i < column->size(); ++i)
        row_names_.push_back(std::to_string(i + 1));
      size_ = row_names_.size();
    }

    ncols_ += 1;
  }

  void add_column(DataTableColumn<float> *column) {
    if (ncols_ > 0 && column->size() != size_) {
      APILogger->error(
          "Column '{0}' does not match size of data table: {1} != {2}",
          column->name(), column->size(), size_);
      throw std::invalid_argument("Invalid column size for data table");
    }
    float_cols_[column->name()] = column;

    if (ncols_ == 0 && row_names_.empty()) {
      for (int i{0}; i < column->size(); ++i)
        row_names_.push_back(std::to_string(i + 1));
      size_ = row_names_.size();
    }

    ncols_ += 1;
  }

  void add_column(DataTableColumn<std::string> *column) {
    if (ncols_ > 0 && column->size() != size_) {
      APILogger->error(
          "Column '{0}' does not match size of data table: {1} != {2}",
          column->name(), column->size(), size_);
      throw std::invalid_argument("Invalid column size for data table");
    }
    str_cols_[column->name()] = column;

    if (ncols_ == 0 && row_names_.empty()) {
      for (int i{0}; i < column->size(); ++i)
        row_names_.push_back(std::to_string(i + 1));
      size_ = row_names_.size();
    }

    ncols_ += 1;
  }

  DataTableColumn<int> *get_int_column(const std::string &col_name) {
    if (int_cols_.find(col_name) == int_cols_.end()) {
      APILogger->error("Column '{0}' either does not exist in data table, or "
                       "is not of type 'int'",
                       col_name);
      throw std::invalid_argument("Failed to retrieve data table column");
    }
    return int_cols_[col_name];
  }

  DataTableColumn<std::string> *get_string_column(const std::string &col_name) {
    if (str_cols_.find(col_name) == str_cols_.end()) {
      APILogger->error("Column '{0}' either does not exist in data table, or "
                       "is not of type 'string'",
                       col_name);
      throw std::invalid_argument("Failed to retrieve data table column");
    }
    return str_cols_[col_name];
  }

  DataTableColumn<float> *get_float_column(const std::string &col_name) {
    if (float_cols_.find(col_name) == float_cols_.end()) {
      APILogger->error("Column '{0}' either does not exist in data table, or "
                       "is not of type 'float'",
                       col_name);
      throw std::invalid_argument("Failed to retrieve data table column");
    }
    return float_cols_[col_name];
  }

  void delete_column(const std::string &col_name);

  template <typename T>
  void add_column(const std::string &header, const std::vector<T> &column_vals,
                  const std::string &unit = "") {
    DataTableColumn<T> *new_col_ =
        new DataTableColumn(header, unit, row_names_, column_vals);
    add_column(new_col_);
  }

  std::map<std::string, DataTableColumn<int> *> get_int_columns() const {
    return int_cols_;
  }

  std::map<std::string, DataTableColumn<float> *> get_float_columns() const {
    return float_cols_;
  }

  std::map<std::string, DataTableColumn<std::string> *>
  get_str_columns() const {
    return str_cols_;
  }

  std::string print() const;
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
    int flat_index_ = co_ords[0];

    if (dimensions_.size() != co_ords.size()) {
      throw std::invalid_argument(
          "Number of coordinates must match number of dimensions");
    }

    int factor_ = 1;

    for (int i{1}; i < dimensions_.size(); ++i) {
      if (co_ords[i] > dimensions_[i] - 1) {
        throw std::invalid_argument(
            "Coordinate[" + std::to_string(i) +
            "] exceeds dimension index limit: " + std::to_string(co_ords[i]) +
            " > " + std::to_string(dimensions_[i] - 1));
      }

      factor_ *= dimensions_[i - 1];

      flat_index_ += factor_ * co_ords[i];
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
  std::vector<int> get_dimensions() const { return dimensions_; }
  std::vector<T> get_values() const { return array_; }
};
}; // namespace SCRC

#endif