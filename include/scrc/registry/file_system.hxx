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

        const DataType &dtype = *HDF5::get_hdf5_type<T>();

        data_set_.read(data_out_.data(), dtype, data_space_, data_space_);

        std::vector<std::vector<T>> array_;

        if(data_out_.size() < 1)
        {
            throw std::runtime_error("Failed to extract array from HDF5 file, output vector is empty");
        }

        if(dim_[1] == 0)
        {
            array_ = std::vector<std::vector<T>>({data_out_});
        }

        else
        {
            array_ = std::vector<std::vector<T>>(static_cast<int>(dim_[1]), std::vector<T>(static_cast<int>(dim_[0]), 0));

            int j = 0;
            int k = 0;

            for(unsigned int i{0}; i < data_out_.size(); ++i)
            {
                if(i > 0 && i % dim_[0] == 0)
                {
                    j += 1;
                    k = 0;
                }
                array_[k][j] = data_out_[i];
                k += 1;
            }
        }

        // -------------------------------------- TITLE RETRIEVAL -------------------------------------------- //

        const int n_titles_ = arr_dims_;

        std::vector<std::string> titles_(n_titles_, "");

        const std::vector<std::string> dim_1_name = std::vector<std::string>(static_cast<int>(dim_[0]), "a");
        const std::vector<std::string> dim_2_name = std::vector<std::string>(static_cast<int>(dim_[1]), "b");

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

        APILogger->debug("FileSystem:ReadArray: Read Successful.");

        return ArrayObject<T>{titles_, dim_1_name, dim_2_name, array_};
    }
};

#endif