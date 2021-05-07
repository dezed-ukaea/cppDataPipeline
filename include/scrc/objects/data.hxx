/*! **************************************************************************
 * @file scrc/objects/data.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-05
 * @brief File containing data object classes for storing API objects
 *
 * The classes within this file are used to store objects read from the SCRC
 * API, keeping metadata alongside values.
 ****************************************************************************/

#ifndef __SCRC_DATA_HXX__
#define __SCRC_DATA_HXX__

#include <algorithm>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "scrc/utilities/logging.hxx"

/*! **************************************************************************
 * @namespace SCRC
 * @brief namespace containing all SCRC API library methods and variables
 * @author K. Zarebski (UKAEA)
 *
 * The SCRC namespace holds all of the C++ API methods, which include reading
 * and writing to the RestAPI, storing of data into objects, file access and
 * handling of metadata.
 *
 ****************************************************************************/
namespace SCRC {

/*! **************************************************************************
 * @class DataTableColumn
 * @brief stores an array of values representing a single column from a table
 * @author K. Zarebski (UKAEA)
 *
 * @tparam T type of the column data
 *
 * The DataTableColumn class allows the values from a single column read from
 * a data table stored within a HDF5 file to be contained alongside the
 * metadata (titles etc.). The class requires a type argument indicating the
 * type of the array contained.
 ****************************************************************************/
template <typename T> class DataTableColumn {
private:
  const std::string name_;
  const std::string unit_;
  const std::vector<std::string> row_names_;
  const std::vector<T> values_;

public:
  /*! ************************************************************************
   * @brief Construct an empty DataTableColumn with no data and no metadata
   * @author K. Zarebski (UKAEA)
   *
   **************************************************************************/
  DataTableColumn() {}

  /*! ************************************************************************
   * @brief Construct a new Data Table Column object
   * @author K. Zarebski (UKAEA)
   *
   * @param name the name/title of the column
   * @param unit the unit of measurement for the column data
   * @param row_names a vector containing labels for each row element
   * @param values the values of the data column
   ***************************************************************************/
  DataTableColumn(std::string name, std::string unit,
                  const std::vector<std::string> &row_names,
                  const std::vector<T> &values)
      : name_(name), unit_(unit), row_names_(row_names), values_(values) {}

  /*! *************************************************************************
   * @brief returns the values from the given data column
   * @author K. Zarebski (UKAEA)
   *
   * @return value data as vector
   ***************************************************************************/
  const std::vector<T> values() const { return values_; }

  /*! *************************************************************************
   * @brief returns the name/title of the data column
   * @author K. Zarebski (UKAEA)
   *
   * @return name of data column
   ***************************************************************************/
  const std::string name() const { return name_; }

  /*! *************************************************************************
   * @brief returns the number of entries within the data column
   * @author K. Zarebski (UKAEA)
   *
   * @return length of data column
   ***************************************************************************/
  int size() const { return values_.size(); }

  /*! *************************************************************************
   * @brief retrieves the value at the given index
   * @author K. Zarebski (UKAEA)
   *
   * @param index data column index
   * @return value at the index position
   ****************************************************************************/
  T operator[](int index) {
    if (index > size()) {
      APILogger->error("The requested index exceeds the column size: {0} > {1}",
                       index, size());
      throw std::invalid_argument("Index exceeds column size");
    }
    return values_.operator[](index);
  }

  /*! **************************************************************************
   * @brief retrieve a value from the data column by row label
   * @author K. Zarebski (UKAEA)
   *
   * @param key label of the row
   * @return value at the relevant index
   ****************************************************************************/
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

/*! ***************************************************************************
 * @class DataTable
 * @brief this class allows the storage of integer, float and string columned
 *data within a single object. The three types are stored separately.
 * @author K. Zarebski (UKAEA)
 *
 * @paragraph testcases Test Case
 *    `test/test_objects.cxx`: TestDataTableCreate
 *
 *    This unit test checks that the DataTable initialisation method works
 *     correctly
 *    @snippet `test/test_objects.cxx TestDataTableCreate
 *
 *****************************************************************************/
class DataTable {
private:
  std::map<std::string, DataTableColumn<int> *> int_cols_;
  std::map<std::string, DataTableColumn<std::string> *> str_cols_;
  std::map<std::string, DataTableColumn<float> *> float_cols_;
  std::vector<std::string> row_names_;
  int size_ = 0;
  int ncols_ = 0;

public:
  /*! ************************************************************************
   * @brief Construct an empty DataTable object
   * @author K. Zarebski (UKAEA)
   *
   **************************************************************************/
  DataTable() {}

  /*! ************************************************************************
   * @brief returns the size of the data table which is the number of entries
   * @author K. Zarebski (UKAEA)
   *
   * @return total number of rows
   **************************************************************************/
  unsigned int size() const { return size_; }

  /*! ************************************************************************
   * @brief retrieve the labels for the column rows
   * @author K. Zarebski (UKAEA)
   *
   * @return label list
   **************************************************************************/
  std::vector<std::string> get_row_names() const { return row_names_; }

  /*! ************************************************************************
   * @brief add a column of integers to the data table
   * @author K. Zarebski (UKAEA)
   *
   * Appends a DataTableColumn of integers to the data table, if the existing
   * table is not empty, the vector must match the size of the table,
   * else an exception will be thrown.
   *
   * @exception std::invalid_argument if the new vector does not match
   * existing table size
   * @param column a DataTableColumn of integers to append to table
   **************************************************************************/
  void add_column(DataTableColumn<int> *column) {
    if (int_cols_.find(column->name()) != int_cols_.end() ||
        float_cols_.find(column->name()) != float_cols_.end() ||
        str_cols_.find(column->name()) != str_cols_.end()) {
      APILogger->error("Cannot add column '{0}' to data table, column exists!");
      throw std::invalid_argument("Cannot append data to data table");
    }

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

  /*! ************************************************************************
   * @brief add a column of floats to the data table
   * @author K. Zarebski (UKAEA)
   *
   * Appends a DataTableColumn of floats to the data table, if the existing
   * table is not empty, the vector must match the size of the table,
   * else an exception will be thrown.
   *
   * @exception std::invalid_argument if the new vector does not match
   * existing table size
   * @param column a DataTableColumn of floats to append to table
   **************************************************************************/
  void add_column(DataTableColumn<float> *column) {
    if (int_cols_.find(column->name()) != int_cols_.end() ||
        float_cols_.find(column->name()) != float_cols_.end() ||
        str_cols_.find(column->name()) != str_cols_.end()) {
      APILogger->error("Cannot add column '{0}' to data table, column exists!");
      throw std::invalid_argument("Cannot append data to data table");
    }
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

  /*! ************************************************************************
   * @brief add a column of strings to the data table
   * @author K. Zarebski (UKAEA)
   *
   * Appends a DataTableColumn of strings to the data table, if the existing
   * table is not empty, the vector must match the size of the table,
   * else an exception will be thrown.
   *
   * @exception std::invalid_argument if the new vector does not match
   * existing table size
   * @param column a DataTableColumn of strings to append to table
   **************************************************************************/
  void add_column(DataTableColumn<std::string> *column) {
    if (int_cols_.find(column->name()) != int_cols_.end() ||
        float_cols_.find(column->name()) != float_cols_.end() ||
        str_cols_.find(column->name()) != str_cols_.end()) {
      APILogger->error("Cannot add column '{0}' to data table, column exists!");
      throw std::invalid_argument("Cannot append data to data table");
    }
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

  /*! **************************************************************************
   * @brief append a column to the data table from a vector
   * @author K. Zarebski (UKAEA)
   *
   * @tparam T the type of the input data
   * @param header the name/title to be assigned to the column
   * @param column_vals the values to append
   * @param unit the unit of measurement associated with the input data
   ****************************************************************************/
  template <typename T>
  void add_column(const std::string &header, const std::vector<T> &column_vals,
                  const std::string &unit = "") {
    if (int_cols_.find(header) != int_cols_.end() ||
        float_cols_.find(header) != float_cols_.end() ||
        str_cols_.find(header) != str_cols_.end()) {
      APILogger->error("Cannot add column '{0}' to data table, column exists!");
      throw std::invalid_argument("Cannot append data to data table");
    }
    DataTableColumn<T> *new_col_ =
        new DataTableColumn(header, unit, row_names_, column_vals);
    add_column(new_col_);
  }

  /*!****************************************************************************
   * @brief retrieve by name/title a column of integers from the data table
   * @author K. Zarebski (UKAEA)
   *
   * @param col_name the name/title of the column within the table
   * @return the named data table column
   *****************************************************************************/
  DataTableColumn<int> *get_int_column(const std::string &col_name) {
    if (int_cols_.find(col_name) == int_cols_.end()) {
      APILogger->error("Column '{0}' either does not exist in data table, or "
                       "is not of type 'int'",
                       col_name);
      throw std::invalid_argument("Failed to retrieve data table column");
    }
    return int_cols_[col_name];
  }

  /*!
   ****************************************************************************
   * @brief retrieve by name/title a column of strings from the data table
   * @author K. Zarebski (UKAEA)
   *
   * @param col_name the name/title of the column within the table
   * @return the named data table column
   *****************************************************************************/
  DataTableColumn<std::string> *get_string_column(const std::string &col_name) {
    if (str_cols_.find(col_name) == str_cols_.end()) {
      APILogger->error("Column '{0}' either does not exist in data table, or "
                       "is not of type 'string'",
                       col_name);
      throw std::invalid_argument("Failed to retrieve data table column");
    }
    return str_cols_[col_name];
  }

  /*!
   ****************************************************************************
   * @brief retrieve by name/title a column of floats from the data table
   * @author K. Zarebski (UKAEA)
   *
   * @param col_name the name/title of the column within the table
   * @return the named data table column
   *****************************************************************************/
  DataTableColumn<float> *get_float_column(const std::string &col_name) {
    if (float_cols_.find(col_name) == float_cols_.end()) {
      APILogger->error("Column '{0}' either does not exist in data table, or "
                       "is not of type 'float'",
                       col_name);
      throw std::invalid_argument("Failed to retrieve data table column");
    }
    return float_cols_[col_name];
  }

  /*! **************************************************************************
   * @brief remove by name/title a column from the data table
   * @author K. Zarebski (UKAEA)
   *
   * @param col_name the name/title of the column to remove
   ****************************************************************************/
  void delete_column(const std::string &col_name);

  /*! *************************************************************************
   * @brief retrieve all integer columns from the data table
   * @author K. Zarebski (UKAEA)
   *
   * Returns all stored integer columns as a map the keys of which are the
   * names/titles of the columns within the table.
   *
   * @return a map of all integer columns
   ***************************************************************************/
  std::map<std::string, DataTableColumn<int> *> get_int_columns() const {
    return int_cols_;
  }

  /*! *************************************************************************
   * @brief retrieve all float columns from the data table
   * @author K. Zarebski (UKAEA)
   *
   * Returns all stored float columns as a map the keys of which are the
   * names/titles of the columns within the table.
   *
   * @return a map of all float columns
   ***************************************************************************/
  std::map<std::string, DataTableColumn<float> *> get_float_columns() const {
    return float_cols_;
  }

  /*! *************************************************************************
   * @brief retrieve all string columns from the data table
   * @author K. Zarebski (UKAEA)
   *
   * Returns all stored string columns as a map the keys of which are the
   * names/titles of the columns within the table.
   *
   * @return a map of all string columns
   ***************************************************************************/
  std::map<std::string, DataTableColumn<std::string> *>
  get_str_columns() const {
    return str_cols_;
  }

  /*! ************************************************************************
   * @brief constructs a string representation of the table which can be
   * printed to std::out
   * @author K. Zarebski (UKAEA)
   *
   * @return table as string
   **************************************************************************/
  std::string print() const;
};

/*! *************************************************************************
 * @class ArrayObject
 * @brief class to store an array of values from the SCRC pipeline alongside
 * the associated metadata
 * @author K. Zarebski (UKAEA)
 *
 * The ArrayObject class stores the values from an SCRC pipeline array type
 * with properties such as the titles of the dimensions of the array, and names
 * of positions along each of the dimension axes.
 *
 * @tparam T the type of the array values stored
 ***************************************************************************/
template <typename T> class ArrayObject {
private:
  const std::vector<std::string> dimension_titles_;
  const std::vector<std::vector<std::string>> dimension_names_;
  const std::vector<int> dimensions_ = {-1};
  const std::vector<T> array_;

public:
  /*! **********************************************************************
   * @brief constructs an empty ArrayObject instance
   * @author K. Zarebski (UKAEA)
   *
   ************************************************************************/
  ArrayObject() {}

  /*! **********************************************************************
   * @brief constructs an array from a vector of values and given metadata
   * @author K. Zarebski (UKAEA)
   *
   * @tparam T the type of the array values to store
   * @param dim_titles the names of the dimensions
   * @param dim_names the labels for each position in each dimension
   * @param size the dimensions of the array
   * @param values the values to insert
   ***********************************************************************/
  ArrayObject(std::vector<std::string> dim_titles,
              std::vector<std::vector<std::string>> &dim_names,
              std::vector<int> &size, std::vector<T> &values)
      : dimension_titles_(dim_titles), dimension_names_(dim_names),
        dimensions_(size), array_(values) {}

  /*! *********************************************************************
   * @brief retrieve an element from the array by giving its coordinates
   * @author K. Zarebski (UKAEA)
   *
   * @param co_ords co-ordinate of the element to retrieve
   * @return the element within the array at the given position
   ***********************************************************************/
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

  /*! *********************************************************************
   * @brief retrieve the title for the given dimension
   * @author K. Zarebski (UKAEA)
   *
   * @param dimension index of the dimension
   * @return the title of the dimension
   ***********************************************************************/
  std::string get_title(unsigned int dimension) const {
    if (dimension >= dimension_titles_.size()) {
      throw std::invalid_argument("Dimension " + std::to_string(dimension) +
                                  " exceeds array rank " +
                                  std::to_string(rank()));
    }

    return dimension_titles_[dimension];
  }

  /*! ********************************************************************
   * @brief retrieve the names of each position in the specified dimension
   * @author K. Zarebski (UKAEA)
   *
   * @param dimension index of the dimension
   * @return vector of labels for that dimension
   **********************************************************************/
  std::vector<std::string> get_dimension_names(unsigned int dimension) const {
    if (dimension >= dimension_titles_.size()) {
      throw std::invalid_argument("Dimension " + std::to_string(dimension) +
                                  " exceeds array rank " +
                                  std::to_string(rank()));
    }

    return dimension_names_[dimension];
  }

  /*! *******************************************************************
   * @brief returns the rank of the array data
   * @author K. Zarebski (UKAEA)
   *
   * @return array rank
   *********************************************************************/
  int rank() const { return dimensions_.size(); }

  /*! *******************************************************************
   * @brief returns the dimensions of the array
   * @author K. Zarebski (UKAEA)
   *
   * @return vector of dimensions
   *********************************************************************/
  std::vector<int> get_dimensions() const { return dimensions_; }

  /*! *******************************************************************
   * @brief retrieves the array itself as a 1D vector of values
   * @author K. Zarebski (UKAEA)
   *
   * @return all array values as a vector
   *********************************************************************/
  std::vector<T> get_values() const { return array_; }
};
}; // namespace SCRC

#endif