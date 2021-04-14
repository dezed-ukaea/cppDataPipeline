#ifndef __SCRC_FILESYSTEM_HXX__
#define __SCRC_FILESYSTEM_HXX__

#include <string>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <yaml-cpp/yaml.h>
#include <cstdlib>
#include "toml.hpp"
#include "H5Cpp.h"

#include <iostream>

#include "scrc/utilities/logging.hxx"
#include "scrc/utilities/hdf5.hxx"
#include "scrc/registry/data_object.hxx"
#include "scrc/objects/distributions.hxx"
#include "scrc/objects/data.hxx"

using namespace H5;

namespace SCRC
{
    YAML::Node parse_config_();
    class LocalFileSystem
    {
        private:
            const std::filesystem::path config_path_;
            const YAML::Node config_data_;
            YAML::Node meta_data_() const;
        public:
            LocalFileSystem(std::filesystem::path config_file_path);
            std::filesystem::path get_data_store() const;
            std::vector<ReadObject::DataProduct*> read_data_products() const;
            std::string get_default_input_namespace() const;
            std::string get_default_output_namespace() const;

    };

    std::string get_first_key_(const toml::value data_table);
    Distribution* construct_dis_(const toml::value data_table);

    double read_point_estimate(const std::filesystem::path var_address);
    Distribution* read_distribution(const std::filesystem::path var_address);

    template<typename T>
    ArrayObject<T> read_array(const std::filesystem::path var_address, const std::filesystem::path key)
    {
        APILogger->debug("FileSystem:ReadArray: Opening file '{0}'", var_address.string());
        if(!std::filesystem::exists(var_address))
        {
            throw std::runtime_error("Could not open HDF5 file '"+var_address.string()+"', file does not exist.");
        }

        const H5File file_(var_address.c_str(), H5F_ACC_RDONLY);


        // -------------------------------------- ARRAY RETRIEVAL -------------------------------------------- //

        const std::string array_key_ = key.string() + "/array";
        APILogger->debug("FileSystem:ReadArray: Reading key '{0}'", array_key_);

        const DataSet data_set_ = file_.openDataSet(array_key_.c_str());
        const DataSpace data_space_ = data_set_.getSpace();

        const int arr_dims_ = data_space_.getSimpleExtentNdims();

        hsize_t dim_[arr_dims_];

        data_space_.getSimpleExtentDims(dim_, NULL);

        hsize_t mem_dim_ = 1;

        for(int i{0}; i < arr_dims_; ++i) mem_dim_ *= dim_[i];

        std::vector<T> data_out_(mem_dim_);

        const DataType &dtype_ = *HDF5::get_hdf5_type<T>();

        data_set_.read(data_out_.data(), dtype_, data_space_, data_space_);

        // -------------------------------------- TITLE RETRIEVAL -------------------------------------------- //

        const int n_titles_ = arr_dims_;

        std::vector<std::string> titles_(n_titles_, "");

        for(int i{0}; i < n_titles_; ++i)
        {
            const std::string title_key_ = key.string() + "/Dimension_" + std::to_string(i+1) + "_title";
            const DataSet title_set_ = file_.openDataSet(title_key_.c_str());
            const DataSpace title_space_ = title_set_.getSpace();
            const DataType dtype_ = title_set_.getDataType();

            if(dtype_.getClass() != H5T_STRING)
            {
                throw std::runtime_error("Type for dimension "+std::to_string(i+1)+" title is not string");
            }

            std::string title_;

            title_set_.read(title_, dtype_);

            titles_[i] = title_;
        }

        // -------------------------------------- NAMES RETRIEVAL -------------------------------------------- //

        const int n_name_sets_ = arr_dims_;

        std::vector<std::vector<std::string>> names_;

        for(int i{0}; i < n_name_sets_; ++i)
        {
            const std::string names_key_ = key.string() + "/Dimension_" + std::to_string(i+1) + "_names";
            const DataSet names_set_ = file_.openDataSet(names_key_.c_str());
            const DataSpace names_space_ = names_set_.getSpace();
            const DataType dtype_ = names_set_.getDataType();

            std::vector<const char*> names_i_(dim_[i], "");
            std::vector<std::string> names_i_str_;

            names_set_.read(names_i_.data(), dtype_, names_space_, names_space_);

            for(auto& c : names_i_) names_i_str_.push_back(std::string(c)); 

            names_.push_back(names_i_str_);
        }

        std::vector<int> dimensions_;

        for(auto& d : dim_) dimensions_.push_back(d);

        APILogger->debug("FileSystem:ReadArray: Read Successful.");

        return ArrayObject<T>{titles_, names_, dimensions_, data_out_};
    }
};

#endif