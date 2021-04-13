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
#include "DataFrame/DataFrame.h"

#include <iostream>

#define NDIMS 2

#include "scrc/utilities/logging.hxx"
#include "scrc/registry/data_object.hxx"
#include "scrc/objects/distributions.hxx"
#include "scrc/objects/data.hxx"

using namespace H5;

namespace SCRC
{
    YAML::Node parse_config_();

    using dataframe = hmdf::StdDataFrame<unsigned int>;

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

        const std::string array_key_ = key.string() + "/array";
        APILogger->debug("FileSystem:ReadArray: Reading key '{0}'", array_key_);

        const H5File file_(var_address.c_str(), H5F_ACC_RDONLY);
        const DataSet data_set_ = file_.openDataSet(array_key_.c_str());
        const DataSpace data_space_ = data_set_.getSpace();

        hsize_t dim_[NDIMS];

        data_space_.getSimpleExtentDims(dim_, NULL);

        const std::vector<std::string> dim_1_name = std::vector<std::string>(static_cast<int>(dim_[0]), "a");
        const std::vector<std::string> dim_2_name = std::vector<std::string>(static_cast<int>(dim_[1]), "b");

        hsize_t mem_dim_ = 1;

        for(int i{0}; i < NDIMS; ++i) mem_dim_ *= dim_[i];

        std::vector<T> data_out_(mem_dim_);

        if(std::is_same<T, float>::value)
        {
            APILogger->debug("FileSystem:ReadArray: Reading HDF5 to float");
            data_set_.read(data_out_.data(), PredType::NATIVE_FLOAT, data_space_, data_space_);
        }

        else if(std::is_same<T, double>::value)
        {
            APILogger->debug("FileSystem:ReadArray: Reading HDF5 to double");
            data_set_.read(data_out_.data(), PredType::NATIVE_DOUBLE, data_space_, data_space_);
        }

        else 
        {
            APILogger->debug("FileSystem:ReadArray: Reading HDF5 to int");
            data_set_.read(data_out_.data(), PredType::NATIVE_INT, data_space_, data_space_);
        }

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

        APILogger->debug("FileSystem:ReadArray: Read Successful.");

        return ArrayObject<T>{"dim_1", "dim_2", dim_1_name, dim_2_name, array_};
    }

    enum FILE_TYPE
    {
        TOML,
        HDF5,
        OTHER
    };
};

#endif